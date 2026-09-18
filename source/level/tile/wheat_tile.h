/*
 * wheat_tile.h - The wheat tile (Java: tile.WheatTile).
 *
 * It grows on farmland up to age 50; once ripe it is harvested by hitting
 * or stepping on it, and drops seeds plus wheat according to its age.
 */
#ifndef LEVEL_TILE_WHEAT_TILE_H_
#define LEVEL_TILE_WHEAT_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"
#include "../../entity/mob.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void wheat_tile_init(Tile* this, TileID id);

/* Java: WheatTile.render(Screen, Level, int, int) */
PUBLIC void wheattile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: WheatTile.tick(Level, int, int) */
PUBLIC void wheattile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: WheatTile.hurt(Level, int, int, Mob, int, int) */
PUBLIC void wheattile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

/* Drops the harvest and turns the square back into dirt. Java: harvest() */
PUBLIC void wheattile_harvest(Tile* this, Level* level, int x, int y);

/* Java: WheatTile.steppedOn(Level, int, int, Entity) */
PUBLIC void wheattile_stepped_on(Tile* this, Level* level, int xt, int yt, Entity* entity);

/* Java: WheatTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean wheattile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

#endif /* LEVEL_TILE_WHEAT_TILE_H_ */
