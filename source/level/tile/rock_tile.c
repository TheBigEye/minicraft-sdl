/*
 * rock_tile.c - Behaviour of rock (Java: tile.RockTile).
 *
 * Damage accumulates in the square's data byte: at 50 the rock breaks,
 * becomes dirt and drops stone, and sometimes coal; if it does not get
 * there, the damage heals one point per tick.
 */
#include "tile.h"
#include "rock_tile.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../gfx/color.h"
#include "../../item/resource/resource.h"
#include "../../item/resourceitem.h"


/* Constructor: solid rock broken with the pickaxe. Java: RockTile(int id) */
PUBLIC void rocktile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render   = rocktile_render;
    this->may_pass = rocktile_may_pass;
    this->hurt     = rocktile_hurt;
    this->interact = rocktile_interact;
    this->tick     = rocktile_tick;
}


/*
 * Accumulated damage: shows the hit (particles and the number) and, at 50,
 * breaks the rock, dropping stone and coal.
 * Java: RockTile.hurt(Level, int, int, int dmg)
 */
PUBLIC void rocktile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg) {
    int damage = level->get_data(level, x, y) + dmg;

    SmashParticle* smash = new(SmashParticle);

    smashparticle_create(smash, x * 16 + 8, y * 16 + 8);
    level->add(level, &smash->entity);

    TextParticle* text = new(TextParticle);
    String txt = new_array(char, 16);

    sprintf(txt, "%d", dmg);
    textparticle_create(text, txt, x * 16 + 8, y * 16 + 8, get_color4(-1, 500, 500, 500));
    level->add(level, &text->entity);

    Random* random = &this->random;

    if (damage >= 50) {
        int count = random->next_int(random, 4) + 1;

        for (int i = 0; i < count; ++i) {
            Item resource;
            ItemEntity* item_entity = new(ItemEntity);

            resourceitem_create(&resource, &stone);

            int xx = x * 16 + random->next_int(random, 10) + 3;
            int yy = y * 16 + random->next_int(random, 10) + 3;

            itementity_create(item_entity, resource, xx, yy);
            level->add(level, &item_entity->entity);
        }

        count = random->next_int(random, 2);

        for (int i = 0; i < count; ++i) {
            Item resource;
            ItemEntity* item_entity = new(ItemEntity);

            resourceitem_create(&resource, &coal);

            int xx = x * 16 + random->next_int(random, 10) + 3;
            int yy = y * 16 + random->next_int(random, 10) + 3;

            itementity_create(item_entity, resource, xx, yy);
            level->add(level, &item_entity->entity);
        }

        level->set_tile(level, x, y, tiles[DIRT], 0);

    } else {
        level->set_data(level, x, y, damage);
    }
}


/* The pickaxe chips off damage proportional to its level. */
PUBLIC boolean rocktile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == PICKAXE) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                int dmg = this->random.next_int(&this->random, 10) + item->add.tool.level * 5 + 10;

                rocktile_hurt_dmg(this, level, xt, yt, dmg);
                return true;
            }
        }
    }

    return false;
}


/* A mob's damage comes in through the same path as the pickaxe's. */
PUBLIC void rocktile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
    (void) source;
    (void) attackDir;

    rocktile_hurt_dmg(this, level, x, y, dmg);
}


/* Rock is solid. Java: RockTile.mayPass() { return false; } */
PUBLIC boolean rocktile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;

#ifdef GODMODE
    if (e->type == PLAYER) return true;
#else
    (void) e;
#endif

    return false;
}


/* Draws the four quadrants of rock, with light edges against other tiles. */
PUBLIC void rocktile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    int col = get_color4(444, 444, 333, 333);
    int transitionColor = get_color4(111, 444, 555, level->dirtColor);

    boolean u = level->get_tile(level, x, y - 1) != this;
    boolean d = level->get_tile(level, x, y + 1) != this;
    boolean l = level->get_tile(level, x - 1, y) != this;
    boolean r = level->get_tile(level, x + 1, y) != this;

    boolean ul = level->get_tile(level, x - 1, y - 1) != this;
    boolean dl = level->get_tile(level, x - 1, y + 1) != this;
    boolean ur = level->get_tile(level, x + 1, y - 1) != this;
    boolean dr = level->get_tile(level, x + 1, y + 1) != this;

    if (!u && !l) {
        if (!ul) {
            screen->render(screen, x * 16 + 0, y * 16 + 0, 0, col, 0);
        } else {
            screen->render(screen, x * 16 + 0, y * 16 + 0, 7 + 0 * 32, transitionColor, 3);
        }
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 0, (l ? 6 : 5) + (u ? 2 : 1) * 32, transitionColor, 3);
    }

    if (!u && !r) {
        if (!ur) {
            screen->render(screen, x * 16 + 8, y * 16 + 0, 1, col, 0);
        } else {
            screen->render(screen, x * 16 + 8, y * 16 + 0, 8 + 0 * 32, transitionColor, 3);
        }
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 0, (r ? 4 : 5) + (u ? 2 : 1) * 32, transitionColor, 3);
    }

    if (!d && !l) {
        if (!dl) {
            screen->render(screen, x * 16 + 0, y * 16 + 8, 2, col, 0);
        } else {
            screen->render(screen, x * 16 + 0, y * 16 + 8, 7 + 1 * 32, transitionColor, 3);
        }
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 8, (l ? 6 : 5) + (d ? 0 : 1) * 32, transitionColor, 3);
    }

    if (!d && !r) {
        if (!dr) {
            screen->render(screen, x * 16 + 8, y * 16 + 8, 3, col, 0);
        } else {
            screen->render(screen, x * 16 + 8, y * 16 + 8, 8 + 1 * 32, transitionColor, 3);
        }
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 8, (r ? 4 : 5) + (d ? 0 : 1) * 32, transitionColor, 3);
    }
}


/*
 * Heals one point of accumulated damage per tick, if any is left.
 * Java: `if (damage > 0)`; testing only for non-zero would keep pushing a
 * negative data byte further down instead of leaving it alone.
 */
PUBLIC void rocktile_tick(Tile* this, Level* level, int xt, int yt) {
    (void) this;

    int damage = level->get_data(level, xt, yt);

    if (damage > 0) {
        level->set_data(level, xt, yt, damage - 1);
    }
}
