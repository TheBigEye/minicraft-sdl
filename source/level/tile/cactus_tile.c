/*
 * cactus_tile.c - Cactus tile behavior (Java: tile.CactusTile).
 *
 * Damage taken is stored in the tile data and heals down each tick;
 * enough damage breaks the cactus into cactus-flower drops.
 */
#include "tile.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../gfx/color.h"
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../entity/itementity.h"
#include "../../item/resourceitem.h"


/* Registers the cactus connection flags and defaults. */
void cactustile_init(TileID id) {
	tile_init(id);
	tiles[id].connectsToSand = 1;
}


/* Draws the cactus sprite over the sand background. */
void cactustile_render(TileID id, Screen* screen, Level* level, int x, int y) {
	int col = getColor4(20, 40, 50, level->sandColor);

	render_screen(screen, (x * 16) + 0, (y * 16) + 0, 8 + 2 * 32, col, 0);
	render_screen(screen, (x * 16) + 8, (y * 16) + 0, 9 + 2 * 32, col, 0);
	render_screen(screen, (x * 16) + 0, (y * 16) + 8, 8 + 3 * 32, col, 0);
	render_screen(screen, (x * 16) + 8, (y * 16) + 8, 9 + 3 * 32, col, 0);
}


/* Adds damage to the tile data with smash/damage feedback; at 10 the
 * cactus breaks into sand and drops 1-2 cactus flowers. */
void cactus_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
	int damage = level_get_data(level, x, y) + dmg;
	Random* random = &tiles[id].random;

	SmashParticle* smash = malloc(sizeof(SmashParticle));
	smashparticle_create(smash, (x * 16) + 8, (y * 16) + 8);
	level_addEntity(level, &smash->entity);

	TextParticle* text = malloc(sizeof(TextParticle));
	char* txt = malloc(16);
	sprintf(txt, "%d", dmg);
	textparticle_create(text, txt, (x * 16) + 8, (y * 16) + 8, getColor4(-1, 500, 500, 500));
	level_addEntity(level, &text->entity);

	if (damage >= 10) {
		int count = random_next_int(random, 2) + 1;

		for (int i = 0; i < count; ++i) {
            Item res;
			ItemEntity* ent = malloc(sizeof(ItemEntity));
			resourceitem_create(&res, &cactusFlower);

			int xx = (x * 16) + random_next_int(random, 10) + 3;
			int yy = (y * 16) + random_next_int(random, 10) + 3;

			itementity_create(ent, res, xx, yy);
			level_addEntity(level, (Entity *) ent);
		}

		level_set_tile(level, x, y, SAND, 0);
	} else {
		level_set_data(level, x, y, damage);
	}
}


/* Heals one point of accumulated damage per tick, if any. */
void cactustile_tick(TileID id, Level* level, int xt, int yt) {
	int damage = level_get_data(level, xt, yt);

	if (damage) {
        level_set_data(level, xt, yt, damage - 1);
    }
}
