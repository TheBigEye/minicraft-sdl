/*
 * sapling_tile.h - Sapling tile: a planted acorn that grows into a
 *                  tree (or cactus) over time.
 */
#ifndef LEVEL_TILE_SAPLING_TILE_H_
#define LEVEL_TILE_SAPLING_TILE_H_

#include "tileids.h"
#include "../level.h"
#include "../../gfx/screen.h"

/* Extra per-tile state for saplings. */
typedef struct{
	TileID onType;   /* Ground tile the sapling sits on. */
	TileID growsTo;  /* Tile the sapling becomes when fully grown. */
} add_sapling_;

/* Configures what the sapling grows on and into. */
void saplingtile_init(TileID id, TileID onType, TileID growsTo);
void saplingtile_render(TileID id, Screen* screen, Level* level, int x, int y); 
/* Ages the sapling until it matures. */
void saplingtile_tick(TileID id, Level* level, int xt, int yt);
/* Any attack uproots the sapling back to its ground tile. */
void saplingtile_hurt(TileID id, Level* level, int x, int y, struct _Mob* source, int dmg, int attackDir);

#endif /* LEVEL_TILE_SAPLING_TILE_H_ */
