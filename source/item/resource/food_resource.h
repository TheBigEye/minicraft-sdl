#ifndef ITEM_RESOURCE_FOOD_RESOURCE_H_
#define ITEM_RESOURCE_FOOD_RESOURCE_H_

struct _Resource;

typedef struct{
	int heal;
	int staminaCost;
} res_food;

void init_food_resource(struct _Resource* resource, char* name, int sprite, int color, int heal, int staminaCost);

#endif /* ITEM_RESOURCE_FOOD_RESOURCE_H_ */
