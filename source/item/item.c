/*
 * item.c - Item base dispatch (Java: Item).
 *
 * Each operation switches on the item id and forwards to the
 * matching type module (resource, tool, furniture or power glove).
 */
#include "item.h"
#include <stdlib.h>
#include "powergloveitem.h"
#include "resourceitem.h"
#include "furniture_item.h"
#include "../entity/furniture.h"
#include "tool_item.h"
#include <string.h>

/* Returns the item's sprite color for the active type. */
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
/* Returns the item's sprite index for the active type. */
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
/* Draws the small icon used in HUD/crafting lists. */
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

/* Interaction aimed at an entity (only the power glove uses it). */
uint8_t item_interact(Item* item, struct _Player* player, Entity* entity, int attackDir){
	switch(item->id){
		case POWERGLOVE:
			return powergloveitem_interact(item, player, entity, attackDir);
		default:
			return 0;
	}
}

/* Interaction aimed at a tile (furniture placement, planting...). */
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

/* Draws the icon plus label row shown in the inventory menu. */
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

/* Resources deplete at zero count; furniture once placed. */
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
/* Only tools add attack damage bonuses. */
int item_getAttackDamageBonus(Item* item, Entity* entity){
	if(item->id == TOOL){
		return toolitem_getAttackDamageBonus(item, entity);
	}
	return 0;
}
void item_onTake(Item* item, struct _ItemEntity* itemEntity){
	//does nothing even in overrides
}

/* Only tools can attack entities. */
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

/* Copies the display name for the active type into buf. */
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

/* Tools compare type and level through their own matcher; other
 * items stack whenever their ids match. */
uint8_t item_matches(Item* item, Item* item2) {
	//XXX vanilla bug: comparing ids is not enough
	if(item->id == TOOL) return toolitem_matches(item, item2);
	return item->id == item2->id;
}

/* Releases type-specific storage: a furniture item still carrying
 * its entity frees it; placed furniture and other types hold none. */
void item_free(Item* item) {
	switch(item->id){
		case FURNITURE:
			if(item->add.furniture.furniture) {
				Entity* furn = (Entity *) item->add.furniture.furniture;
				furn->vt->free(furn);
				free(item->add.furniture.furniture);
			}
			break;
		case ITEM:
		case RESOURCE:
		case TOOL:
			break;
	}
}
