/*
 * hard_rock_tile.c - Behaviour of hard rock (Java: tile.HardRockTile).
 *
 * It takes 200 points of accumulated damage, which only the gem pickaxe
 * manages, and heals over time. When it breaks it leaves dirt, stone and
 * coal behind.
 */
#include "tile.h"
#include "hard_rock_tile.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../gfx/color.h"
#include "../../item/item.h"
#include "../../item/resourceitem.h"


/* Constructor: very hard rock, only for the gem pickaxe. */
PUBLIC void hardrocktile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render   = hardrocktile_render;
    this->may_pass = hardrocktile_may_pass;
    this->hurt     = hardrocktile_hurt;
    this->interact = hardrocktile_interact;
    this->tick     = hardrocktile_tick;
}


/* It is solid. Java: HardRockTile.mayPass() { return false; } */
PUBLIC boolean hardrocktile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
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


/*
 * Heals one point of accumulated damage per tick, if any is left.
 * Java: `if (damage > 0)`; testing only for non-zero would keep pushing a
 * negative data byte further down instead of leaving it alone.
 */
PUBLIC void hardrocktile_tick(Tile* this, Level* level, int xt, int yt) {
    (void) this;

    int damage = level->get_data(level, xt, yt);

    if (damage > 0) {
        level->set_data(level, xt, yt, damage - 1);
    }
}


/* Damage adds up: at 200 points it breaks, dropping stone and coal.
 * Java: HardRockTile.hurt(Level, int, int, int dmg) */
PUBLIC void hardrocktile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg) {
    int damage = level->get_data(level, x, y) + dmg;

    SmashParticle* smash_particle = new(SmashParticle);

    smashparticle_create(smash_particle, x * 16 + 8, y * 16 + 8);
    level->add(level, &smash_particle->entity);

    TextParticle* text_particle = new(TextParticle);
    String text = new_array(char, 16);

    sprintf(text, "%d", dmg);
    textparticle_create(text_particle, text, x * 16 + 8, y * 16 + 8, get_color4(-1, 500, 500, 500));
    level->add(level, &text_particle->entity);

    if (damage >= 200) {
        Random* rand = &this->random;

        int count = rand->next_int(rand, 4) + 1;

        for (int i = 0; i < count; ++i) {
            ItemEntity* item_entity = new(ItemEntity);

            if (!item_entity) continue;

            Item resource;

            resourceitem_create(&resource, &stone);

            int xx = x * 16 + rand->next_int(rand, 10) + 3;
            int yy = y * 16 + rand->next_int(rand, 10) + 3;

            itementity_create(item_entity, resource, xx, yy);
            level->add(level, &item_entity->entity);
        }

        count = rand->next_int(rand, 2);

        for (int i = 0; i < count; ++i) {
            ItemEntity* item_entity = new(ItemEntity);

            if (!item_entity) continue;

            Item resource;

            resourceitem_create(&resource, &coal);

            int xx = x * 16 + rand->next_int(rand, 10) + 3;
            int yy = y * 16 + rand->next_int(rand, 10) + 3;

            itementity_create(item_entity, resource, xx, yy);
            level->add(level, &item_entity->entity);
        }

        level->set_tile(level, x, y, tiles[DIRT], 0);

    } else {
        level->set_data(level, x, y, damage);
    }
}


/* Only the gem pickaxe, level 4, can mine it. */
PUBLIC boolean hardrocktile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == PICKAXE && item->add.tool.level == 4) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                int dmg = this->random.next_int(&this->random, 10) + item->add.tool.level * 5 + 10;

                hardrocktile_hurt_dmg(this, level, xt, yt, dmg);
                return true;
            }
        }
    }

    return false;
}


/* Generic hits do not damage it: they only show the impact. */
PUBLIC void hardrocktile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
    (void) source;
    (void) dmg;
    (void) attackDir;

    hardrocktile_hurt_dmg(this, level, x, y, 0);
}


/* Draws the four quadrants, with dark edges against other tiles. */
PUBLIC void hardrocktile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    int col = get_color4(334, 334, 223, 223);
    int transitionColor = get_color4(001, 334, 445, level->dirtColor);

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
