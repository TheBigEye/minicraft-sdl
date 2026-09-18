/*
 * flower_tile.h - The flower tile (Java: tile.FlowerTile).
 *
 * Drawn on top of grass and picked with any tool, dropping flowers. The
 * shape of the flower comes out of the data byte.
 */
#ifndef LEVEL_TILE_FLOWER_TILE_H_
#define LEVEL_TILE_FLOWER_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/mob.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void flowertile_init(Tile* this, TileID id);

/* Java: FlowerTile.render(Screen, Level, int, int) */
PUBLIC void flowertile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: FlowerTile.hurt(Level, int, int, Mob, int, int) */
PUBLIC void flowertile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

/* Java: FlowerTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean flowertile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

#endif /* LEVEL_TILE_FLOWER_TILE_H_ */
