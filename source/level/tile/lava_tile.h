/*
 * lava_tile.h - The lava tile (Java: tile.LavaTile).
 *
 * An animated, glowing surface (light radius 6) crossable only by
 * swimming; it slowly spreads into neighbouring holes.
 */
#ifndef LEVEL_TILE_LAVA_TILE_H_
#define LEVEL_TILE_LAVA_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/entity.h"

/* Constructor */
PUBLIC void lavatile_init(Tile* this, TileID id);

/* Java: LavaTile.render(Screen, Level, int, int) */
PUBLIC void lavatile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: LavaTile.tick(Level, int, int) */
PUBLIC void lavatile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: LavaTile.mayPass(Level, int, int, Entity) */
PUBLIC boolean lavatile_may_pass(Tile* this, Level* level, int x, int y, Entity* e);

/* Java: LavaTile.getLightRadius(Level, int, int) */
PUBLIC int lavatile_get_light_radius(Tile* this, Level* level, int x, int y);

#endif /* LEVEL_TILE_LAVA_TILE_H_ */
