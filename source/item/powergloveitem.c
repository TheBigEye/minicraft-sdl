#include "powergloveitem.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "itemids.h"
#include <string.h>
#include "../entity/furniture.h"
#include "../entity/_entity_caller.h"

void powergloveitem_create(Item* item){
	item->id = POWERGLOVE;
}
int powergloveitem_getColor(Item* item){
	return getColor4(-1, 100, 320, 430);
}
int powergloveitem_getSprite(Item* item){
	return 7+4*32;
}
void powergloveitem_renderIcon(Item* item, Screen* screen, int x, int y){
	render_screen(screen, x, y, powergloveitem_getSprite(item), powergloveitem_getColor(item), 0);
}
static char name[] = "Pow glove";

void powergloveitem_renderInventory(Item* item, Screen* screen, int x, int y){
	render_screen(screen, x, y, powergloveitem_getSprite(item), powergloveitem_getColor(item), 0);
	font_draw(powergloveitem_getName(item), strlen(name), screen, x + 8, y, getColor4(-1, 555, 555, 555));
}



char* powergloveitem_getName(Item* item){
	return name;
}
uint8_t powergloveitem_interact(Item* item, Player* player, Entity* entity, int attackDir){
	if(entity_isfurniture(entity)){
		furniture_take((Furniture *) entity, player);
		return 1;
	}
	return 0;
}
