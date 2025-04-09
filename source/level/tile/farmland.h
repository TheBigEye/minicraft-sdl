#ifndef LEVEL_TILE_FARMLAND_H_
#define LEVEL_TILE_FARMLAND_H_

#include "tile.h"

struct _Item;
struct _Player;

void farmlandtile_render(TileID id, Screen* screen, Level* level, int x, int y);
void farmland_tick(TileID id, Level* level, int xt, int yt);
char farmtile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_FARMLAND_H_ */
