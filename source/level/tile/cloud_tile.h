/*
 * cloud_tile.h - Cloud tile: walkable floor of the sky island, with
 *                soft edges against infinite-fall holes.
 */
#ifndef LEVEL_TILE_CLOUD_TILE_H_
#define LEVEL_TILE_CLOUD_TILE_H_

#include "tile.h"

struct _Item;
struct _Player;

void cloudtile_render(TileID id, Screen* screen, Level* level, int x, int y);
/* Shovel interaction: digs cloud resources out of the floor. */
char cloudtile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir);

#endif /* LEVEL_TILE_CLOUD_TILE_H_ */
