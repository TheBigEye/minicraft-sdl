/*
 * cactus_tile.h - Cactus tile: stings nothing by itself but takes
 *                 damage until it breaks into cactus flowers.
 */
#ifndef LEVEL_TILE_CACTUS_TILE_H_
#define LEVEL_TILE_CACTUS_TILE_H_

#include "tile.h"

void cactustile_init(TileID id);
void cactustile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Slowly heals accumulated damage over time. */
void cactustile_tick(TileID id, Level* level, int xt, int yt);
/* Accumulates damage; at 10 breaks into sand dropping cactus flowers. */
void cactus_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

#endif /* LEVEL_TILE_CACTUS_TILE_H_ */
