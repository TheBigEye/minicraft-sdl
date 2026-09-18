/*
 * food_resource.c - The edible resource initializer
 *                   (Java: com.mojang.ld22.item.resource.FoodResource).
 */
#include "food_resource.h"

#include "resource.h"


/* Constructor: fills the common fields, then the food-specific heal and
 * stamina data. Java: FoodResource(String, int, int, int, int) */
PUBLIC void init_food_resource(struct Resource* this, const char* name, int sprite, int color, int heal, int staminaCost) {
    init_resource(this, name, sprite, color);   /* super(name, sprite, color) */

    this->add.food.heal = heal;
    this->add.food.staminaCost = staminaCost;
}
