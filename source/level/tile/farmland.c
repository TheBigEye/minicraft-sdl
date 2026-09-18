/*
 * farmland.c - Behaviour of farmland (Java: tile.FarmTile).
 *
 * The data byte holds the furrow's "age", up to 5. Walking on it often
 * turns it back into plain dirt; the shovel does it at once.
 */
#include "tile.h"
#include "farmland.h"

#include "../../entity/player.h"
#include "../../gfx/color.h"
#include "../../item/item.h"


/* Constructor: tilled with the hoe, undone with the shovel. */
PUBLIC void farmlandtile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render     = farmlandtile_render;
    this->tick       = farmlandtile_tick;
    this->stepped_on = farmlandtile_stepped_on;
    this->interact   = farmlandtile_interact;
}


/* Draws the ploughed furrow: two mirrored quadrants. */
PUBLIC void farmlandtile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    int col = get_color4(level->dirtColor - 121, level->dirtColor - 11, level->dirtColor, level->dirtColor + 111);

    screen->render(screen, x * 16 + 0, y * 16 + 0, 2 + 32, col, 1);
    screen->render(screen, x * 16 + 8, y * 16 + 0, 2 + 32, col, 0);
    screen->render(screen, x * 16 + 0, y * 16 + 8, 2 + 32, col, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 8, 2 + 32, col, 1);
}


/* The shovel returns it to plain dirt. */
PUBLIC boolean farmlandtile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
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


/* The furrow matures up to 5. */
PUBLIC void farmlandtile_tick(Tile* this, Level* level, int xt, int yt) {
    (void) this;

    int age = level->get_data(level, xt, yt);

    if (age < 5) level->set_data(level, xt, yt, age + 1);
}


/* Stepping on it degrades it: 1 in 60 of becoming plain dirt again. */
PUBLIC void farmlandtile_stepped_on(Tile* this, Level* level, int xt, int yt, Entity* entity) {
    (void) entity;

    if (this->random.next_int(&this->random, 60) != 0) return;
    if (level->get_data(level, xt, yt) < 5) return;

    level->set_tile(level, xt, yt, tiles[DIRT], 0);
}
