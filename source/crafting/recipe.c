/*
 * recipe.c - The crafting recipes
 *            (Java: com.mojang.ld22.crafting.Recipe).
 */
#include "recipe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../entity/entityid.h"
#include "../entity/furniture.h"
#include "../entity/inventory.h"
#include "../entity/player.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../gfx/screen.h"
#include "../item/furniture_item.h"
#include "../item/resourceitem.h"
#include "../item/tool_item.h"
#include "../log.h"


/* Constructor: copies the result template and starts the cost list.
 * Java: Recipe(Item resultTemplate) */
PUBLIC void recipe_create(Recipe* this, Item* result) {
    this->resultTemplate = *result;
    arraylist_create(&this->costs);
    this->canCraft = false;
}


/*
 * Constructor for furniture results (Java: FurnitureRecipe).
 *
 * A throwaway furniture entity provides the icon and the name template; the
 * real one is instantiated at craft time.
 */
PUBLIC void furniturerecipe_create(Recipe* this, EntityId furniture) {
    Item item;
    Furniture* furn = entity_create_furniture(furniture);

    if (furn == null) {
        LOG_ERROR("tried creating unknown furniture typed entity(%d)", furniture);
    }

    furnitureitem_create(&item, furn);
    recipe_create(this, &item);

    this->id = rip_FURNITURE;
    this->add.furniture.type = furniture;
}


/* Constructor for resource results (Java: ResourceRecipe): builds a recipe
 * producing one unit of a resource. */
PUBLIC void resourcerecipe_create(Recipe* this, struct Resource* resource) {
    Item item;

    resourceitem_create(&item, resource, 1);
    recipe_create(this, &item);

    this->add.resource.resource = resource;
    this->id = rip_RESOURCE;
}


/* Constructor for tool results (Java: ToolRecipe): builds a recipe
 * producing a tool of the given type and tier. */
PUBLIC void toolrecipe_create(Recipe* this, ToolType type, int level) {
    Item item;

    toolitem_create(&item, type, level);
    recipe_create(this, &item);

    this->add.tool.type = type;
    this->add.tool.level = level;
    this->id = rip_TOOL;
}


/*
 * Java: Recipe addCost(Resource resource, int count).
 *
 * Appends a heap-allocated resource cost item to the cost list and hands
 * the recipe back, so that calls can be chained as they are in Java.
 */
PUBLIC Recipe* recipe_add_cost(Recipe* this, struct Resource* resource, int count) {
    Item* item = new(Item);

    resourceitem_create(item, resource, count);
    this->costs.add(&this->costs, item);

    return this;
}


/*
 * Java: Recipe.checkCanCraft(Player).
 *
 * Sets canCraft only when the inventory holds every cost in full.
 */
PUBLIC void recipe_check_can_craft(Recipe* this, struct Player* player) {
    for (int i = 0; i < this->costs.size; ++i) {
        Item* item = this->costs.elements[i];

        if (item->id == RESOURCE) {
            if (!inventory_has_resources(&player->inventory, item->add.resource.resource, item->add.resource.count)) {
                this->canCraft = false;
                return;
            }
        }
    }

    this->canCraft = true;
}


/*
 * Java: Recipe.renderInventory(Screen, int, int).
 *
 * Draws icon and name; the name is dimmed while canCraft is false.
 */
PUBLIC void recipe_render_inventory(Recipe* this, struct Screen* screen, int x, int y) {
    int sprite = item_get_sprite(&this->resultTemplate);
    int color = item_get_color(&this->resultTemplate);

    screen->render(screen, x, y, sprite, color, 0);

    char buffer[64];

    item_get_name(&this->resultTemplate, buffer);

    int textColor = this->canCraft ? get_color4(-1, 555, 555, 555) : get_color4(-1, 222, 222, 222);

    font_draw(buffer, strlen(buffer), screen, x + 8, y, textColor);
}


/*
 * Java: Recipe.craft(Player), which the original declares abstract and the
 * three subclasses implement. The port switches on the id instead.
 *
 * Furniture recipes instantiate a fresh entity at craft time; tools and
 * resources reuse the template item.
 */
PUBLIC void recipe_craft(Recipe* this, struct Player* player) {
    Item item;
    Furniture* furniture;

    switch (this->id) {
        case rip_FURNITURE:
            furniture = entity_create_furniture(this->add.furniture.type);

            if (furniture == null) {
                LOG_ERROR("tried creating unknown furniture typed entity(%d)", this->add.furniture.type);
            }

            furnitureitem_create(&item, furniture);
            inventory_add(&player->inventory, 0, &item);
            return;

        case rip_TOOL:
            inventory_add(&player->inventory, 0, &this->resultTemplate);
            return;

        case rip_RESOURCE:
            inventory_add(&player->inventory, 0, &this->resultTemplate);
            return;

        default:
            break;
    }

    LOG_ERROR("tried calling recipe_craft (%d)!", this->id);
}


/* Java: Recipe.deductCost(Player). Removes every resource cost from the
 * player's inventory. */
PUBLIC void recipe_deduct_cost(Recipe* this, struct Player* player) {
    for (int i = 0; i < this->costs.size; ++i) {
        Item* item = this->costs.elements[i];

        if (item->id == RESOURCE) {
            inventory_remove_resource(&player->inventory, item->add.resource.resource, item->add.resource.count);
        }
    }
}


/* C-only: releases the result template and the cost items. */
PUBLIC void recipe_free(Recipe* this) {
    item_free(&this->resultTemplate);
    this->costs.free_each(&this->costs);
}
