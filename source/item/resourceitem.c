#include "itemids.h"
#include "resourceitem.h"
#include "resource/resource.h"
#include <stdlib.h>
#include "../gfx/font.h"
#include <string.h>
#include "../gfx/color.h"

void resourceitem_create(Item* item, Resource* resource){
	item->id = RESOURCE;
	item->add.resource.resource = resource;
	item->add.resource.count = 1;
}

void resourceitem_create_cnt(Item* item, Resource* resource, int count){
	item->id = RESOURCE;
	item->add.resource.resource = resource;
	item->add.resource.count = count;
}

int resourceitem_getColor(Item* item){
	return item->add.resource.resource->color;
}

int resourceitem_getSprite(Item* item){
	return item->add.resource.resource->sprite;
}

void resourceitem_renderIcon(Item* item, Screen* screen, int x, int y){
	render_screen(screen, x, y, item->add.resource.resource->sprite, item->add.resource.resource->color, 0);
}

void resourceitem_renderInventory(Item* item, Screen* screen, int x, int y){
	render_screen(screen, x, y, item->add.resource.resource->sprite, item->add.resource.resource->color, 0);
	font_draw(item->add.resource.resource->name, strlen(item->add.resource.resource->name), screen, x + 32, y, getColor4(-1, 555, 555, 555));
	int cc = item->add.resource.count;
	if(cc > 999) cc = 999;
	char num[4] = {0, 0, 0, 0};
	sprintf(num, "%d", cc);
	font_draw(num, 3, screen, x + 8, y, getColor4(-1, 444, 444, 444));
}

char* resourceitem_getName(Item* item){
	return item->add.resource.resource->name;
}

char resourceitem_interactOn(Item* item, TileID tile, Level* level, int xt, int yt, Player* player, int attackDir){
	char b = resource_interactOn(item->add.resource.resource, tile, level, xt, yt, player, attackDir);
	if(b){
		--item->add.resource.count;
		return 1;
	}
	return 0;
}

char resourceitem_isDepleted(Item* item){
	return item->add.resource.count <= 0;
}
