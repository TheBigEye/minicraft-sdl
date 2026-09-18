/*
 * farmland.h - The farmland tile (Java: tile.FarmTile).
 *
 * The shovel returns it to plain dirt; wheat grows on top of it. Stepping
 * on it a lot degrades it.
 */
#ifndef LEVEL_TILE_FARMLAND_H_
#define LEVEL_TILE_FARMLAND_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void farmlandtile_init(Tile* this, TileID id);

/* Java: FarmTile.render(Screen, Level, int, int) */
PUBLIC void farmlandtile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: FarmTile.tick(Level, int, int) */
PUBLIC void farmlandtile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: FarmTile.steppedOn(Level, int, int, Entity) */
PUBLIC void farmlandtile_stepped_on(Tile* this, Level* level, int xt, int yt, Entity* entity);

/* Java: FarmTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean farmlandtile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

#endif /* LEVEL_TILE_FARMLAND_H_ */
