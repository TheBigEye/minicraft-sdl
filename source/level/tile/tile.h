#ifndef LEVEL_TILE_TILE_H_
#define LEVEL_TILE_TILE_H_

#include "../../utils/javarandom.h"
#include "stairs_tile.h"
#include "sapling_tile.h"
#include "ore_tile.h"
#include "tileids.h"
#include "../level.h"
#include "../../gfx/screen.h"
#include "../../entity/entity.h"
#include "../../entity/mob.h"

typedef struct{
	Random random;

	TileID id;

	union{
		add_stairs_ stairs;
		add_sapling_ sapling;
		add_ore_ ore;
	} add;

	char connectsToGrass : 1;
	char connectsToSand : 1;
	char connectsToLava : 1;
	char connectsToWater : 1;
} Tile;
struct _Player;
struct _Item;

extern Tile tiles[256];
extern int tile_tickCount;
void init_tiles();
void tile_init(TileID id);
void tile_render(TileID id, Screen* screen, Level* level, int x, int y);
char tile_mayPass(TileID id, Level* level, int x, int y, Entity* e);
void tile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
char tile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);
char tile_use(TileID id, Level* level, int xt, int yt, struct _Player* player, int attackDir);
void tile_steppedOn(TileID id, Level* level, int x, int y, struct _Entity* entity);
int tile_getLightRadius(TileID id, Level* level, int x, int y);
void tile_bumpedInto(TileID id, Level* level, int x, int y, Entity* entity);

void tile_tick(TileID id, Level* level, int xt, int yt);
char tile_connectsToLiquid(TileID id);

#endif /* LEVEL_TILE_TILE_H_ */
