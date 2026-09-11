/*
 * flower_tile.h - Flower tile: decorative grass variant that drops
 *                 flower items when cleared.
 */
#ifndef LEVEL_TILE_FLOWER_TILE_H_
#define LEVEL_TILE_FLOWER_TILE_H_

#include "tile.h"

struct _Item;
struct _Player;

/* Registers the flower connection flags. */
void flowertile_init(TileID id);
/* Clears the flowers into a flower item drop. */
void flowertile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir);
void flowertile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Shovel interaction: harvests the flowers. */
char flowertile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_FLOWER_TILE_H_ */
