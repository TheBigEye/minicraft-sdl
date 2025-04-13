#include "tile.h"
#include "../../gfx/color.h"
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../entity/itementity.h"
#include "../../item/resource/resource.h"
#include "../../item/resourceitem.h"

void rocktile_hurt_(TileID id, Level* level, int x, int y, int dmg){
	int damage = level_get_data(level, x, y) + dmg;

	SmashParticle* smash = malloc(sizeof(SmashParticle));
	smashparticle_create(smash, (x * 16) + 8, (y * 16) + 8);
	level_addEntity(level, &smash->entity);

	TextParticle* text = malloc(sizeof(TextParticle));
	char* txt = malloc(16);
	sprintf(txt, "%d", dmg);
	textparticle_create(text, txt, (x * 16) + 8, (y * 16) + 8, getColor4(-1, 500, 500, 500));
	level_addEntity(level, &text->entity);

	Random* random = &tiles[id].random;
	if (damage >= 50) {
		int count = random_next_int(random, 4) + 1;

		for (int i = 0; i < count; ++i) {
            Item resource;
			ItemEntity* item_entity = malloc(sizeof(ItemEntity));
			resourceitem_create(&resource, &stone);

			int xx = (x * 16) + random_next_int(random, 10) + 3;
			int yy = (y * 16) + random_next_int(random, 10) + 3;

			itementity_create(item_entity, resource, xx, yy);
			level_addEntity(level, &item_entity->entity);
		}

		count = random_next_int(random, 2);

		for (int i = 0; i < count; ++i) {
            Item resource;
			ItemEntity* item_entity = malloc(sizeof(ItemEntity));
			resourceitem_create(&resource, &coal);

			int xx = (x * 16) + random_next_int(random, 10) + 3;
			int yy = (y * 16) + random_next_int(random, 10) + 3;

			itementity_create(item_entity, resource, xx, yy);
			level_addEntity(level, &item_entity->entity);
		}

		level_set_tile(level, x, y, DIRT, 0);
	} else {
		level_set_data(level, x, y, damage);
	}
}


char rocktile_interact(TileID id, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
	if (item->id == TOOL) {
		if (item->add.tool.type == PICKAXE) {
			if (player_payStamina(player, 4 - item->add.tool.level)) {
				rocktile_hurt_(id, level, xt, yt, random_next_int(&tiles[id].random, 10) + (item->add.tool.level) * 5 + 10);
				return 1;
			}
		}
	}
	return 0;
}


void rocktile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
	rocktile_hurt_(id, level, x, y, dmg);
}


void rocktile_render(TileID id, Screen* screen, Level* level, int x, int y) {
	int col = getColor4(444, 444, 333, 333);
	int transitionColor = getColor4(111, 444, 555, level->dirtColor);

	char u = level_get_tile(level, x, y - 1) != id;
	char d = level_get_tile(level, x, y + 1) != id;
	char l = level_get_tile(level, x - 1, y) != id;
	char r = level_get_tile(level, x + 1, y) != id;

	char ul = level_get_tile(level, x - 1, y - 1) != id;
	char dl = level_get_tile(level, x - 1, y + 1) != id;
	char ur = level_get_tile(level, x + 1, y - 1) != id;
	char dr = level_get_tile(level, x + 1, y + 1) != id;

	if (!u && !l) {
		if (!ul) {
            render_screen(screen, (x * 16) + 0, (y * 16) + 0, 0, col, 0);
        } else {
            render_screen(screen, (x * 16) + 0, (y * 16) + 0, 7 + 0 * 32, transitionColor, 3);
        }
	} else {
        render_screen(screen, (x * 16) + 0, (y * 16) + 0, (l ? 6 : 5) + (u ? 2 : 1) * 32, transitionColor, 3);
    }

	if (!u && !r) {
		if (!ur) {
            render_screen(screen, (x * 16) + 8, (y * 16) + 0, 1, col, 0);
        } else {
            render_screen(screen, (x * 16 + 8), (y * 16) + 0, 8 + 0 * 32, transitionColor, 3);
        }
	} else {
        render_screen(screen, (x * 16) + 8, (y * 16) + 0, (r ? 4 : 5) + (u ? 2 : 1) * 32, transitionColor, 3);
    }

	if (!d && !l) {
		if (!dl) {
            render_screen(screen, (x * 16) + 0, (y * 16) + 8, 2, col, 0);
        } else {
            render_screen(screen, (x * 16) + 0, (y * 16) + 8, 7 + 1 * 32, transitionColor, 3);
        }
	} else {
        render_screen(screen, (x * 16) + 0, (y * 16) + 8, (l ? 6 : 5) + (d ? 0 : 1) * 32, transitionColor, 3);
    }

	if (!d && !r) {
		if (!dr) {
            render_screen(screen, (x * 16) + 8, (y * 16) + 8, 3, col, 0);
        } else {
            render_screen(screen, (x * 16) + 8, (y * 16) + 8, 8 + 1 * 32, transitionColor, 3);
        }
	} else {
        render_screen(screen, (x * 16) + 8, (y * 16) + 8, (r ? 4 : 5) + (d ? 0 : 1) * 32, transitionColor, 3);
    }
}


void rocktile_tick(TileID id, Level* level, int xt, int yt) {
	int damage = level_get_data(level, xt, yt);
	if (damage) {
        level_set_data(level, xt, yt, damage - 1);
    }
}
