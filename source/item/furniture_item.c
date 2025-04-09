#include "furniture_item.h"
#include "item.h"
#include "../entity/furniture.h"
#include "../gfx/screen.h"
#include "../gfx/font.h"
#include "../gfx/color.h"

#include <string.h>

void furnitureitem_create(Item* item, Furniture* furniture){
	item->id = FURNITURE;
	item->add.furniture.furniture = furniture;
	item->add.furniture.placed = 0;
}

int furnitureitem_getColor(Item* item){
	return item->add.furniture.furniture->col;
}

int furnitureitem_getSprite(Item* item){
	return item->add.furniture.furniture->sprite + 10 * 32;
}

void furnitureitem_renderIcon(Item* item, Screen* screen, int x, int y){
	render_screen(screen, x, y, furnitureitem_getSprite(item), furnitureitem_getColor(item), 0);
}

void furnitureitem_renderInventory(Item* item, Screen* screen, int x, int y){
	render_screen(screen, x, y, furnitureitem_getSprite(item), furnitureitem_getColor(item), 0);
	font_draw(item->add.furniture.furniture->name, strlen(item->add.furniture.furniture->name), screen, x + 8, y, getColor4(-1, 555, 555, 555));
}

char furnitureitem_interactOn(Item* item, TileID tile, Level* level, int xt, int yt, struct _Player* player, int attackDir){
	if(tile_mayPass(tile, level, xt, yt, (Entity *) item->add.furniture.furniture)){
		item->add.furniture.furniture->entity.x = xt * 16 + 8;
		item->add.furniture.furniture->entity.y = yt * 16 + 8;
		level_addEntity(level, (Entity *) item->add.furniture.furniture);
		item->add.furniture.placed = 1;
		item->add.furniture.furniture = 0;
		return 1;
	}

	return 0;
}
