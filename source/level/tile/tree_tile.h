/*
 * tree_tile.h - The tree tile (Java: tile.TreeTile).
 *
 * It takes 20 points of accumulated damage before falling: it drops wood,
 * acorns and sometimes an apple. The axe fells it.
 *
 * Java declares hurt() twice, overloaded on its arguments:
 *
 *     hurt(Level, int, int, Mob source, int dmg, int attackDir)
 *     hurt(Level, int, int, int dmg)
 *
 * As with RockTile, the port needs one C name per signature: the slot
 * treetile_hurt() and the shared worker treetile_hurt_dmg().
 */
#ifndef LEVEL_TILE_TREE_TILE_H_
#define LEVEL_TILE_TREE_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"
#include "../../entity/mob.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void treetile_init(Tile* this, TileID id);

/* Java: TreeTile.render(Screen, Level, int, int) */
PUBLIC void treetile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: TreeTile.hurt(Level, int, int, Mob, int, int) */
PUBLIC void treetile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

/* Java: TreeTile.tick(Level, int, int) */
PUBLIC void treetile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: TreeTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean treetile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

/* Java: TreeTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean treetile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

/* Applies the felling damage: the shared body of both hurt() overloads. */
PUBLIC void treetile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg);

#endif /* LEVEL_TILE_TREE_TILE_H_ */
