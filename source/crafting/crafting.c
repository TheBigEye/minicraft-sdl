/*
 * crafting.c - The recipe tables
 *              (Java: com.mojang.ld22.crafting.Crafting).
 *
 * Registers every vanilla recipe: furniture and wood/rock tools at the
 * workbench, iron/gold/gem tools at the anvil, smelting at the furnace and
 * bread at the oven.
 */
#include "crafting.h"

#include <stdlib.h>

#include "../utils/javalang.h"
#include "../utils/arraylist.h"
#include "recipe.h"

/* Java: `public static final List<Recipe> anvilRecipes`, and so on. */
ArrayList anvilRecipes = {0};
ArrayList ovenRecipes = {0};
ArrayList furnaceRecipes = {0};
ArrayList workbenchRecipes = {0};


/* Creates the four lists and fills them with every vanilla recipe.
 * Java: the static initializer of class Crafting. */
PUBLIC void crafting_init(void) {
    arraylist_create(&anvilRecipes);
    arraylist_create(&ovenRecipes);
    arraylist_create(&furnaceRecipes);
    arraylist_create(&workbenchRecipes);

    /* --- furniture --- */

    Recipe* recipe = new(Recipe);

    furniturerecipe_create(recipe, LANTERN);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &slime, 10);
    recipe_add_cost(recipe, &glass, 4);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    furniturerecipe_create(recipe, OVEN);
    recipe_add_cost(recipe, &stone, 15);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    furniturerecipe_create(recipe, FURNACE);
    recipe_add_cost(recipe, &stone, 20);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    furniturerecipe_create(recipe, WORKBENCH);
    recipe_add_cost(recipe, &wood, 20);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    furniturerecipe_create(recipe, CHEST);
    recipe_add_cost(recipe, &wood, 20);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    furniturerecipe_create(recipe, ANVIL);
    recipe_add_cost(recipe, &ironIngot, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    /* --- wood tools (tier 0) --- */

    recipe = new(Recipe);
    toolrecipe_create(recipe, SWORD, 0);
    recipe_add_cost(recipe, &wood, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, AXE, 0);
    recipe_add_cost(recipe, &wood, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, HOE, 0);
    recipe_add_cost(recipe, &wood, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, PICKAXE, 0);
    recipe_add_cost(recipe, &wood, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, SHOVEL, 0);
    recipe_add_cost(recipe, &wood, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    /* --- rock tools (tier 1) --- */

    recipe = new(Recipe);
    toolrecipe_create(recipe, SWORD, 1);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &stone, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, AXE, 1);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &stone, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, HOE, 1);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &stone, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, PICKAXE, 1);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &stone, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, SHOVEL, 1);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &stone, 5);
    workbenchRecipes.add(&workbenchRecipes, recipe);

    /* --- iron tools (tier 2) --- */

    recipe = new(Recipe);
    toolrecipe_create(recipe, SWORD, 2);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &ironIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, AXE, 2);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &ironIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, HOE, 2);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &ironIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, PICKAXE, 2);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &ironIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, SHOVEL, 2);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &ironIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    /* --- gold tools (tier 3) --- */

    recipe = new(Recipe);
    toolrecipe_create(recipe, SWORD, 3);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &goldIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, AXE, 3);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &goldIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, HOE, 3);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &goldIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, PICKAXE, 3);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &goldIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, SHOVEL, 3);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &goldIngot, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    /* --- gem tools (tier 4) --- */

    recipe = new(Recipe);
    toolrecipe_create(recipe, SWORD, 4);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &gem, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, AXE, 4);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &gem, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, HOE, 4);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &gem, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, PICKAXE, 4);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &gem, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    recipe = new(Recipe);
    toolrecipe_create(recipe, SHOVEL, 4);
    recipe_add_cost(recipe, &wood, 5);
    recipe_add_cost(recipe, &gem, 5);
    anvilRecipes.add(&anvilRecipes, recipe);

    /* --- furnace --- */

    recipe = new(Recipe);
    resourcerecipe_create(recipe, &ironIngot);
    recipe_add_cost(recipe, &ironOre, 4);
    recipe_add_cost(recipe, &coal, 1);
    furnaceRecipes.add(&furnaceRecipes, recipe);

    recipe = new(Recipe);
    resourcerecipe_create(recipe, &goldIngot);
    recipe_add_cost(recipe, &goldOre, 4);
    recipe_add_cost(recipe, &coal, 1);
    furnaceRecipes.add(&furnaceRecipes, recipe);

    recipe = new(Recipe);
    resourcerecipe_create(recipe, &glass);
    recipe_add_cost(recipe, &sand, 4);
    recipe_add_cost(recipe, &coal, 1);
    furnaceRecipes.add(&furnaceRecipes, recipe);

    /* --- oven --- */

    recipe = new(Recipe);
    resourcerecipe_create(recipe, &bread);
    recipe_add_cost(recipe, &wheat, 4);
    ovenRecipes.add(&ovenRecipes, recipe);
}


/* Frees every recipe, and its cost items, in all four lists. */
PUBLIC void crafting_free(void) {
    for (int i = 0; i < anvilRecipes.size; ++i) {
        recipe_free(anvilRecipes.elements[i]);
        delete(anvilRecipes.elements[i]);
    }
    anvilRecipes.free(&anvilRecipes);

    for (int i = 0; i < ovenRecipes.size; ++i) {
        recipe_free(ovenRecipes.elements[i]);
        delete(ovenRecipes.elements[i]);
    }
    ovenRecipes.free(&ovenRecipes);

    for (int i = 0; i < furnaceRecipes.size; ++i) {
        recipe_free(furnaceRecipes.elements[i]);
        delete(furnaceRecipes.elements[i]);
    }
    furnaceRecipes.free(&furnaceRecipes);

    for (int i = 0; i < workbenchRecipes.size; ++i) {
        recipe_free(workbenchRecipes.elements[i]);
        delete(workbenchRecipes.elements[i]);
    }
    workbenchRecipes.free(&workbenchRecipes);
}
