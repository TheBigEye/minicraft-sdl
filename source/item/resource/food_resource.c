#include "resource.h"

void init_food_resource(Resource* resource, char* name, int sprite, int color, int heal, int staminaCost){
	init_resource(resource, name, sprite, color);
	resource->add.food.heal = heal;
	resource->add.food.staminaCost = staminaCost;
}
