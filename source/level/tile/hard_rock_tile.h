/*
 * hard_rock_tile.h - Hard rock tile: deep-underground boulder that
 *                    only the gem (level 4) pickaxe can break.
 */
#ifndef LEVEL_TILE_HARD_ROCK_TILE_H_
#define LEVEL_TILE_HARD_ROCK_TILE_H_

#include "tile.h"

struct _Item;
struct _Player;

void hardrocktile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Heals accumulated damage over time. */
void hardrocktile_tick(TileID id, Level* level, int xt, int yt);
/* Retaliation path: no damage, feedback only. */
void hardrocktile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
/* Gem pickaxe interaction: chips the rock until it breaks. */
char hardrocktile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_HARD_ROCK_TILE_H_ */
