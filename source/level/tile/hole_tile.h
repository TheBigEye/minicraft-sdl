/*
 * hole_tile.h - The hole tile (Java: tile.HoleTile).
 *
 * Shallow water: it can only be crossed by swimming, and its edges blend
 * with sand and with liquids.
 */
#ifndef LEVEL_TILE_HOLE_TILE_H_
#define LEVEL_TILE_HOLE_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"

/* Constructor */
PUBLIC void holetile_init(Tile* this, TileID id);

/* Java: HoleTile.render(Screen, Level, int, int) */
PUBLIC void holetile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: HoleTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean holetile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

#endif /* LEVEL_TILE_HOLE_TILE_H_ */
