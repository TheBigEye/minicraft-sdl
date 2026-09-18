/*
 * sapling_tile.c - Behaviour of the sapling (Java: tile.SaplingTile).
 *
 * It ages in the data byte and, once it passes 100, turns into the tile it
 * grows to, a tree or a cactus. Any hit uproots it and leaves bare ground.
 */
#include "tile.h"
#include "sapling_tile.h"

#include "../../gfx/color.h"


/* Constructor: stores the ground and the tile it grows into, copying flags. */
PUBLIC void saplingtile_init(Tile* this, TileID id, TileID onType, TileID growsTo) {
    tile_init(this, id);

    this->render = saplingtile_render;
    this->tick   = saplingtile_tick;
    this->hurt   = saplingtile_hurt;

    this->add.sapling.onType  = onType;
    this->add.sapling.growsTo = growsTo;

    Tile* on = tiles[onType];

    this->connects_to_sand  = on->connects_to_sand;
    this->connects_to_grass = on->connects_to_grass;
    this->connects_to_water = on->connects_to_water;
    this->connects_to_lava  = on->connects_to_lava;
}


/* The ground underneath, with the sapling on top of it. */
PUBLIC void saplingtile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    Tile* on = tiles[this->add.sapling.onType];

    on->render(on, screen, level, x, y);

    int col = get_color4(10, 40, 50, -1);

    screen->render(screen, x * 16 + 4, y * 16 + 4, 11 + 3 * 32, col, 0);
}


/* Ages one step per tick; once past 100 it grows. */
PUBLIC void saplingtile_tick(Tile* this, Level* level, int xt, int yt) {
    int age = level->get_data(level, xt, yt) + 1;

    if (age > 100) {
        level->set_tile(level, xt, yt, tiles[this->add.sapling.growsTo], 0);
    } else {
        level->set_data(level, xt, yt, age);
    }
}


/* Any hit uproots it: it becomes the bare ground again. */
PUBLIC void saplingtile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
    (void) source;
    (void) dmg;
    (void) attackDir;

    level->set_tile(level, x, y, tiles[this->add.sapling.onType], 0);
}
