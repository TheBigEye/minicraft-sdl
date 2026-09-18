/*
 * infinite_fall_tile.h - The bottomless gap of the sky island
 *                        (Java: tile.InfiniteFallTile).
 *
 * It draws nothing and only the Air Wizard can cross it.
 */
#ifndef LEVEL_TILE_INFINITE_FALL_TILE_H_
#define LEVEL_TILE_INFINITE_FALL_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"

/* Constructor */
PUBLIC void infinitefalltile_init(Tile* this, TileID id);

/* Java: InfiniteFallTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean infinitefalltile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

#endif /* LEVEL_TILE_INFINITE_FALL_TILE_H_ */
