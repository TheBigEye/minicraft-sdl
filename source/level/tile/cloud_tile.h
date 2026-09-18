/*
 * cloud_tile.h - The cloud tile (Java: tile.CloudTile).
 *
 * The floor of the sky island: it can be dug with the shovel to open an
 * infinite fall, and digging drops cloud.
 */
#ifndef LEVEL_TILE_CLOUD_TILE_H_
#define LEVEL_TILE_CLOUD_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void cloudtile_init(Tile* this, TileID id);

/* Java: CloudTile.render(Screen, Level, int, int) */
PUBLIC void cloudtile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: CloudTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean cloudtile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

#endif /* LEVEL_TILE_CLOUD_TILE_H_ */
