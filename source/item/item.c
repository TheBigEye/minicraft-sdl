#include "item.h"
#include "powergloveitem.h"
#include "resourceitem.h"
#include "furniture_item.h"
#include "../entity/furniture.h"
#include "../entity/_entity_caller.h"
#include "tool_item.h"
#include <string.h>

int item_getColor(Item* item){
	switch(item->id){
		case POWERGLOVE:
			return powergloveitem_getColor(item);
		case RESOURCE:
			return resourceitem_getColor(item);
		case TOOL:
			return toolitem_getColor(item);
		case FURNITURE:
			return furnitureitem_getColor(item);
		default:
			return 0;
	}
}
int item_getSprite(Item* item){
	switch(item->id){
		case POWERGLOVE:
			return powergloveitem_getSprite(item);
		case RESOURCE:
			return resourceitem_getSprite(item);
		case TOOL:
			return toolitem_getSprite(item);
		case FURNITURE:
			return furnitureitem_getSprite(item);
		default:
			return 0;
	}
}
void item_renderIcon(Item* item, Screen* screen, int x, int y){
	switch(item->id){
		case POWERGLOVE:
			powergloveitem_renderIcon(item, screen, x, y);
			break;
		case RESOURCE:
			resourceitem_renderIcon(item, screen, x, y);
			break;
		case TOOL:
			toolitem_renderIcon(item, screen, x, y);
			break;
		case FURNITURE:
			furnitureitem_renderIcon(item, screen, x, y);
			break;
		default:
			break;
	}
}

uint8_t item_interact(Item* item, struct _Player* player, Entity* entity, int attackDir){
	switch(item->id){
		case POWERGLOVE:
			return powergloveitem_interact(item, player, entity, attackDir);
		default:
			return 0;
	}
}

uint8_t item_interactOn(Item* item, TileID tile, Level* level, int xt, int yt, struct _Player* player, int attackDir){
	switch(item->id){
		case FURNITURE:
			return furnitureitem_interactOn(item, tile, level, xt, yt, player, attackDir);
		case RESOURCE:
			return resourceitem_interactOn(item, tile, level, xt, yt, player, attackDir);
		default:
			return 0;
	}
}

void item_renderInventory(Item* item, Screen* screen, int x, int y){
	switch(item->id){
		case POWERGLOVE:
			powergloveitem_renderInventory(item, screen, x, y);
			break;
		case RESOURCE:
			resourceitem_renderInventory(item, screen, x, y);
			break;
		case TOOL:
			toolitem_renderInventory(item, screen, x, y);
			break;
		case FURNITURE:
			furnitureitem_renderInventory(item, screen, x, y);
			break;
		default:
			break;
	}
}

uint8_t item_isDepleted(Item* item){
	switch(item->id){
		case RESOURCE:
			return item->add.resource.count <= 0;
		case FURNITURE:
			return item->add.furniture.placed;
		default:
			return 0;
	}
}
int item_getAttackDamageBonus(Item* item, Entity* entity){
	if(item->id == TOOL){
		return toolitem_getAttackDamageBonus(item, entity);
	}
	return 0;
}
void item_onTake(Item* item, struct _ItemEntity* itemEntity){
	//does nothing even in overrides
}

uint8_t item_canAttack(Item* item){
	switch(item->id){
		case TOOL:
			return 1;
		case FURNITURE:
			return 0;
		default:
			return 0;
	}
}

void item_getName(Item* item, char* buf){
	char* name;
	switch(item->id){
		case FURNITURE:
			name = item->add.furniture.furniture->name;
			strcpy(buf, name);
			break;
		case POWERGLOVE:
			strcpy(buf, powergloveitem_getName(item));
			break;
		case RESOURCE:
			name = item->add.resource.resource->name;
			strcpy(buf, name);
			break;
		case TOOL:
			toolitem_getName(item, buf);
			break;
	}
}

uint8_t matches(Item* item, Item* item2);

uint8_t item_matches(Item* item, Item* item2) {
	//XXX vanilla bug: comparing ids is not enough
	if(item->id == TOOL) return toolitem_matches(item, item2);
	return item->id == item2->id;
}

void item_free(Item* item) {
	switch(item->id){
		case FURNITURE:
			if(item->add.furniture.furniture) {
				call_entity_free((Entity *) item->add.furniture.furniture);
				free(item->add.furniture.furniture);
			}
			break;
		case ITEM:
		case RESOURCE:
		case TOOL:
			break;
	}
}
