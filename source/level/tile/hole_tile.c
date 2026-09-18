/*
 * hole_tile.c - Behaviour of the hole (Java: tile.HoleTile).
 *
 * It behaves like shallow water: crossable only by swimming. When drawn it
 * blends its edges with neighbouring sand and liquids.
 */
#include "tile.h"
#include "hole_tile.h"

#include "../../gfx/color.h"


/* Constructor: connects to sand and to both liquids. */
PUBLIC void holetile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render   = holetile_render;
    this->may_pass = holetile_may_pass;

    this->connects_to_sand  = true;
    this->connects_to_water = true;
    this->connects_to_lava  = true;
}


/* Crossable only by swimming. Java: HoleTile.mayPass() { return e.canSwim(); } */
PUBLIC boolean holetile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;

    return e->can_swim(e);
}


/* Draws the four quadrants, choosing a sand edge or a dirt edge. */
PUBLIC void holetile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    int col = get_color4(111, 111, 110, 110);
    int transitionColor1 = get_color4(3, 111, level->dirtColor - 111, level->dirtColor);
    int transitionColor2 = get_color4(3, 111, level->sandColor - 110, level->sandColor);

    Tile* up    = level->get_tile(level, x, y - 1);
    Tile* down  = level->get_tile(level, x, y + 1);
    Tile* left  = level->get_tile(level, x - 1, y);
    Tile* right = level->get_tile(level, x + 1, y);

    boolean u = !up->connects_to_liquid(up);
    boolean d = !down->connects_to_liquid(down);
    boolean l = !left->connects_to_liquid(left);
    boolean r = !right->connects_to_liquid(right);

    boolean su = u && up->connects_to_sand;
    boolean sd = d && down->connects_to_sand;
    boolean sl = l && left->connects_to_sand;
    boolean sr = r && right->connects_to_sand;

    if (!u && !l) screen->render(screen, x * 16 + 0, y * 16 + 0, 0, col, 0);
    else screen->render(screen, x * 16 + 0, y * 16 + 0, (l ? 14 : 15) + (u ? 0 : 1) * 32, (su || sl) ? transitionColor2 : transitionColor1, 0);

    if (!u && !r) screen->render(screen, x * 16 + 8, y * 16 + 0, 1, col, 0);
    else screen->render(screen, x * 16 + 8, y * 16 + 0, (r ? 16 : 15) + (u ? 0 : 1) * 32, (su || sr) ? transitionColor2 : transitionColor1, 0);

    if (!d && !l) screen->render(screen, x * 16 + 0, y * 16 + 8, 2, col, 0);
    else screen->render(screen, x * 16 + 0, y * 16 + 8, (l ? 14 : 15) + (d ? 2 : 1) * 32, (sd || sl) ? transitionColor2 : transitionColor1, 0);

    if (!d && !r) screen->render(screen, x * 16 + 8, y * 16 + 8, 3, col, 0);
    else screen->render(screen, x * 16 + 8, y * 16 + 8, (r ? 16 : 15) + (d ? 2 : 1) * 32, (sd || sr) ? transitionColor2 : transitionColor1, 0);
}
