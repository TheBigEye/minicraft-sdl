#ifndef LEVEL_TILE_FLOWER_TILE_H_
#define LEVEL_TILE_FLOWER_TILE_H_

#include "tile.h"

struct _Item;
struct _Player;

void flowertile_init(TileID id);
void flowertile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
void flowertile_render(TileID id, Screen* screen, Level* level, int x, int y);
char flowertile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_FLOWER_TILE_H_ */
