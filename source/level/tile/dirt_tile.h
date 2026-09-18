/*
 * dirt_tile.h - The dirt tile (Java: tile.DirtTile).
 */
#ifndef LEVEL_TILE_DIRT_TILE_H_
#define LEVEL_TILE_DIRT_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void dirttile_init(Tile* this, TileID id);

/* Java: DirtTile.render(Screen, Level, int, int) */
PUBLIC void dirttile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: DirtTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean dirttile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

#endif /* LEVEL_TILE_DIRT_TILE_H_ */
