/*
 * furniture_item.c - Furniture item behavior (Java: FurnitureItem).
 */
#include "furniture_item.h"
#include "item.h"
#include "../entity/furniture.h"
#include "../gfx/screen.h"
#include "../gfx/font.h"
#include "../gfx/color.h"

#include <string.h>


/* Tags the item as furniture and takes ownership of the entity. */
void furnitureitem_create(Item* item, Furniture* furniture){
	item->id = FURNITURE;
	item->add.furniture.furniture = furniture;
	item->add.furniture.placed = 0;
}


/* Icon color comes from the carried furniture. */
int furnitureitem_getColor(Item* item){
	return item->add.furniture.furniture->col;
}


/* Icon sprite is the furniture sprite offset into the item row. */
int furnitureitem_getSprite(Item* item){
	return item->add.furniture.furniture->sprite + 10 * 32;
}


void furnitureitem_renderIcon(Item* item, Screen* screen, int x, int y){
	render_screen(screen, x, y, furnitureitem_getSprite(item), furnitureitem_getColor(item), 0);
}


/* Draws icon plus the furniture's name in the inventory row. */
void furnitureitem_renderInventory(Item* item, Screen* screen, int x, int y){
	render_screen(screen, x, y, furnitureitem_getSprite(item), furnitureitem_getColor(item), 0);
	font_draw(item->add.furniture.furniture->name, strlen(item->add.furniture.furniture->name), screen, x + 8, y, getColor4(-1, 555, 555, 555));
}


/* Drops the carried furniture centered on the tile when the tile
 * allows it; the item is then marked placed (ownership transferred
 * to the level). */
char furnitureitem_interactOn(Item* item, TileID tile, Level* level, int xt, int yt, struct _Player* player, int attackDir) {
	if (tile_mayPass(tile, level, xt, yt, (Entity *) item->add.furniture.furniture)) {
		item->add.furniture.furniture->entity.x = xt * 16 + 8;
		item->add.furniture.furniture->entity.y = yt * 16 + 8;
		level_addEntity(level, (Entity *) item->add.furniture.furniture);
		item->add.furniture.placed = 1;
		item->add.furniture.furniture = 0;
		return 1;
	}

	return 0;
}
