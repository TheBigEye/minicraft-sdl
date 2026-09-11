/*
 * recipe.h - Recipe (Java: Recipe): a craftable result with its
 *            resource costs and per-kind extra data.
 */
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
	RecipeID id;          /* Selects which add.* payload is active. */
	ArrayList costs;      /* Item* resource costs, heap-allocated. */
	Item resultTemplate;  /* Item produced when crafted. */
	char canCraft;        /* Last availability check result. */
	union{
		struct{
			EntityId type; /* Furniture kind to instantiate. */
		} furniture;
		struct{
			struct _Resource* resource; /* Resource produced. */
		} resource;
		struct{
			ToolType type; /* Tool kind produced. */
			int level;     /* Material tier produced. */
		} tool;
	} add;
} Recipe;

/* Kind-specific constructors; each sets id and add.*. */
void recipe_create(Recipe* recipe, Item* result);
void furniturerecipe_create(Recipe* recipe, EntityId furniture);
void resourcerecipe_create(Recipe* recipe, struct _Resource* resource);
void toolrecipe_create(Recipe* recipe, ToolType type, int level);

/* Adds one resource cost to the recipe. */
void recipe_addCost(Recipe* recipe, struct _Resource* resource, int count);
/* Updates canCraft from the player's inventory contents. */
void recipe_checkCanCraft(Recipe* recipe, struct _Player* player);
/* Draws the recipe row in a crafting menu. */
void recipe_renderInventory(Recipe* recipe, struct _Screen* screen, int x, int y);
/* Produces the result into the player's inventory. */
void recipe_craft(Recipe* recipe, struct _Player* player);
/* Removes the recipe costs from the player's inventory. */
void recipe_deductCost(Recipe* recipe, struct _Player* player);
void recipe_free(Recipe* recipe);

#endif // RECIPE_H
