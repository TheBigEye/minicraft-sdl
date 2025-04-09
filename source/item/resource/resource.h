#ifndef ITEM_RESOURCE_RESOURCE_H_
#define ITEM_RESOURCE_RESOURCE_H_

#include "food_resource.h"
#include "plantable_resource.h"

struct _Player;

typedef struct _Resource{
	char name[7]; //max name len = 6 + 1 \x00
	int sprite;
	int color;
	union{
		res_food food;
		res_plantable plantable;
	} add;
} Resource;

extern Resource wood, stone, flower, acorn, dirt, sand, cactusFlower, seeds, wheat, bread, apple;
extern Resource coal, ironOre, goldOre, ironIngot, goldIngot;
extern Resource slime, glass, cloth, cloud, gem;


void init_resources();
void init_resource(Resource* resource, char* name, int sprite, int color);
char resource_interactOn(Resource* resource, TileID tile, Level* level, int xt, int yt, struct _Player* player, int attackDir);

#endif /* ITEM_RESOURCE_RESOURCE_H_ */
