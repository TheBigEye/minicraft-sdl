/*
 * hard_rock_tile.h - Hard rock (Java: tile.HardRockTile).
 *
 * It takes 200 points of accumulated damage to break, and only the gem
 * pickaxe, level 4, can deal them. It drops stone and coal.
 *
 * Java declares hurt() twice, overloaded on its arguments:
 *
 *     hurt(Level, int, int, Mob source, int dmg, int attackDir)
 *     hurt(Level, int, int, int dmg)
 *
 * As with RockTile, the port needs one C name per signature: the slot
 * hardrocktile_hurt() and the shared worker hardrocktile_hurt_dmg().
 */
#ifndef LEVEL_TILE_HARD_ROCK_TILE_H_
#define LEVEL_TILE_HARD_ROCK_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"
#include "../../entity/mob.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void hardrocktile_init(Tile* this, TileID id);

/* Java: HardRockTile.render(Screen, Level, int, int) */
PUBLIC void hardrocktile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: HardRockTile.hurt(Level, int, int, Mob, int, int) */
PUBLIC void hardrocktile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

/* Java: HardRockTile.tick(Level, int, int) */
PUBLIC void hardrocktile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: HardRockTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean hardrocktile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

/* Java: HardRockTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean hardrocktile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

/* Applies the damage: the shared body of both hurt() overloads. */
PUBLIC void hardrocktile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg);

#endif /* LEVEL_TILE_HARD_ROCK_TILE_H_ */
