/*
 * sand_tile.h - The sand tile (Java: tile.SandTile).
 *
 * The shovel turns it into dirt and drops sand; it records footprints in
 * the data byte when a mob steps on it.
 */
#ifndef LEVEL_TILE_SAND_TILE_H_
#define LEVEL_TILE_SAND_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void sandtile_init(Tile* this, TileID id);

/* Java: SandTile.render(Screen, Level, int, int) */
PUBLIC void sandtile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: SandTile.tick(Level, int, int) */
PUBLIC void sandtile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: SandTile.steppedOn(Level, int, int, Entity) */
PUBLIC void sandtile_stepped_on(Tile* this, Level* level, int xt, int yt, Entity* entity);

/* Java: SandTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean sandtile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

#endif /* LEVEL_TILE_SAND_TILE_H_ */
