#include "furniture.h"
#include "entity.h"
#include "player.h"
#include "../item/item.h"
#include "inventory.h"
#include "../item/furniture_item.h"
#include "_entity_caller.h"

void furniture_create(Furniture* furniture, char* name){
	entity_create((Entity *) furniture);

	furniture->pushTime = 0;
	furniture->pushDir = -1;
	furniture->col = 0;
	furniture->sprite = 0;
	furniture->shouldTake = 0;

	furniture->name = name;
	furniture->entity.xr = 3;
	furniture->entity.yr = 3;
}


void furniture_tick(Furniture* furniture){
	if (furniture->shouldTake){
		Item* item = furniture->shouldTake->activeItem;
		if (item && item->id == POWERGLOVE){
			Furniture* cp = furniture_create_copy(furniture); // XXX ew
			entity_remove((Entity *) furniture);
			inventory_addItemIntoSlot_nalloc(&furniture->shouldTake->inventory, 0, item);
			item = malloc(sizeof(Item));
			furnitureitem_create(item, cp);
			furniture->shouldTake->activeItem = item;
		}

		furniture->shouldTake = 0;
	}

	if (furniture->pushDir == 0) entity_move((Entity *) furniture, 0, 1);
	if (furniture->pushDir == 1) entity_move((Entity *) furniture, 0, -1);
	if (furniture->pushDir == 2) entity_move((Entity *) furniture, -1, 0);
	if (furniture->pushDir == 3) entity_move((Entity *) furniture, 1, 0);

	furniture->pushDir = -1;

	if (furniture->pushTime > 0) --furniture->pushTime;
}


void furniture_render(Furniture* furniture, Screen* screen){
	int x = furniture->entity.x;
	int y = furniture->entity.y;
	int sprite = furniture->sprite;
	int col = furniture->col;

	render_screen(screen, x - 8, y - 8 - 4, sprite * 2 + 8 * 32, col, 0);
	render_screen(screen, x - 0, y - 8 - 4, sprite * 2 + 8 * 32 + 1, col, 0);
	render_screen(screen, x - 8, y - 0 - 4, sprite * 2 + 8 * 32 + 32, col, 0);
	render_screen(screen, x - 0, y - 0 - 4, sprite * 2 + 8 * 32 + 33, col, 0);
}


void furniture_touchedBy(Furniture* furniture, Entity* entity){
	if(entity->type == PLAYER && furniture->pushTime == 0){
		furniture->pushDir = ((Mob*) entity)->dir;
		furniture->pushTime = 10;
	}
}


void furniture_take(Furniture* furniture, Player* player){
	furniture->shouldTake = player;
}


void furniture_free(Furniture* furniture){
	free(furniture->name);
}
