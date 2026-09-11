/*
 * lava_tile.h - Lava tile: deadly animated liquid of the deep levels.
 */
#ifndef LEVEL_TILE_LAVA_TILE_H_
#define LEVEL_TILE_LAVA_TILE_H_

#include "tile.h"

void lavatile_init(TileID id);
void lavatile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Spreads lava into adjacent holes over time. */
void lavatile_tick(TileID id, Level* level, int xt, int yt);

#endif /* LEVEL_TILE_LAVA_TILE_H_ */
