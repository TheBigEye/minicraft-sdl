/*
 * ore_tile.h - Ore tile: mineral vein embedded in dirt; shares one
 *              implementation for iron, gold and gem ores.
 */
#ifndef LEVEL_TILE_ORE_TILE_H_
#define LEVEL_TILE_ORE_TILE_H_

#include "tileids.h"

#include "../level.h"
#include "../../gfx/screen.h"
#include "../../entity/mob.h"

struct _Resource;
struct _Player;
struct _Item;

/* Extra per-tile state for ores. */
typedef struct{
	struct _Resource* toDrop; /* Resource dropped when the vein breaks. */
	int color;                /* Cached sprite color for the ore vein. */
} add_ore_;

/* Registers the tile to drop the given resource. */
void oretile_init(TileID id, struct _Resource* toDrop);
void oretile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Retaliation path: no damage, feedback only. */
void oretile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
/* Pickaxe interaction: chips the vein and drops its resource. */
char oretile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_ORE_TILE_H_ */
