
#ifndef LEVEL_TILE_STAIRS_TILE_H_
#define LEVEL_TILE_STAIRS_TILE_H_

#include "tile.h"
#include "../level.h"
#include "../../gfx/screen.h"

enum tileid_;
typedef struct{
	char leadsUp;
} add_stairs_;

void stairstile_init(enum tileid_ id, char leadsUp);
void stairstile_render(enum tileid_ id, Screen* screen, Level* level, int x, int y);

#endif /* LEVEL_TILE_STAIRS_TILE_H_ */
