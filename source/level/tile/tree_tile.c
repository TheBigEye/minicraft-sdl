/*
 * tree_tile.c - Tree tile behavior (Java: tile.TreeTile).
 *
 * Takes 20 accumulated damage to fell; falling trees drop wood plus
 * possible acorns, and any hit may shake an apple loose.
 */
#include "tile.h"
#include <stdlib.h>
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../entity/itementity.h"
#include "../../item/resourceitem.h"
#include "../../gfx/color.h"

#include <stdio.h>


/* Registers the tree connection flags. */
void treetile_init(TileID id) {
	tile_init(id);
	tiles[id].connectsToGrass = 1;
}


/* Internal chop-damage helper: may shake apples loose, shows smash
 * and damage feedback, and at 20 damage fells the tree into grass
 * dropping wood and possibly acorns. */
void treetile_hurt2(TileID id, Level* level, int x, int y, int dmg) {
	int count = random_next_int(&tiles[id].random, 10) == 0 ? 1 : 0;
	Random* random = &tiles[id].random;

	for (int i = 0; i < count; i++) {
        Item res;
		ItemEntity* ent = malloc(sizeof(ItemEntity));
		resourceitem_create(&res, &apple);

		int xx = (x * 16) + random_next_int(random, 10) + 3;
		int yy = (y * 16) + random_next_int(random, 10) + 3;

		itementity_create(ent, res, xx, yy);
		level_addEntity(level, &ent->entity);
	}

	int damage = level_get_data(level, x, y) + dmg;
	SmashParticle* smash = malloc(sizeof(SmashParticle));
	smashparticle_create(smash, (x * 16) + 8, (y * 16) + 8);
	level_addEntity(level, &smash->entity);

	TextParticle* text = malloc(sizeof(TextParticle));
	char* txt = malloc(16);
	sprintf(txt, "%d", dmg);
	textparticle_create(text, txt, (x * 16) + 8, (y * 16) + 8, getColor4(-1, 500, 500, 500));
	level_addEntity(level, &text->entity);

	if (damage >= 20) {
		int count = random_next_int(random, 2) + 1;

		for (int i = 0; i < count; ++i) {
            Item res;
			ItemEntity* ent = malloc(sizeof(ItemEntity));
			resourceitem_create(&res, &wood);

			int xx = (x * 16) + random_next_int(random, 10) + 3;
			int yy = (y * 16) + random_next_int(random, 10) + 3;

			itementity_create(ent, res, xx, yy);
			level_addEntity(level, &ent->entity);
		}

		count = random_next_int(random, random_next_int(random, 4) + 1);
		for (int i = 0; i < count; ++i) {
            Item res;
			ItemEntity* ent = malloc(sizeof(ItemEntity));
			resourceitem_create(&res, &acorn);

			int xx = (x * 16) + random_next_int(random, 10) + 3;
			int yy = (y * 16) + random_next_int(random, 10) + 3;

			itementity_create(ent, res, xx, yy);
			level_addEntity(level, &ent->entity);
		}

		level_set_tile(level, x, y, GRASS, 0);
	} else {
		level_set_data(level, x, y, damage);
	}
}


/* Forwards mob damage into the chop-damage path. */
void treetile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
	treetile_hurt2(id, level, x, y, dmg);
}


/* Draws the canopy quadrants, showing bark sprites where neighboring
 * tree tiles form the trunk. */
void treetile_render(TileID id, Screen* screen, Level* level, int x, int y) {
	int col = getColor4(10, 30, 151, level->grassColor);
	int barkCol1 = getColor4(10, 30, 430, level->grassColor);
	int barkCol2 = getColor4(10, 30, 320, level->grassColor);

	char u = level_get_tile(level, x, y - 1) == id;
	char l = level_get_tile(level, x - 1, y) == id;
	char r = level_get_tile(level, x + 1, y) == id;
	char d = level_get_tile(level, x, y + 1) == id;

	char ul = level_get_tile(level, x - 1, y - 1) == id;
	char ur = level_get_tile(level, x + 1, y - 1) == id;
	char dl = level_get_tile(level, x - 1, y + 1) == id;
	char dr = level_get_tile(level, x + 1, y + 1) == id;

	if (u && ul && l) {
        render_screen(screen, (x * 16) + 0, (y * 16) + 0, 10 + 1 * 32, col, 0);
    } else {
        render_screen(screen, (x * 16) + 0, (y * 16) + 0, 9 + 0 * 32, col, 0);
    }

	if (u && ur && r) {
        render_screen(screen, (x * 16) + 8, (y * 16) + 0, 10 + 2 * 32, barkCol2, 0);
    } else {
        render_screen(screen, (x * 16) + 8, (y * 16) + 0, 10 + 0 * 32, col, 0);
    }

	if (d && dl && l) {
        render_screen(screen, (x * 16) + 0, (y * 16) + 8, 10 + 2 * 32, barkCol2, 0);
    } else {
        render_screen(screen, (x * 16) + 0, (y * 16) + 8, 9 + 1 * 32, barkCol1, 0);
    }

	if (d && dr && r) {
        render_screen(screen, (x * 16) + 8, (y * 16) + 8, 10 + 1 * 32, col, 0);
    } else {
        render_screen(screen, (x * 16) + 8, (y * 16) + 8, 10 + 3 * 32, barkCol2, 0);
    }
}


/* Heals one point of accumulated damage per tick, if any. */
void treetile_tick(TileID id, Level* level, int xt, int yt) {
	int damage = level_get_data(level, xt, yt);
	if (damage) {
        level_set_data(level, xt, yt, damage - 1);
    }
}


/* Axe interaction: each swing lands a random chunk of damage scaled
 * by the tool level. */
char treetile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir) {
	if (item->id == TOOL) {
		if (item->add.tool.type == AXE) {
			if (player_payStamina(player, 4 - item->add.tool.level)) {
				treetile_hurt2(id, level, xt, yt, random_next_int(&tiles[id].random, 10) + (item->add.tool.level * 5) + 10);
				return 1;
			}
		}
	}
	return 0;
}
