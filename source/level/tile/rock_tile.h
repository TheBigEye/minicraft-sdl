/*
 * rock_tile.h - Rock tile: breakable stone of the underground that
 *               drops stone and coal.
 */
#ifndef LEVEL_TILE_ROCK_TILE_H_
#define LEVEL_TILE_ROCK_TILE_H_

#include "tile.h"

void rocktile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Forwards mob damage into the chip-damage path. */
void rocktile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
/* Heals accumulated damage over time. */
void rocktile_tick(TileID id, Level* level, int xt, int yt);
/* Pickaxe interaction: chips the rock until it breaks. */
char rocktile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_WATER_TILE_H_ */
