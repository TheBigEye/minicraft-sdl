/*
 * cloud_cactus_tile.h - The cloud cactus (Java: tile.CloudCactusTile).
 *
 * A hazard of the sky island: only the Air Wizard can pass through it, it
 * hurts whoever touches it, and the pickaxe chips away at it until it
 * becomes cloud again.
 *
 * Java declares hurt() twice, overloaded on its arguments:
 *
 *     hurt(Level, int, int, Mob source, int dmg, int attackDir)
 *     hurt(Level, int, int, int dmg)
 *
 * As with RockTile, the port needs one C name per signature: the slot
 * cloudcactustile_hurt() and the shared worker cloudcactustile_hurt_dmg().
 */
#ifndef LEVEL_TILE_CLOUD_CACTUS_TILE_H_
#define LEVEL_TILE_CLOUD_CACTUS_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"
#include "../../entity/mob.h"
#include "../../entity/player.h"
#include "../../item/item.h"

/* Constructor */
PUBLIC void cloudcactustile_init(Tile* this, TileID id);

/* Java: CloudCactusTile.render(Screen, Level, int, int) */
PUBLIC void cloudcactustile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: CloudCactusTile.hurt(Level, int, int, Mob, int, int) */
PUBLIC void cloudcactustile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

/* Java: CloudCactusTile.bumpedInto(Level, int, int, Entity) */
PUBLIC void cloudcactustile_bumped_into(Tile* this, Level* level, int xt, int yt, Entity* entity);

/* Java: CloudCactusTile.interact(Level, int, int, Player, Item, int) */
PUBLIC boolean cloudcactustile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir);

/* Java: CloudCactusTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean cloudcactustile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

/* One swing of the pickaxe: the shared body of both hurt() overloads. */
PUBLIC void cloudcactustile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg);

#endif /* LEVEL_TILE_CLOUD_CACTUS_TILE_H_ */
