/*
 * food_resource.h - The extra data of an edible resource
 *                   (Java: com.mojang.ld22.item.resource.FoodResource).
 */
#ifndef ITEM_RESOURCE_FOOD_RESOURCE_H_
#define ITEM_RESOURCE_FOOD_RESOURCE_H_

#include "../../utils/javalang.h"

struct Resource;

typedef struct res_food res_food;

struct res_food {
    /* Health restored per bite. Java: `private int heal` */
    int heal;
    /* Stamina spent per bite. Java: `private int staminaCost` */
    int staminaCost;
};

/*
 * Constructor: fills the Resource fields, then the food-specific values.
 * Java: FoodResource(String, int, int, int heal, int staminaCost)
 */
PUBLIC void init_food_resource(struct Resource* this, const char* name, int sprite, int color, int heal, int staminaCost);

#endif /* ITEM_RESOURCE_FOOD_RESOURCE_H_ */
