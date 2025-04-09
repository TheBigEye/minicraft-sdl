#ifndef LEVEL_TILE_CLOUD_TILE_H_
#define LEVEL_TILE_CLOUD_TILE_H_

#include "tile.h"

struct _Item;
struct _Player;

void cloudtile_render(TileID id, Screen* screen, Level* level, int x, int y);
char cloudtile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_CLOUD_TILE_H_ */
