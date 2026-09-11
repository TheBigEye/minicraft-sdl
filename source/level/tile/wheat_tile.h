/*
 * wheat_tile.h - Wheat tile: crop planted on farmland; its data byte
 *                is the growth age (max 50).
 */
#ifndef LEVEL_TILE_WHEAT_TILE_H_
#define LEVEL_TILE_WHEAT_TILE_H_

#include "tile.h"

void wheat_tile_init(TileID id);
void wheattile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Grows the crop one age step per tick (half the ticks). */
void wheattile_tick(TileID id, Level* level, int xt, int yt);
/* Any attack harvests the crop. */
void wheattile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
/* Collects seeds and wheat according to the crop's age. */
void wheattile_harvest(TileID id, Level* level, int x, int y);
/* Shovel clears the crop (and the farmland) back to dirt. */
char wheattile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_WATER_TILE_H_ */
