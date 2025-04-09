#include "tile.h"
#include "../../utils/javarandom.h"
#include "../../entity/player.h"
#include "../../item/item.h"
#include "../../entity/itementity.h"
#include "../level.h"
#include "../../gfx/color.h"
#include "../../item/resourceitem.h"

static Random trandom;

void grasstile_init(TileID id){
	tile_init(id);
	Tile* t = tiles + id;
	random_set_seed(&trandom, getTimeUS() / 1000);

	t->connectsToGrass = 1;
}

char grasstile_interact(TileID id, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
	if(item->id == TOOL){
		if(item->add.tool.type == SHOVEL){
			if(player_payStamina(player, 4 - item->add.tool.level)){
				level_set_tile(level, xt, yt, DIRT, 0);
				//TODO sounds Sound.monsterHurt.play();
				Random* random = &tiles[id].random;

				if (random_next_int(random, 5) == 0) {
					ItemEntity* entity = malloc(sizeof(ItemEntity));
					Item item;
					resourceitem_create(&item, &seeds);
					itementity_create(entity, item, xt*16 + random_next_int(random, 10) + 3, yt*16 + random_next_int(random, 10) + 3);
					level_addEntity(level, (Entity *) entity);
					return 1;
				}
			}
		}

		if(item->add.tool.type == HOE){
			if(player_payStamina(player, 4 - item->add.tool.level)){
				//TODO sounds Sound.monsterHurt.play();
				Random* random = &tiles[id].random;
				if(random_next_int(random, 5) == 0){
					ItemEntity* entity = malloc(sizeof(ItemEntity));
					Item item;
					resourceitem_create(&item, &seeds);
					itementity_create(entity, item, xt*16 + random_next_int(random, 10) + 3, yt*16 + random_next_int(random, 10) + 3);
					level_addEntity(level, (Entity *) entity);
					return 1;
				}

				level_set_tile(level, xt, yt, FARMLAND, 0);
				return 1;
			}
		}
	}
	return 0;
}

void grasstile_render(TileID id, Screen* screen, Level* level, int x, int y) {
	int col = getColor4(level->grassColor, level->grassColor, level->grassColor + 111, level->grassColor + 111);
	int transitionColor = getColor4(level->grassColor - 111, level->grassColor, level->grassColor + 111, level->dirtColor);

	char u = !tiles[level_get_tile(level, x, y - 1)].connectsToGrass;
	char d = !tiles[level_get_tile(level, x, y + 1)].connectsToGrass;
	char l = !tiles[level_get_tile(level, x - 1, y)].connectsToGrass;
	char r = !tiles[level_get_tile(level, x + 1, y)].connectsToGrass;

	if (!u && !l) render_screen(screen, x * 16 + 0, y * 16 + 0, 0, col, 0);
	else render_screen(screen, x * 16 + 0, y * 16 + 0, (l ? 11 : 12) + (u ? 0 : 1) * 32, transitionColor, 0);

	if (!u && !r) render_screen(screen, x * 16 + 8, y * 16 + 0, 1, col, 0);
	else render_screen(screen, x * 16 + 8, y * 16 + 0, (r ? 13 : 12) + (u ? 0 : 1) * 32, transitionColor, 0);

	if (!d && !l) render_screen(screen, x * 16 + 0, y * 16 + 8, 2, col, 0);
	else render_screen(screen, x * 16 + 0, y * 16 + 8, (l ? 11 : 12) + (d ? 2 : 1) * 32, transitionColor, 0);

	if (!d && !r) render_screen(screen, x * 16 + 8, y * 16 + 8, 3, col, 0);
	else render_screen(screen, x * 16 + 8, y * 16 + 8, (r ? 13 : 12) + (d ? 2 : 1) * 32, transitionColor, 0);
}

void grasstile_tick(TileID id, Level* level, int xt, int yt){
	if(random_next_int(&trandom, 40)) return;

	int xn = xt;
	int yn = yt;

	if(random_next_boolean(&trandom)) xn += random_next_int(&trandom, 2) * 2 - 1;
	else yn += random_next_int(&trandom, 2) * 2 - 1;

	if(level_get_tile(level, xn, yn) == DIRT) level_set_tile(level, xn, yn, id, 0);
}
