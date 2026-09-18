/*
 * ore_tile.h - An ore vein (Java: tile.OreTile).
 *
 * One implementation serves iron, gold and gem alike: the resource it
 * drops, and with it the colour of the vein, is configured on construction.
 *
 * Java declares hurt() twice, overloaded on its arguments:
 *
 *     hurt(Level, int, int, Mob source, int dmg, int attackDir)
 *     hurt(Level, int, int, int dmg)
 *
 * As with RockTile, the port needs one C name per signature: the slot
 * oretile_hurt() and the shared worker oretile_hurt_dmg().
 */
#ifndef LEVEL_TILE_ORE_TILE_H_
#define LEVEL_TILE_ORE_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"
#include "../../entity/mob.h"
#include "../../entity/player.h"
#include "../../item/item.h"
#include "../../item/resource/resource.h"

/* Constructor. Java: OreTile(int id, Resource toDrop) */
PUBLIC void oretile_init(Tile* this, TileID id, Resource* toDrop);

/* Java: OreTile.render(Screen, Level, int, int) */
PUBLIC void oretile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: OreTile.hurt(Level, int, int, Mob, int, int) */
PUBLIC void oretile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

/* Java: OreTile.bumpedInto(Level, int, int, Entity) */
PUBLIC void oretile_bumped_into(Tile* this, Level* level, int xt, int yt, Entity* entity);

/* Java: OreTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean oretile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

/* Java: OreTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean oretile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

/* One swing of the pickaxe: the shared body of both hurt() overloads. */
PUBLIC void oretile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg);

#endif /* LEVEL_TILE_ORE_TILE_H_ */
