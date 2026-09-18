/*
 * cactus_tile.h - The cactus tile (Java: tile.CactusTile).
 *
 * Damage is kept in the data byte and heals one point per tick; at 10 the
 * cactus breaks into sand and drops cactus flowers. It also pricks anyone
 * who touches it.
 */
#ifndef LEVEL_TILE_CACTUS_TILE_H_
#define LEVEL_TILE_CACTUS_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"
#include "../../entity/mob.h"

/* Constructor */
PUBLIC void cactustile_init(Tile* this, TileID id);

/* Java: CactusTile.render(Screen, Level, int, int) */
PUBLIC void cactustile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: CactusTile.hurt(Level, int, int, Mob, int, int) */
PUBLIC void cactustile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

/* Java: CactusTile.tick(Level, int, int) */
PUBLIC void cactustile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: CactusTile.bumpedInto(Level, int, int, Entity) */
PUBLIC void cactustile_bumped_into(Tile* this, Level* level, int xt, int yt, Entity* entity);

/* Java: CactusTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean cactustile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

#endif /* LEVEL_TILE_CACTUS_TILE_H_ */
