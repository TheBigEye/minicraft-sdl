#ifndef RECIPE_H
#define RECIPE_H

#include <string.h>

#include "../item/item.h"
#include "../utils/arraylist.h"
#include "recipeids.h"

struct _Resource;
struct _Player;
struct _Screen;

typedef struct _Recipe{
	RecipeID id;
	ArrayList costs;
	Item resultTemplate;
	char canCraft;
	union{
		struct{
			EntityId type;
		} furniture;
		struct{
			struct _Resource* resource;
		} resource;
		struct{
			ToolType type;
			int level;
		} tool;
	} add;
} Recipe;

void recipe_create(Recipe* recipe, Item* result);
void furniturerecipe_create(Recipe* recipe, EntityId furniture);
void resourcerecipe_create(Recipe* recipe, struct _Resource* resource);
void toolrecipe_create(Recipe* recipe, ToolType type, int level);

void recipe_addCost(Recipe* recipe, struct _Resource* resource, int count);
void recipe_checkCanCraft(Recipe* recipe, struct _Player* player);
void recipe_renderInventory(Recipe* recipe, struct _Screen* screen, int x, int y);
void recipe_craft(Recipe* recipe, struct _Player* player);
void recipe_deductCost(Recipe* recipe, struct _Player* player);
void recipe_free(Recipe* recipe);

#endif // RECIPE_H
