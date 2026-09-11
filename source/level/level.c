/*
 * level.c - Level container and per-frame level work (Java: Level).
 *
 * Generation dispatch by depth, tile/data accessors, entity
 * bookkeeping (list + per-tile buckets), sprite and light rendering
 * and the per-tick update with mob spawning.
 */
#include "level.h"
#include <string.h>
#include <stdlib.h>
#include "levelgen/levelgen.h"
#include "tile/tile.h"

#include "../entity/slime.h"
#include "../entity/zombie.h"
#include "../entity/airwizard.h"
#include "../utils/javarandom.h"
#include "../utils/utils.h"
#include "../gfx/screen.h"
#include "../utils/arraylist.h"


/*
 * Generates a level of the given depth: sky map for positive depth,
 * surface map for 0 and underground maps (scaled by -depth) below.
 * Stairs in the parent level become matching stairs here, ringed by
 * hard rock (surface) or dirt (underground) so they stay reachable.
 */
void level_init(Level* lvl, int w, int h, int level, Level* parent) {
	random_set_seed(&lvl->random, getTimeMS());
	create_arraylist(&lvl->entities);

	lvl->dirtColor = level < 0 ? 222 : 322;
	lvl->grassColor = 141;
	lvl->sandColor = 550;
	lvl->monsterDensity = 8;

	lvl->depth = level;
	lvl->w = w;
	lvl->h = h;

	if (level == 0) {
		createAndValidateTopMap(&lvl->tiles, &lvl->data, w, h);
	} else if(level < 0) {
		createAndValidateUndergroundMap(&lvl->tiles, &lvl->data, w, h, -level);
		lvl->monsterDensity = 4;
	} else {
		createAndValidateSkyMap(&lvl->tiles, &lvl->data, w, h);
		lvl->monsterDensity = 4;
	}

	if (parent) {
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				if (level_get_tile(parent, x, y) == STAIRS_DOWN) {
					level_set_tile(lvl, x, y, STAIRS_UP, 0);

					if (level == 0) {
						level_set_tile(lvl, x - 1, y, (int) HARD_ROCK, 0);
						level_set_tile(lvl, x + 1, y, (int) HARD_ROCK, 0);
						level_set_tile(lvl, x, y - 1, (int) HARD_ROCK, 0);
						level_set_tile(lvl, x, y + 1, (int) HARD_ROCK, 0);
						level_set_tile(lvl, x - 1, y - 1, (int) HARD_ROCK, 0);
						level_set_tile(lvl, x - 1, y + 1, (int) HARD_ROCK, 0);
						level_set_tile(lvl, x + 1, y - 1, (int) HARD_ROCK, 0);
						level_set_tile(lvl, x + 1, y + 1, (int) HARD_ROCK, 0);
					}else{
						level_set_tile(lvl, x - 1, y, (int) DIRT, 0);
						level_set_tile(lvl, x + 1, y, (int) DIRT, 0);
						level_set_tile(lvl, x, y - 1, (int) DIRT, 0);
						level_set_tile(lvl, x, y + 1, (int) DIRT, 0);
						level_set_tile(lvl, x - 1, y - 1, (int) DIRT, 0);
						level_set_tile(lvl, x - 1, y + 1, (int) DIRT, 0);
						level_set_tile(lvl, x + 1, y - 1, (int) DIRT, 0);
						level_set_tile(lvl, x + 1, y + 1, (int) DIRT, 0);
					}
				}
			}
		}
	}

	lvl->entitiesInTiles = malloc(sizeof(ArrayList) * w * h);
	for (int i = 0; i < w * h; ++i) {
		create_arraylist(lvl->entitiesInTiles + i);
	}

	if (level == 1) {
		AirWizard* wizard = malloc(sizeof(AirWizard));
		airwizard_create(wizard);
		wizard->mob.entity.x = w * 8;
		wizard->mob.entity.y = h * 8;
		level_addEntity(lvl, &wizard->mob.entity);
	}
}


/* Draws every tile inside the scroll window, row by row; each tile
 * renders itself through its behavior's render hook. */
void level_renderBackground(Level* level, Screen* screen, int xScroll, int yScroll) {
	int xo = xScroll >> 4;
	int yo = yScroll >> 4;

	int w = (screen->w + 15) >> 4;
	int h = (screen->h + 15) >> 4;

	screen_set_offset(screen, xScroll, yScroll);

	for (int y = yo; y <= h + yo; ++y) {
		for (int x = xo; x <= w + xo; ++x) {
			unsigned char tile = level_get_tile(level, x, y);
			tile_render(tile, screen, level, x, y);
		}
	}

	screen_set_offset(screen, 0, 0);
}


/* qsort comparator: painter's order by y so lower entities draw
 * on top of higher ones. */
int _cmpEnt(const void* ent, const void* ent2) {
	Entity* e = *(Entity**) ent;
	Entity* e2 = *(Entity**) ent2;

	if (e2->y < e->y) return 1;
	if (e2->y > e->y) return -1;
	return 0;
}


/* Sorts the visible entity list by y and renders them in order. */
void level_sortAndRender(Level* level, Screen* screen, ArrayList* list) {
	qsort(list->elements, list->size, sizeof(*list->elements), _cmpEnt);
	for (int i = 0; i < list->size; ++i) {
		((Entity*) list->elements[i])->vt->render((Entity*) list->elements[i], screen);
	}
}


/*
 * Mob spawning (Java: Level.trySpawn): `count` attempts to place a
 * random mob kind via mob_findStartPos, which enforces distance from
 * the player and local density; successful finds are added here.
 */
void level_trySpawn(Level* level, int count){
	Random* random = &level->random;

	for (int i = 0; i < count; ++i) {
		Mob* mob = 0;

		int minLevel = 1;
		int maxLevel = 1;

		if (level->depth < 0) maxLevel = (-level->depth) + 1;
		if (level->depth > 0) minLevel = maxLevel = 4;

		int lvl = random_next_int(random, maxLevel - minLevel + 1) + minLevel;

		if (random_next_int(random, 2) == 0) {
			mob = malloc(sizeof(Slime));
			slime_create((Slime *) mob, lvl);
		} else {
			mob = malloc(sizeof(Zombie));
			zombie_create((Zombie *) mob, lvl);
		}

		if (!mob) continue; // ... I should check this later ...

		if (mob_findStartPos(mob, level)) {
			level_addEntity(level, &mob->entity);
		} else {
			mob->entity.vt->free(&mob->entity);
			free(mob);
		}
	}
}


/* Collects the entities overlapping the view rectangle and draws
 * them sorted, so sprites overlap correctly. */
void level_renderSprites(Level* level, Screen* screen, int xScroll, int yScroll) {
	ArrayList rowSprites;

	create_arraylist(&rowSprites);

	int xo = xScroll >> 4;
	int yo = yScroll >> 4;

	int w = (screen->w + 15) >> 4;
	int h = (screen->h + 15) >> 4;

	screen_set_offset(screen, xScroll, yScroll);

	for (int y = yo; y < h+yo; ++y) {
		for (int x = xo; x <= w+xo; ++x) {

			if (x < 0 || y < 0 || x >= level->w || y >= level->h) continue;
			ArrayList* ents = &level->entitiesInTiles[x + y * level->w];

            // TODO: use memcpy?
			for (int i = 0; i < ents->size; ++i) {
                arraylist_push(&rowSprites, ents->elements[i]);
            }
		}

		if (rowSprites.size > 0) {
			level_sortAndRender(level, screen, &rowSprites);
		}

		arraylist_clear(&rowSprites);
	}

	arraylist_remove(&rowSprites);
	screen_set_offset(screen, 0, 0);
}



/*
 * Lighting pass (the caves' fog-of-war): starts from a dark map,
 * adds each entity's light radius and the player's, then dithers
 * the result over the rendered screen.
 */
void renderLight(Level* level, Screen* screen, int xScroll, int yScroll) {
	int xo = xScroll >> 4;
	int yo = yScroll >> 4;
	int w = (screen->w + 15) >> 4;
	int h = (screen->h + 15) >> 4;

	screen_set_offset(screen, xScroll, yScroll);

	int r = 4;
	for (int y = yo - r; y <= h + yo + r; ++y) {
		for (int x = xo - r; x <= w + xo + r; ++x) {
			if (x < 0 || y < 0 || x >= level->w || y >= level->h) continue;

			ArrayList* entities = &level->entitiesInTiles[x + y * level->w];

			for (int i = 0; i < entities->size; ++i) {
				Entity* e = entities->elements[i];
				int lr = e->vt->getLightRadius(e);
				if(lr > 0) screen_render_light(screen, e->x - 1, e->y - 4, lr * 8);
			}

			TileID tile = level_get_tile(level, x, y);
			int lr = tile_getLightRadius(tile, level, x, y);
			if (lr > 0) screen_render_light(screen, x * 16 + 8, y * 16 + 8, lr * 8);
		}
	}

	screen_set_offset(screen, 0, 0);
}


/* Tile id accessor; indices are tile coordinates. */
extern inline unsigned char level_get_tile(Level* level, int x, int y){
	if(x < 0 || y < 0 || x >= level->w || y >= level->h) return ROCK;
	return level->tiles[x + y*level->w];
}


/* Replaces a tile and clears its data; neighbors are not notified
 * (callers handle follow-up effects). */
void level_set_tile(Level* level, int x, int y, int id, int data){
	if(x < 0 || y < 0 || x >= level->w || y >= level->h) return;
	level->tiles[x + y*level->w] = id;
	level->data[x + y*level->w] = data;
}


/* Data byte accessor (wheat growth, ore kind, ...). */
extern inline unsigned char level_get_data(Level* level, int x, int y){
	if(x < 0 || y < 0 || x >= level->w || y >= level->h) return 0;
	return level->data[x + y*level->w];
}


/* Sets a tile's data byte without touching its id. */
void level_set_data(Level* level, int x, int y, int val){
	if(x < 0 || y < 0 || x >= level->w || y >= level->h) return;
	level->data[x + y*level->w] = val;
}


/* Adds the entity to the bucket of the tile under its center. */
void level_insertEntity(Level* level, int x, int y, Entity* entity){
	if(x < 0 || y < 0 || x >= level->w || y >= level->h) return;
	arraylist_push(&level->entitiesInTiles[x+y*level->w], entity);
}


/*
 * Adds an entity to the level: initializes it, appends it to the
 * entity list and registers it in its tile bucket. The level takes
 * ownership of the allocation from here on.
 */
void level_addEntity(Level* level, Entity* entity){
	// TODO: if(e is player) level->player = e;

	entity->removed = 0;
	arraylist_push(&level->entities, entity);
	entity_init(entity, level);

	level_insertEntity(level, entity->x >> 4, entity->y >> 4, entity);
}


void level_removeEntity1(Level* level, Entity* e) {
	arraylist_removeElement(&level->entities, e);
	int xto = e->x >> 4;
	int yto = e->y >> 4;
	level_removeEntity(level, xto, yto, e);
}


/* Drops the entity from its tile bucket (list removal happens in
 * level_tick when the removed flag is seen). */
void level_removeEntity(Level* level, int x, int y, Entity* entity) {
	if (x < 0 || y < 0 || x >= level->w || y >= level->h) return;
	arraylist_removeElement(&level->entitiesInTiles[x+y*level->w], entity);
}


/* Appends every entity whose box intersects the pixel rectangle
 * (x0, y0)-(x1, y1) to `list`; used by movement and interactions. */
void level_getEntities(Level* level, ArrayList* list, int x0, int y0, int x1, int y1) {
	int xt0 = (x0 >> 4) - 1;
	int yt0 = (y0 >> 4) - 1;
	int xt1 = (x1 >> 4) + 1;
	int yt1 = (y1 >> 4) + 1;

	for (int y = yt0; y <= yt1; ++y) {
		for (int x = xt0; x <= xt1; ++x) {
			if (x < 0 || y < 0 || x >= level->w || y >= level->h) continue;

			ArrayList* entities = &level->entitiesInTiles[x + y *level->w];
			for (int i = 0; i < entities->size; ++i) {
				Entity* e = entities->elements[i];
				if (entity_intersects(e, x0, y0, x1, y1)) {
					arraylist_push(list, e);
				}
			}
		}
	}
}


/*
 * Per-tick level update (Java: Level.tick): ticks every entity,
 * compacts the list by dropping removed ones (unregistering them
 * from tile buckets and freeing them) and offers one mob spawn
 * attempt, matching the original's spawn pacing.
 */
void level_tick(Level* level) {
	level_trySpawn(level, 1);

	for (int i = 0; i < level->w*level->h / 50; ++i) {
		int xt = random_next_int(&level->random, level->w);
		int yt = random_next_int(&level->random, level->w);

		tile_tick(level_get_tile(level, xt, yt), level, xt, yt);
	}

	for (int i = 0; i < level->entities.size; ++i) {
		Entity* e = (Entity*) arraylist_get(&level->entities, i);
		int xto = e->x >> 4;
		int yto = e->y >> 4;

		e->vt->tick(e);

		if (e->removed) {
			arraylist_removeId(&level->entities, i--);
			level_removeEntity(level, xto, yto, e);

			if (e->type != PLAYER) {
				((Entity*) e)->vt->free((Entity*) e);
				free(e);
			}

		} else {
			int xt = e->x >> 4;
			int yt = e->y >> 4;

			if (xto != xt || yto != yt) {
				level_removeEntity(level, xto, yto, e);
				level_insertEntity(level, xt, yt, e);
			}
		}
	}
}


/* Releases every owned allocation: entity list, buckets, tiles and
 * data. Entities themselves are freed by the caller's teardown. */
void level_free(Level* lvl) {
	if (lvl->tiles) free(lvl->tiles);
	if (lvl->data) free(lvl->data);

	if (lvl->entities.elements) {
		arraylist_remove(&lvl->entities);
	}

	if (lvl->entitiesInTiles) {
		for (int i = 0; i < lvl->w*lvl->h; ++i) {
			ArrayList* list = lvl->entitiesInTiles + i;

			for (int i = 0; i < list->size; ++i) {
				Entity* e = list->elements[i];

				if (e->type != PLAYER) {
					((Entity*) e)->vt->free((Entity*) e);
					free(e);
				}
			}

			arraylist_remove(list);
		}

		free(lvl->entitiesInTiles);
	}
}
