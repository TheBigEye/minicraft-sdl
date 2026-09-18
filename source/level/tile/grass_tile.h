/*
 * grass_tile.h - The grass tile (Java: tile.GrassTile).
 *
 * The shovel turns it into dirt and the hoe into farmland, with a 1 in 5
 * chance of dropping seeds. Now and then it spreads onto a neighbouring
 * dirt square.
 */
#ifndef LEVEL_TILE_GRASS_TILE_H_
#define LEVEL_TILE_GRASS_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void grasstile_init(Tile* this, TileID id);

/* Java: GrassTile.render(Screen, Level, int, int) */
PUBLIC void grasstile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: GrassTile.tick(Level, int, int) */
PUBLIC void grasstile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: GrassTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean grasstile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

#endif /* LEVEL_TILE_GRASS_TILE_H_ */
