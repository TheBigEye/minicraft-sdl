#ifndef LEVEL_TILE_GRASS_TILE_H_
#define LEVEL_TILE_GRASS_TILE_H_

#include "tile.h"

struct _Item;
struct _Player;

void grasstile_init(char id);
void grasstile_tick(TileID id, Level* level, int xt, int yt);
void grasstile_render(TileID id, Screen* screen, Level* level, int x, int y);
char grasstile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_GRASS_TILE_H_ */
