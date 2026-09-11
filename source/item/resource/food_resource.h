/*
 * food_resource.h - Edible resource extra data (Java: FoodResource).
 */
#ifndef ITEM_RESOURCE_FOOD_RESOURCE_H_
#define ITEM_RESOURCE_FOOD_RESOURCE_H_

struct _Resource;

/* Extra per-resource state for food. */
typedef struct{
	int heal;         /* Health restored per bite. */
	int staminaCost;  /* Stamina spent per bite. */
} res_food;

/* Initializes a resource as food with heal/stamina values. */
void init_food_resource(struct _Resource* resource, char* name, int sprite, int color, int heal, int staminaCost);

#endif /* ITEM_RESOURCE_FOOD_RESOURCE_H_ */
