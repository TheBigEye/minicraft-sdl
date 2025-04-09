#ifndef LEVEL_TILE_CLOUD_CACTUS_TILE_H_
#define LEVEL_TILE_CLOUD_CACTUS_TILE_H_

#include "tile.h"

struct _Item;
struct _Player;

void cloudcactustile_render(TileID id, Screen* screen, Level* level, int x, int y);
void cloudcactustile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
char cloudcactustile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_CLOUD_CACTUS_TILE_H_ */
