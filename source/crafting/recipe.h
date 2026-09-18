/*
 * recipe.h - The crafting recipes
 *             (Java: com.mojang.ld22.crafting.Recipe): a craftable result
 *             with its resource costs and its per-kind extra data.
 */
#ifndef RECIPE_H
#define RECIPE_H 1

#include <string.h>

#include "../item/item.h"
#include "../utils/javalang.h"
#include "../utils/arraylist.h"

#include "recipeids.h"

struct Resource;
struct Player;
struct Screen;

typedef struct Recipe Recipe;

struct Recipe {
    /*
     * Selects which add.* payload is active. C-only: Java gets this from
     * instanceof over the FurnitureRecipe, ResourceRecipe and ToolRecipe
     * subclasses.
     */
    RecipeID id;
    /* Item* resource costs, heap-allocated.
     * Java: `public List<Item> costs` */
    ArrayList costs;
    /* Item produced when crafted. Java: `public Item resultTemplate` */
    Item resultTemplate;
    /* Last availability check result. Java: `public boolean canCraft` */
    boolean canCraft;
    union {
        struct {
            /* Furniture kind to instantiate. Java: FurnitureRecipe */
            EntityId type;
        } furniture;
        struct {
            /* Resource produced. Java: ResourceRecipe */
            struct Resource* resource;
        } resource;
        struct {
            /* Tool kind produced. Java: ToolRecipe */
            ToolType type;
            /* Material tier produced. Java: ToolRecipe */
            int level;
        } tool;
    } add;
};

/* Constructor. Java: Recipe(Item resultTemplate) */
PUBLIC void recipe_create(Recipe* this, Item* result);

/* Constructor for furniture results. Java: FurnitureRecipe */
PUBLIC void furniturerecipe_create(Recipe* this, EntityId furniture);

/* Constructor for resource results. Java: ResourceRecipe */
PUBLIC void resourcerecipe_create(Recipe* this, struct Resource* resource);

/* Constructor for tool results. Java: ToolRecipe */
PUBLIC void toolrecipe_create(Recipe* this, ToolType type, int level);

/*
 * Adds one resource cost to the recipe and hands the recipe back, so that
 * calls can be chained as Java does.
 * Java: Recipe addCost(Resource resource, int count)
 */
PUBLIC Recipe* recipe_add_cost(Recipe* this, struct Resource* resource, int count);

/* Updates canCraft from the player's inventory contents.
 * Java: Recipe.checkCanCraft(Player) */
PUBLIC void recipe_check_can_craft(Recipe* this, struct Player* player);

/* Draws the recipe row in a crafting menu.
 * Java: Recipe.renderInventory(Screen, int, int) */
PUBLIC void recipe_render_inventory(Recipe* this, struct Screen* screen, int x, int y);

/* Produces the result into the player's inventory. Java: Recipe.craft(Player),
 * which is abstract there and dispatched on the subclass here. */
PUBLIC void recipe_craft(Recipe* this, struct Player* player);

/* Removes the recipe costs from the player's inventory.
 * Java: Recipe.deductCost(Player) */
PUBLIC void recipe_deduct_cost(Recipe* this, struct Player* player);

/* C-only: releases the cost items. */
PUBLIC void recipe_free(Recipe* this);

#endif /* RECIPE_H */
