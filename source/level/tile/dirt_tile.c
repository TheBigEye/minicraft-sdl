#include "tile.h"
#include "../../gfx/color.h"
#include "../../entity/player.h"
#include "../../item/item.h"
#include "../level.h"
#include "../../item/resourceitem.h"
#include "../../entity/itementity.h"

char dirttile_interact(TileID id, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
	if(item->id == TOOL){
		if(item->add.tool.type == SHOVEL){
			if(player_payStamina(player, 4 - item->add.tool.level)){
				level_set_tile(level, xt, yt, HOLE, 0);
				Random* random = &tiles[id].random;
				ItemEntity* entity = malloc(sizeof(ItemEntity));
				Item item;
				resourceitem_create(&item, &dirt);
				itementity_create(entity, item, xt*16 + random_next_int(random, 10) + 3, yt*16 + random_next_int(random, 10) + 3);
				level_addEntity(level, (Entity *) entity);
				//TODO sounds Sound.monsterHurt.play();
				return 1;
			}
		}

		if(item->add.tool.type == HOE){
			if(player_payStamina(player, 4 - item->add.tool.level)){
				level_set_tile(level, xt, yt, FARMLAND, 0);
				//TODO: sounds Sound.monsterHurt.play();
				return 1;
			}
		}
	}
	return 0;
}

void dirttile_render(TileID id, Screen* screen, Level* level, int x, int y){
	int col = getColor4(level->dirtColor, level->dirtColor, level->dirtColor - 111, level->dirtColor - 111);

	render_screen(screen, x*16 + 0, y * 16 + 0, 0, col, 0);
	render_screen(screen, x*16 + 8, y * 16 + 0, 1, col, 0);
	render_screen(screen, x*16 + 0, y * 16 + 8, 2, col, 0);
	render_screen(screen, x*16 + 8, y * 16 + 8, 3, col, 0);
}
