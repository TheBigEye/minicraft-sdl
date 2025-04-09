#ifndef LEVEL_TILE_WHEAT_TILE_H_
#define LEVEL_TILE_WHEAT_TILE_H_

#include "tile.h"

void wheat_tile_init(TileID id);
void wheattile_render(TileID id, Screen* screen, Level* level, int x, int y);
void wheattile_tick(TileID id, Level* level, int xt, int yt);
void wheattile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
void wheattile_harvest(TileID id, Level* level, int x, int y);
char wheattile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_WATER_TILE_H_ */
