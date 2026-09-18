/*
 * stairs_tile.c - Behaviour of the stairs (Java: tile.StairsTile).
 *
 * They do nothing on their own: the player triggers the level change by
 * stepping on them. They only draw, with the up or down sprite set.
 */
#include "tile.h"
#include "stairs_tile.h"

#include "../../gfx/color.h"


/* Constructor: records whether the stairs lead up or down. */
PUBLIC void stairstile_init(Tile* this, TileID id, boolean leadsUp) {
    tile_init(this, id);

    this->render = stairstile_render;

    this->add.stairs.leadsUp = leadsUp;
}


/* Draws the four quadrants, according to whether it goes up or down. */
PUBLIC void stairstile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    int color = get_color4(level->dirtColor, 000, 333, 444);
    int xt    = this->add.stairs.leadsUp ? 2 : 0;

    screen->render(screen, x * 16 + 0, y * 16 + 0, xt + 2 * 32, color, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 0, xt + 1 + 2 * 32, color, 0);
    screen->render(screen, x * 16 + 0, y * 16 + 8, xt + 3 * 32, color, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 8, xt + 1 + 3 * 32, color, 0);
}
