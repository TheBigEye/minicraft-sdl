#ifndef LEVEL_TILE_ORE_TILE_H_
#define LEVEL_TILE_ORE_TILE_H_

#include "tileids.h"

#include "../level.h"
#include "../../gfx/screen.h"
#include "../../entity/mob.h"

struct _Resource;
struct _Player;
struct _Item;

typedef struct{
	struct _Resource* toDrop;
	int color;
} add_ore_;

void oretile_init(TileID id, struct _Resource* toDrop);
void oretile_render(TileID id, Screen* screen, Level* level, int x, int y);
void oretile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
char oretile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_ORE_TILE_H_ */
