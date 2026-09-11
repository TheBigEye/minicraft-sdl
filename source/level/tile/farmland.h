/*
 * farmland.h - Farmland tile: tilled soil where wheat grows; its data
 *              byte tracks hydration/age.
 */
#ifndef LEVEL_TILE_FARMLAND_H_
#define LEVEL_TILE_FARMLAND_H_

#include "tile.h"

struct _Item;
struct _Player;

void farmlandtile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Ages the soil one step per tick, up to its maximum. */
void farmland_tick(TileID id, Level* level, int xt, int yt);
/* Shovel reverts farmland to plain dirt. */
char farmtile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_FARMLAND_H_ */
