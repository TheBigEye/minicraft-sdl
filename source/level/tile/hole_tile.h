/*
 * hole_tile.h - Hole tile: dug-out ground left by the shovel.
 */
#ifndef LEVEL_TILE_HOLE_TILE_H_
#define LEVEL_TILE_HOLE_TILE_H_
#include "tile.h"

void holetile_init(TileID id);
void holetile_render(TileID id, Screen* screen, Level* level, int x, int y);

#endif /* LEVEL_TILE_HOLE_TILE_H_ */
