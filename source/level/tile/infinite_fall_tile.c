/*
 * infinite_fall_tile.c - Behaviour of the bottomless gap
 *                        (Java: tile.InfiniteFallTile).
 *
 * It draws nothing, inheriting Tile's empty render(), and only the Air
 * Wizard can cross it: everyone else falls.
 */
#include "tile.h"
#include "infinite_fall_tile.h"


/* Constructor: a bottomless gap. */
PUBLIC void infinitefalltile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->may_pass = infinitefalltile_may_pass;
}


/* Only the Air Wizard can fly. Java: InfiniteFallTile.mayPass() */
PUBLIC boolean infinitefalltile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;

#ifdef GODMODE
    if (e->type == PLAYER) return true;
#endif

    return e->type == AIRWIZARD;
}
