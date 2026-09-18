/*
 * stairs_tile.h - The stairs tile (Java: tile.StairsTile).
 *
 * The `leadsUp` flag decides whether it leads up or down a level, and so
 * which set of sprites it uses.
 */
#ifndef LEVEL_TILE_STAIRS_TILE_H_
#define LEVEL_TILE_STAIRS_TILE_H_ 1

#include "tile.h"

#include "../level.h"

/* Constructor. `leadsUp`: true for the way up, false for the way down. */
PUBLIC void stairstile_init(Tile* this, TileID id, boolean leadsUp);

/* Java: StairsTile.render(Screen, Level, int, int) */
PUBLIC void stairstile_render(Tile* this, Screen* screen, Level* level, int x, int y);

#endif /* LEVEL_TILE_STAIRS_TILE_H_ */
