/*
 * tree_tile.h - Tree tile: overworld foliage chopped for wood, which
 *               may drop apples and acorns.
 */
#ifndef LEVEL_TILE_TREE_TILE_H_
#define LEVEL_TILE_TREE_TILE_H_

#include "tile.h"

void treetile_init(TileID id);
/* Heals accumulated damage over time. */
void treetile_tick(TileID id, Level* level, int xt, int yt);
/* Forwards mob damage into the chop-damage path. */
void treetile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
void treetile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Axe interaction: chops the tree until it falls. */
char treetile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_TREE_TILE_H_ */
