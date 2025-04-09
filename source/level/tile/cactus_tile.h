#ifndef LEVEL_TILE_CACTUS_TILE_H_
#define LEVEL_TILE_CACTUS_TILE_H_

#include "tile.h"

void cactustile_init(TileID id);
void cactustile_render(TileID id, Screen* screen, Level* level, int x, int y);
void cactustile_tick(TileID id, Level* level, int xt, int yt);
void cactus_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

#endif /* LEVEL_TILE_CACTUS_TILE_H_ */
