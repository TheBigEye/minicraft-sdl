#include "tile.h"
#include "../../gfx/color.h"
#include "../../entity/player.h"
#include "../../item/item.h"

void farmlandtile_render(TileID id, Screen* screen, Level* level, int x, int y){
	int col = getColor4(level->dirtColor - 121, level->dirtColor - 11, level->dirtColor, level->dirtColor + 111);

	render_screen(screen, x*16 + 0, y * 16 + 0, 2 + 32, col, 1);
	render_screen(screen, x*16 + 8, y * 16 + 0, 2 + 32, col, 0);
	render_screen(screen, x*16 + 0, y * 16 + 8, 2 + 32, col, 0);
	render_screen(screen, x*16 + 8, y * 16 + 8, 2 + 32, col, 1);
}

char farmtile_interact(TileID id, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
	if(item->id == TOOL){
		if(item->add.tool.type == SHOVEL){
			if(player_payStamina(player, 4- item->add.tool.level)){
				level_set_tile(level, xt, yt, DIRT, 0);
				return 1;
			}
		}
	}
	return 0;
}

void farmland_tick(TileID id, Level* level, int xt, int yt) {
	int age = level_get_data(level, xt, yt);
	if(age < 5) level_set_data(level, xt, yt, age + 1);
}
