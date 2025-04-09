#ifndef LEVEL_TILE_ROCK_TILE_H_
#define LEVEL_TILE_ROCK_TILE_H_

#include "tile.h"

void rocktile_render(TileID id, Screen* screen, Level* level, int x, int y);
void rocktile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
void rocktile_tick(TileID id, Level* level, int xt, int yt);
char rocktile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_WATER_TILE_H_ */
