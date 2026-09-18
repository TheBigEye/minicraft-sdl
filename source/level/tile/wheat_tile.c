/*
 * wheat_tile.c - Behaviour of wheat (Java: tile.WheatTile).
 *
 * The data byte is the age, which climbs up to 50. Once ripe it is
 * harvested by hitting or stepping on it: it drops seeds and, from age 40
 * onwards, wheat as well, topping out at 50. The shovel just pulls it up.
 */
#include "tile.h"
#include "wheat_tile.h"

#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../entity/player.h"
#include "../../gfx/color.h"
#include "../../item/item.h"
#include "../../item/resourceitem.h"


/* Constructor: a crop that is harvested once ripe. */
PUBLIC void wheat_tile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render     = wheattile_render;
    this->tick       = wheattile_tick;
    this->hurt       = wheattile_hurt;
    this->stepped_on = wheattile_stepped_on;
    this->interact   = wheattile_interact;
}


/* The shovel pulls the crop up and leaves plain dirt. */
PUBLIC boolean wheattile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) this;
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == SHOVEL) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                level->set_tile(level, xt, yt, tiles[DIRT], 0);
                return true;
            }
        }
    }

    return false;
}


/*
 * Harvest: drops up to two seeds and, if it is ripe, wheat too, more of it
 * the riper it is. Afterwards the square becomes dirt again.
 */
PUBLIC void wheattile_harvest(Tile* this, Level* level, int x, int y) {
    int age = level->get_data(level, x, y);
    Random* random = &this->random;

    int count = random->next_int(random, 2);

    for (int i = 0; i < count; ++i) {
        ItemEntity* ent = new(ItemEntity);
        Item res;

        resourceitem_create(&res, &seeds);

        int xx = x * 16 + random->next_int(random, 10) + 3;
        int yy = y * 16 + random->next_int(random, 10) + 3;

        itementity_create(ent, res, xx, yy);
        level->add(level, (Entity*) ent);
    }

    count = 0;

    if (age == 50) {
        count = random->next_int(random, 3) + 2;
    } else if (age >= 40) {
        count = random->next_int(random, 2) + 1;
    }

    for (int i = 0; i < count; ++i) {
        ItemEntity* ent = new(ItemEntity);
        Item res;

        resourceitem_create(&res, &wheat);

        int xx = x * 16 + random->next_int(random, 10) + 3;
        int yy = y * 16 + random->next_int(random, 10) + 3;

        itementity_create(ent, res, xx, yy);
        level->add(level, (Entity*) ent);
    }

    level->set_tile(level, x, y, tiles[DIRT], 0);
}


/* A hit harvests the wheat. */
PUBLIC void wheattile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
    (void) source;
    (void) dmg;
    (void) attackDir;

    wheattile_harvest(this, level, x, y);
}


/* Stepping on ripe wheat harvests it, 1 in 60 per step. */
PUBLIC void wheattile_stepped_on(Tile* this, Level* level, int xt, int yt, Entity* entity) {
    (void) entity;

    if (this->random.next_int(&this->random, 60) != 0) return;
    if (level->get_data(level, xt, yt) < 2) return;

    wheattile_harvest(this, level, xt, yt);
}


/* Draws the growth stage; golden once it is nearly ready, at age 50. */
PUBLIC void wheattile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    int age = level->get_data(level, x, y);
    int col = get_color4(level->dirtColor - 121, level->dirtColor - 11, level->dirtColor, 50);
    int icon = age / 10;

    if (icon >= 3) {
        col = get_color4(level->dirtColor - 121, level->dirtColor - 11, 50 + icon * 100, 40 + (icon - 3) * 2 * 100);

        if (age == 50) col = get_color4(0, 0, 50 + icon * 100, 40 + (icon - 3) * 2 * 100);

        icon = 3;
    }

    screen->render(screen, x * 16 + 0, y * 16 + 0, 4 + 3 * 32 + icon, col, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 0, 4 + 3 * 32 + icon, col, 0);
    screen->render(screen, x * 16 + 0, y * 16 + 8, 4 + 3 * 32 + icon, col, 1);
    screen->render(screen, x * 16 + 8, y * 16 + 8, 4 + 3 * 32 + icon, col, 1);
}


/* Grows on half of the ticks, up to age 50. */
PUBLIC void wheattile_tick(Tile* this, Level* level, int xt, int yt) {
    if (this->random.next_int(&this->random, 2) == 0) return;

    int age = level->get_data(level, xt, yt);

    if (age < 50) level->set_data(level, xt, yt, age + 1);
}
