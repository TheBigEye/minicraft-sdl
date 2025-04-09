#include "tile.h"
#include "../../entity/player.h"
#include "../../item/item.h"
#include "../../item/resourceitem.h"
#include "../../entity/itementity.h"
#include "../../gfx/color.h"

static Random trandom;

void wheat_tile_init(TileID id) {
	tile_init(id);
	random_set_seed(&trandom, getTimeUS() / 1000);
}

char wheattile_interact(TileID id, Level* level, int xt, int yt, struct _Player* player, struct _Item* item, int attackDir) {

	if(item->id == TOOL){
		if(item->add.tool.type == SHOVEL){
			if(player_payStamina(player, 4 - item->add.tool.level)){
				level_set_tile(level, xt, yt, DIRT, 0);
				return 1;
			}
		}
	}

	return 0;
}

void wheattile_harvest(TileID id, Level* level, int x, int y) {
	int age = level_get_data(level, x, y);
	Random* random = &tiles[id].random;
	int count = random_next_int(random, 2);

	for(int i = 0; i < count; ++i){
		ItemEntity* ent = malloc(sizeof(ItemEntity));
		Item res;
		resourceitem_create(&res, &seeds);
		int xx = x * 16 + random_next_int(random, 10) + 3;
		int yy = y * 16 + random_next_int(random, 10) + 3;
		itementity_create(ent, res, xx, yy);
		level_addEntity(level, (Entity *) ent);
	}

	count = 0;
	if(age == 50){
		count = random_next_int(random, 3) + 2;
	}else if(age >= 40){
		count = random_next_int(random, 2) + 1;
	}

	for(int i = 0; i < count; ++i){
		ItemEntity* ent = malloc(sizeof(ItemEntity));
		Item res;
		resourceitem_create(&res, &wheat);
		int xx = x * 16 + random_next_int(random, 10) + 3;
		int yy = y * 16 + random_next_int(random, 10) + 3;
		itementity_create(ent, res, xx, yy);
		level_addEntity(level, (Entity *) ent);
	}
	level_set_tile(level, x, y, DIRT, 0);
}

void wheattile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir){
	wheattile_harvest(id, level, x, y);
}

void wheattile_render(TileID id, Screen* screen, Level* level, int x, int y){
	int age = level_get_data(level, x, y);
	int col = getColor4(level->dirtColor - 121, level->dirtColor - 11, level->dirtColor, 50);
	int icon = age / 10;
	if (icon >= 3){
		col = getColor4(level->dirtColor - 121, level->dirtColor - 11, 50 + (icon) * 100, 40 + (icon - 3) * 2 * 100);
		if (age == 50) col = getColor4(0, 0, 50 + (icon) * 100, 40 + (icon - 3) * 2 * 100);
		icon = 3;
	}

	render_screen(screen, x * 16 + 0, y * 16 + 0, 4 + 3 * 32 + icon, col, 0);
	render_screen(screen, x * 16 + 8, y * 16 + 0, 4 + 3 * 32 + icon, col, 0);
	render_screen(screen, x * 16 + 0, y * 16 + 8, 4 + 3 * 32 + icon, col, 1);
	render_screen(screen, x * 16 + 8, y * 16 + 8, 4 + 3 * 32 + icon, col, 1);
}


void wheattile_tick(TileID id, Level* level, int xt, int yt){
	if(random_next_int(&trandom, 2) == 0) return;

	int age = level_get_data(level, xt, yt);
	if(age < 50) level_set_data(level, xt, yt, age + 1);
}
