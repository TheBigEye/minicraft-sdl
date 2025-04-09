#include "recipe.h"
#include "../entity/entityid.h"
#include "../gfx/screen.h"
#include "../entity/player.h"
#include "../item/resourceitem.h"
#include "../item/furniture_item.h"
#include "../entity/furniture.h"
#include "../entity/inventory.h"
#include "../gfx/font.h"
#include "../entity/_entity_caller.h"
#include "../item/tool_item.h"
#include "../item/resourceitem.h"
#include "../gfx/color.h"

void recipe_create(Recipe* recipe, Item* result){
	recipe->resultTemplate = *result;
	create_arraylist(&recipe->costs);
	recipe->canCraft = 0;
}

void furniturerecipe_create(Recipe* recipe, EntityId furniture){
	Item it;
	Furniture* furn = entity_createFurniture(furniture);
	if(furn == 0){
		printf("Tried creating unknown furniture typed entity(%d)\n", furniture);
	}
	furnitureitem_create(&it, furn);
	recipe_create(recipe, &it);
	recipe->id = rip_FURNITURE;
	recipe->add.furniture.type = furniture;
}

void resourcerecipe_create(Recipe* recipe, Resource* resource){
	Item it;
	resourceitem_create_cnt(&it, resource, 1);
	recipe_create(recipe, &it);
	recipe->add.resource.resource = resource;
	recipe->id = rip_RESOURCE;
}

void toolrecipe_create(Recipe* recipe, ToolType type, int level){
	Item it;
	toolitem_create(&it, type, level);
	recipe_create(recipe, &it);
	recipe->add.tool.type = type;
	recipe->add.tool.level = level;
	recipe->id = rip_TOOL;
}

void recipe_addCost(Recipe* recipe, Resource* resource, int count){
	Item* it = malloc(sizeof(Item));
	resourceitem_create_cnt(it, resource, count);
	arraylist_push(&recipe->costs, it);
}

void recipe_checkCanCraft(Recipe* recipe, Player* player){
	for(int i = 0; i < recipe->costs.size; ++i){
		Item* item = recipe->costs.elements[i];
		if(item->id == RESOURCE){
			char has = inventory_hasResources(&player->inventory, item->add.resource.resource, item->add.resource.count);
			if(!has){
				recipe->canCraft = 0;
				return;
			}
		}
	}
	recipe->canCraft = 1;
}

void recipe_renderInventory(Recipe* recipe, Screen* screen, int x, int y){
	int sprite = item_getSprite(&recipe->resultTemplate);
	int color = item_getColor(&recipe->resultTemplate);
	render_screen(screen, x, y, sprite, color, 0);
	int texColor = recipe->canCraft ? getColor4(-1, 555, 555, 555) : getColor4(-1, 222, 222, 222);
	char buf[64];
	item_getName(&recipe->resultTemplate, buf);
	font_draw(buf, strlen(buf), screen, x+8, y, texColor);
}
void recipe_craft(Recipe* recipe, Player* player){
	Item it;
	Furniture* furn;
	switch(recipe->id){
		case rip_FURNITURE:
			furn = entity_createFurniture(recipe->add.furniture.type);
			if(furn == 0){
				printf("Tried creating unknown furniture typed entity(%d)\n", recipe->add.furniture.type);
			}
			furnitureitem_create(&it, furn);
			inventory_addItemIntoSlot(&player->inventory, 0, &it);
			return;
		case rip_TOOL:
			inventory_addItemIntoSlot(&player->inventory, 0, &recipe->resultTemplate);
			return;
		case rip_RESOURCE:
			inventory_addItemIntoSlot(&player->inventory, 0, &recipe->resultTemplate);
			return;
	}

	printf("Tried calling recipe_craft (%d)!\n", recipe->id);
}

void recipe_deductCost(Recipe* recipe, Player* player){
	for(int i = 0; i < recipe->costs.size; ++i){
		Item* item = recipe->costs.elements[i];
		if(item->id == RESOURCE){
			inventory_removeResource(&player->inventory, item->add.resource.resource, item->add.resource.count);
		}
	}
}

void recipe_free(Recipe* recipe){
	item_free(&recipe->resultTemplate);
	arraylist_remove_and_dealloc_each(&recipe->costs);
	//TODO
}
