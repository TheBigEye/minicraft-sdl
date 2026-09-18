/*
 * rock_tile.h - The rock tile (Java: tile.RockTile).
 *
 * It takes 50 points of accumulated damage before breaking into dirt,
 * dropping stone and sometimes coal; the damage heals over time.
 *
 * Java declares hurt() twice, overloaded on its arguments:
 *
 *     hurt(Level, int, int, Mob source, int dmg, int attackDir)
 *     hurt(Level, int, int, int dmg)
 *
 * In C a struct slot has a single signature, so the port keeps one name
 * per signature: the slot-shaped rocktile_hurt(), which is the method the
 * level calls, and rocktile_hurt_dmg(), the worker both overloads shared.
 * See javalang.h for the LANG_PICK / LANG_FIRST macros that give a single name
 * at the call site when that is wanted.
 */
#ifndef LEVEL_TILE_ROCK_TILE_H_
#define LEVEL_TILE_ROCK_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"
#include "../../entity/mob.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor. Java: RockTile(int id) */
PUBLIC void rocktile_init(Tile* this, TileID id);

/* Java: RockTile.render(Screen, Level, int, int) */
PUBLIC void rocktile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: RockTile.hurt(Level, int, int, Mob, int, int) */
PUBLIC void rocktile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

/* Java: RockTile.tick(Level, int, int) */
PUBLIC void rocktile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: RockTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean rocktile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

/* Java: RockTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean rocktile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

/*
 * Applies the damage: the shared body of both hurt() overloads, which is
 * the reason the original had two of them.
 * Java: RockTile.hurt(Level, int, int, int dmg)
 */
PUBLIC void rocktile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg);

#endif /* LEVEL_TILE_ROCK_TILE_H_ */
