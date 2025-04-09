#ifndef LEVEL_TILE_LAVA_TILE_H_
#define LEVEL_TILE_LAVA_TILE_H_

#include "tile.h"

void lavatile_init(TileID id);
void lavatile_render(TileID id, Screen* screen, Level* level, int x, int y);
void lavatile_tick(TileID id, Level* level, int xt, int yt);

#endif /* LEVEL_TILE_LAVA_TILE_H_ */
