#ifndef LEVEL_TILE_WATER_TILE_H_
#define LEVEL_TILE_WATER_TILE_H_

#include "tile.h"
void watertile_init(TileID id);
void watertile_render(TileID id, Screen* screen, Level* level, int x, int y);
void watertile_tick(TileID id, Level* level, int xt, int yt);

#endif /* LEVEL_TILE_WATER_TILE_H_ */
