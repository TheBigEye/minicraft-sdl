/*
 * tile.h - Tile system: the 16x16 building blocks of every level
 *          (Java: com.mojang.ld22.level.tile.Tile).
 *
 * Each tile id has a global entry in tiles[] with its behavior hooks
 * (render, tick, interact, hurt, ...) registered by init_tiles(), plus
 * connection flags used for edge-transition rendering.
 */
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
	Random random;   /* per-tile RNG for tick randomness */

	TileID id;       /* own id, for hooks that receive only the Tile */

	/* Per-type configuration set by each tile's init: */
	union{
		add_stairs_ stairs;
		add_sapling_ sapling;
		add_ore_ ore;
	} add;

	char connectsToGrass : 1;  /* blend edges with grass neighbors */
	char connectsToSand : 1;   /* blend edges with sand neighbors */
	char connectsToLava : 1;   /* liquid edge blending (lava) */
	char connectsToWater : 1;  /* liquid edge blending (water) */
} Tile;
struct _Player;
struct _Item;

/* Global tile table, indexed by TileID. */
extern Tile tiles[256];
/* Global tick counter gating the occasional tile ticks. */
extern int tile_tickCount;
/* Registers every tile type once at startup. */
void init_tiles();
/* Resets one tiles[] entry to defaults. */
void tile_init(TileID id);
/* Draws the tile at tile coords (x, y). */
void tile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Whether entity `e` may walk/swim into the tile. */
char tile_mayPass(TileID id, Level* level, int x, int y, Entity* e);
/* Attack/mining damage against the tile. */
void tile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
/* Item-on-tile interaction (shovel, hoe, ...); returns success. */
char tile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);
/* Use-key interaction with the tile. */
char tile_use(TileID id, Level* level, int xt, int yt, struct _Player* player, int attackDir);
/* Notification that an entity stands on the tile. */
void tile_steppedOn(TileID id, Level* level, int x, int y, struct _Entity* entity);
/* Light emitted by the tile (lava glows). */
int tile_getLightRadius(TileID id, Level* level, int x, int y);
/* Notification that an entity bumped into the tile. */
void tile_bumpedInto(TileID id, Level* level, int x, int y, Entity* entity);

/* Occasional per-tile update (growth, spread). */
void tile_tick(TileID id, Level* level, int xt, int yt);
/* True for water/lava tiles (liquid edge rendering). */
char tile_connectsToLiquid(TileID id);

#endif /* LEVEL_TILE_TILE_H_ */
