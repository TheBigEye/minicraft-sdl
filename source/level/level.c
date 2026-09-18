/*
 * level.c - Implementation of the Level class.
 *
 * Generation by depth, tile and data access, entity bookkeeping (a list
 * plus one bucket per tile), tile and sprite rendering, the light map and
 * the simulation tick with its spawn attempt.
 */
#include "level.h"

#include <stdlib.h>
#include <string.h>

#include "levelgen/levelgen.h"
#include "tile/tile.h"

#include "../entity/airwizard.h"
#include "../entity/slime.h"
#include "../entity/zombie.h"
#include "../gfx/screen.h"
#include "../log.h"
#include "../utils/arraylist.h"
#include "../utils/javarandom.h"
#include "../utils/utils.h"


/*
 * Painter's order for qsort: lower entities are drawn last, so they cover
 * the ones behind them. Java: the private Comparator field spriteSorter.
 */
PRIVATE int level_sprite_sorter(const void* a, const void* b) {
    Entity* e0 = *(Entity**) a;
    Entity* e1 = *(Entity**) b;

    return e0->y - e1->y;
}


/*
 * Generates the level for its depth and, when there is a parent, turns the
 * parent's down-stairs into up-stairs here, ringed with hard rock on the
 * surface or dirt underground so that they stay reachable.
 */
PUBLIC void level_create(Level* this, int w, int h, int depth, Level* parent) {
    this->render_background = level_render_background;
    this->render_sprites    = level_render_sprites;
    this->render_light      = level_render_light;
    this->sort_and_render   = level_sort_and_render;
    this->get_tile          = level_get_tile;
    this->set_tile          = level_set_tile;
    this->get_data          = level_get_data;
    this->set_data          = level_set_data;
    this->add               = level_add;
    this->remove            = level_remove;
    this->insert_entity     = level_insert_entity;
    this->remove_entity     = level_remove_entity;
    this->get_entities      = level_get_entities;
    this->try_spawn         = level_try_spawn;
    this->tick              = level_tick;
    this->free              = level_free;

    random_create(&this->random);
    random_set_seed(&this->random, get_time_ms());

    arraylist_create(&this->entities);

    this->dirtColor      = depth < 0 ? 222 : 322;
    this->grassColor     = 141;
    this->sandColor      = 550;
    this->monsterDensity = 8;

    this->depth = depth;
    this->w     = w;
    this->h     = h;

    if (depth == 0) {
        create_and_validate_top_map(&this->tiles, &this->data, w, h);

    } else if (depth < 0) {
        create_and_validate_underground_map(&this->tiles, &this->data, w, h, -depth);
        this->monsterDensity = 4;

    } else {
        create_and_validate_sky_map(&this->tiles, &this->data, w, h);
        this->monsterDensity = 4;
    }

    if (parent) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                if (parent->get_tile(parent, x, y) != tiles[STAIRS_DOWN]) continue;

                this->set_tile(this, x, y, tiles[STAIRS_UP], 0);

                /* Safety ring around the stairs. */
                Tile* ring = depth == 0 ? tiles[HARD_ROCK] : tiles[DIRT];

                this->set_tile(this, x - 1, y - 1, ring, 0);
                this->set_tile(this, x + 1, y - 1, ring, 0);
                this->set_tile(this, x - 1, y + 1, ring, 0);
                this->set_tile(this, x + 1, y + 1, ring, 0);
                this->set_tile(this, x - 1, y,     ring, 0);
                this->set_tile(this, x + 1, y,     ring, 0);
                this->set_tile(this, x,     y - 1, ring, 0);
                this->set_tile(this, x,     y + 1, ring, 0);
            }
        }
    }

    this->entitiesInTiles = new_array(ArrayList, w * h);

    for (int i = 0; i < w * h; ++i) {
        arraylist_create(this->entitiesInTiles + i);
    }

    /* The Air Wizard lives in the sky. Java: if (level == 1) add(new AirWizard()) */
    if (depth == 1) {
        AirWizard* wizard = new(AirWizard);
        airwizard_create(wizard);

        wizard->mob.entity.x = w * 8;
        wizard->mob.entity.y = h * 8;

        this->add(this, &wizard->mob.entity);
    }
}


/* Draws every visible tile; each tile draws itself. */
PUBLIC void level_render_background(Level* this, Screen* screen, int xScroll, int yScroll) {
    int xo = xScroll >> 4;
    int yo = yScroll >> 4;

    int w = (screen->w + 15) >> 4;
    int h = (screen->h + 15) >> 4;

    screen->set_offset(screen, xScroll, yScroll);

    for (int y = yo; y <= h + yo; ++y) {
        for (int x = xo; x <= w + xo; ++x) {
            Tile* tile = this->get_tile(this, x, y);
            tile->render(tile, screen, this, x, y);
        }
    }

    screen->set_offset(screen, 0, 0);
}


/* Sorts the row by Y (painter's order) and draws it. Java: sortAndRender */
PUBLIC void level_sort_and_render(Level* this, Screen* screen, ArrayList* list) {
    (void) this;

    if (list->size <= 0) return;

    qsort(list->elements, (size_t) list->size, sizeof(void*), level_sprite_sorter);

    for (int i = 0; i < list->size; ++i) {
        Entity* e = list->elements[i];
        e->render(e, screen);
    }
}


/* Collects the entities that touch the viewport and draws them in order. */
PUBLIC void level_render_sprites(Level* this, Screen* screen, int xScroll, int yScroll) {
    ArrayList rowSprites;
    arraylist_create(&rowSprites);

    int xo = xScroll >> 4;
    int yo = yScroll >> 4;

    int w = (screen->w + 15) >> 4;
    int h = (screen->h + 15) >> 4;

    screen->set_offset(screen, xScroll, yScroll);

    for (int y = yo; y < h + yo; ++y) {
        for (int x = xo; x <= w + xo; ++x) {
            if (x < 0 || y < 0 || x >= this->w || y >= this->h) continue;

            ArrayList* ents = &this->entitiesInTiles[x + y * this->w];

            for (int i = 0; i < ents->size; ++i) {
                rowSprites.add(&rowSprites, ents->elements[i]);
            }
        }

        if (rowSprites.size > 0) {
            this->sort_and_render(this, screen, &rowSprites);
        }

        rowSprites.clear(&rowSprites);
    }

    rowSprites.free(&rowSprites);
    screen->set_offset(screen, 0, 0);
}


/*
 * The light pass, which is the fog of war in caves: start from a dark map,
 * add the light radius of every entity and of the tile itself, and dither
 * the result over the finished screen.
 */
PUBLIC void level_render_light(Level* this, Screen* screen, int xScroll, int yScroll) {
    int xo = xScroll >> 4;
    int yo = yScroll >> 4;

    int w = (screen->w + 15) >> 4;
    int h = (screen->h + 15) >> 4;
    int r = 4;

    screen->set_offset(screen, xScroll, yScroll);

    for (int y = yo - r; y <= h + yo + r; ++y) {
        for (int x = xo - r; x <= w + xo + r; ++x) {
            if (x < 0 || y < 0 || x >= this->w || y >= this->h) continue;

            ArrayList* entities = &this->entitiesInTiles[x + y * this->w];

            for (int i = 0; i < entities->size; ++i) {
                Entity* e  = entities->elements[i];
                int     lr = e->get_light_radius(e);

                if (lr > 0) screen->render_light(screen, e->x - 1, e->y - 4, lr * 8);
            }

            Tile* tile = this->get_tile(this, x, y);
            int   lr   = tile->get_light_radius(tile, this, x, y);

            if (lr > 0) screen->render_light(screen, x * 16 + 8, y * 16 + 8, lr * 8);
        }
    }

    screen->set_offset(screen, 0, 0);
}


/* The tile at (x, y); out of range gives rock, as in the original. */
PUBLIC Tile* level_get_tile(Level* this, int x, int y) {
    if (x < 0 || y < 0 || x >= this->w || y >= this->h) return tiles[ROCK];

    return tiles[this->tiles[x + y * this->w]];
}


/* Replaces the tile and resets its data byte. Java: setTile(x, y, t, dataVal) */
PUBLIC void level_set_tile(Level* this, int x, int y, Tile* t, int dataVal) {
    if (x < 0 || y < 0 || x >= this->w || y >= this->h) return;

    this->tiles[x + y * this->w] = (unsigned char) t->id;
    this->data[x + y * this->w]  = (unsigned char) dataVal;
}


/* The per-tile data byte: wheat growth, ore type, and so on. */
PUBLIC unsigned char level_get_data(Level* this, int x, int y) {
    if (x < 0 || y < 0 || x >= this->w || y >= this->h) return 0;

    return this->data[x + y * this->w];
}


/* Sets the data byte without touching the tile. */
PUBLIC void level_set_data(Level* this, int x, int y, int val) {
    if (x < 0 || y < 0 || x >= this->w || y >= this->h) return;

    this->data[x + y * this->w] = (unsigned char) val;
}


/* Puts the entity in the bucket of the tile it stands on. */
PUBLIC void level_insert_entity(Level* this, int x, int y, Entity* entity) {
    if (x < 0 || y < 0 || x >= this->w || y >= this->h) return;

    ArrayList* bucket = &this->entitiesInTiles[x + y * this->w];
    bucket->add(bucket, entity);
}


/*
 * Adds an entity to the level: initialises it, puts it in the list and
 * registers it in its bucket. From here the level owns the memory.
 */
PUBLIC void level_add(Level* this, Entity* entity) {
    entity->removed = false;

    this->entities.add(&this->entities, entity);
    entity_init(entity, this);

    this->insert_entity(this, entity->x >> 4, entity->y >> 4, entity);
}


/* Removes the entity from the list and from its bucket. */
PUBLIC void level_remove(Level* this, Entity* e) {
    this->entities.remove_element(&this->entities, e);

    int xto = e->x >> 4;
    int yto = e->y >> 4;

    this->remove_entity(this, xto, yto, e);
}


/* Takes the entity out of the bucket of tile (x, y). */
PUBLIC void level_remove_entity(Level* this, int x, int y, Entity* entity) {
    if (x < 0 || y < 0 || x >= this->w || y >= this->h) return;

    ArrayList* bucket = &this->entitiesInTiles[x + y * this->w];
    bucket->remove_element(bucket, entity);
}


/* Adds to `list` every entity whose box overlaps the given rectangle. */
PUBLIC void level_get_entities(Level* this, ArrayList* list, int x0, int y0, int x1, int y1) {
    int xt0 = (x0 >> 4) - 1;
    int yt0 = (y0 >> 4) - 1;
    int xt1 = (x1 >> 4) + 1;
    int yt1 = (y1 >> 4) + 1;

    for (int y = yt0; y <= yt1; ++y) {
        for (int x = xt0; x <= xt1; ++x) {
            if (x < 0 || y < 0 || x >= this->w || y >= this->h) continue;

            ArrayList* entities = &this->entitiesInTiles[x + y * this->w];

            for (int i = 0; i < entities->size; ++i) {
                Entity* e = entities->elements[i];

                if (entity_intersects(e, x0, y0, x1, y1)) {
                    list->add(list, e);
                }
            }
        }
    }
}


/*
 * One level tick (Java: Level.tick): tick a scattering of random tiles,
 * tick every entity while compacting the list (the removed ones are freed
 * here), and try to spawn.
 *
 * Java draws yt with random.nextInt(w) rather than nextInt(h), which is a
 * typo in the original; this port uses h. It makes no difference in play,
 * because every level is square.
 */
PUBLIC void level_tick(Level* this) {
    this->try_spawn(this, 1);

    for (int i = 0; i < this->w * this->h / 50; ++i) {
        int xt = this->random.next_int(&this->random, this->w);
        int yt = this->random.next_int(&this->random, this->h);

        Tile* tile = this->get_tile(this, xt, yt);
        tile->tick(tile, this, xt, yt);
    }

    for (int i = 0; i < this->entities.size; ++i) {
        Entity* e = this->entities.elements[i];

        int xto = e->x >> 4;
        int yto = e->y >> 4;

        e->tick(e);

        if (e->removed) {
            this->entities.remove_at(&this->entities, i--);
            this->remove_entity(this, xto, yto, e);

            if (e->type != PLAYER) {
                e->free(e);
                delete(e);
            }

        } else {
            int xt = e->x >> 4;
            int yt = e->y >> 4;

            if (xto != xt || yto != yt) {
                this->remove_entity(this, xto, yto, e);
                this->insert_entity(this, xt, yt, e);
            }
        }
    }
}


/*
 * Attempts `count` spawns while respecting the density: picks a slime or a
 * zombie at random, at a level drawn from the depth, and places it if
 * there is room.
 *
 * Java lets the mob go when findStartPos() fails, trusting the garbage
 * collector; this port has to free it, or every refused spawn would leak.
 */
PUBLIC void level_try_spawn(Level* this, int count) {
    Random* random = &this->random;

    for (int i = 0; i < count; ++i) {
        Mob* mob = null;

        int minLevel = 1;
        int maxLevel = 1;

        if (this->depth < 0) maxLevel = (-this->depth) + 1;
        if (this->depth > 0) minLevel = maxLevel = 4;

        int lvl = random->next_int(random, maxLevel - minLevel + 1) + minLevel;

        if (random->next_int(random, 2) == 0) {
            mob = (Mob*) new(Slime);
            slime_create((Slime*) mob, lvl);

        } else {
            mob = (Mob*) new(Zombie);
            zombie_create((Zombie*) mob, lvl);
        }

        if (mob->find_start_pos(mob, this)) {
            this->add(this, &mob->entity);

        } else {
            mob->entity.free(&mob->entity);
            delete(mob);
        }
    }
}


/* Destructor: frees the grids, the buckets and the entity list. */
PUBLIC void level_free(Level* this) {
    if (this->tiles) delete(this->tiles);
    if (this->data)  delete(this->data);

    if (this->entities.elements) {
        this->entities.free(&this->entities);
    }

    if (this->entitiesInTiles) {
        for (int i = 0; i < this->w * this->h; ++i) {
            ArrayList* list = this->entitiesInTiles + i;

            for (int j = 0; j < list->size; ++j) {
                Entity* e = list->elements[j];

                if (e->type != PLAYER) {
                    e->free(e);
                    delete(e);
                }
            }

            list->free(list);
        }

        delete(this->entitiesInTiles);
    }
}
