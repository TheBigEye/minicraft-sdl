#include "tile.h"
#include "../../item/item.h"
#include "../../entity/itementity.h"
#include "../../gfx/color.h"
#include "../../item/resourceitem.h"

void sandtile_init(TileID id){
	tile_init(id);
	tiles[id].connectsToSand = 1;
}

char sandtile_interact(TileID id, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {

	if(item->id == TOOL){
		if(item->add.tool.type == SHOVEL){
			if(player_payStamina(player, 4 - item->add.tool.level)){
				level_set_tile(level, xt, yt, DIRT, 0);
				Random* random = &tiles[id].random;
				ItemEntity* entity = malloc(sizeof(ItemEntity));
				Item item;
				resourceitem_create(&item, &sand);
				itementity_create(entity, item, xt*16 + random_next_int(random, 10) + 3, yt*16 + random_next_int(random, 10) + 3);
				level_addEntity(level, (Entity *) entity);
				return 1;
			}
		}
	}
	return 0;
}

void sandtile_tick(TileID id, Level* level, int xt, int yt){
	int damage = level_get_data(level, xt, yt);
	if(damage) level_set_data(level, xt, yt, damage - 1);
}

void sandtile_render(TileID id, Screen* screen, Level* level, int x, int y){
	int col = getColor4(level->sandColor + 2, level->sandColor, level->sandColor - 110, level->sandColor - 110);
	int transitionColor = getColor4(level->sandColor - 110, level->sandColor, level->sandColor - 110, level->dirtColor);

	char u = !tiles[level_get_tile(level, x, y - 1)].connectsToSand;
	char d = !tiles[level_get_tile(level, x, y + 1)].connectsToSand;
	char l = !tiles[level_get_tile(level, x - 1, y)].connectsToSand;
	char r = !tiles[level_get_tile(level, x + 1, y)].connectsToSand;
	char steppedOn = level_get_data(level, x, y) > 0;

	if (!u && !l){
		if (!steppedOn) render_screen(screen, x * 16 + 0, y * 16 + 0, 0, col, 0);
		else render_screen(screen, x * 16 + 0, y * 16 + 0, 3 + 1 * 32, col, 0);
	} else render_screen(screen, x * 16 + 0, y * 16 + 0, (l ? 11 : 12) + (u ? 0 : 1) * 32, transitionColor, 0);

	if (!u && !r) render_screen(screen, x * 16 + 8, y * 16 + 0, 1, col, 0);
	else render_screen(screen, x * 16 + 8, y * 16 + 0, (r ? 13 : 12) + (u ? 0 : 1) * 32, transitionColor, 0);

	if (!d && !l) render_screen(screen, x * 16 + 0, y * 16 + 8, 2, col, 0);
	else render_screen(screen, x * 16 + 0, y * 16 + 8, (l ? 11 : 12) + (d ? 2 : 1) * 32, transitionColor, 0);

	if (!d && !r){
		if (!steppedOn) render_screen(screen, x * 16 + 8, y * 16 + 8, 3, col, 0);
		else render_screen(screen, x * 16 + 8, y * 16 + 8, 3 + 1 * 32, col, 0);
	}else render_screen(screen, x * 16 + 8, y * 16 + 8, (r ? 13 : 12) + (d ? 2 : 1) * 32, transitionColor, 0);
}
