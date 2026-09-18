/*
 * water_tile.h - The water tile (Java: tile.WaterTile).
 *
 * An animated surface: crossable only by swimming, and it slowly spreads
 * into neighbouring holes.
 */
#ifndef LEVEL_TILE_WATER_TILE_H_
#define LEVEL_TILE_WATER_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"

/* Constructor */
PUBLIC void watertile_init(Tile* this, TileID id);

/* Java: WaterTile.render(Screen, Level, int, int) */
PUBLIC void watertile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: WaterTile.tick(Level, int, int) */
PUBLIC void watertile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: WaterTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean watertile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

#endif /* LEVEL_TILE_WATER_TILE_H_ */
