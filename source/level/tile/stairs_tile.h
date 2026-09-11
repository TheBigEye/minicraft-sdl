/*
 * stairs_tile.h - Stairs tile: the exit between level depths; one
 *                 variant leads up, the other leads down.
 */
#ifndef LEVEL_TILE_STAIRS_TILE_H_
#define LEVEL_TILE_STAIRS_TILE_H_

#include "tile.h"
#include "../level.h"
#include "../../gfx/screen.h"

enum tileid_;
/* Extra per-tile state for stairs. */
typedef struct{
	char leadsUp; /* Selects the up or down staircase sprite. */
} add_stairs_;

/* Configures whether this staircase leads up or down. */
void stairstile_init(enum tileid_ id, char leadsUp);
void stairstile_render(enum tileid_ id, Screen* screen, Level* level, int x, int y);

#endif /* LEVEL_TILE_STAIRS_TILE_H_ */
