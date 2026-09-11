/*
 * resource.h - Resource definitions (Java: Resource).
 *
 * Resources are the game's materials (wood, stone, ores, food...),
 * defined as singletons and initialized once at startup. Plantable
 * and edible resources carry extra state in add.*.
 */
#ifndef ITEM_RESOURCE_RESOURCE_H_
#define ITEM_RESOURCE_RESOURCE_H_

#include "food_resource.h"
#include "plantable_resource.h"

struct _Player;

typedef struct _Resource{
	char name[7]; //max name len = 6 + 1 \x00
	int sprite;   /* Item-row sprite index. */
	int color;    /* Sprite palette. */
	union{
		res_food food;            /* Healing/stamina values for edibles. */
		res_plantable plantable;  /* Planting rules for plantables. */
	} add;
} Resource;

extern Resource wood, stone, flower, acorn, dirt, sand, cactusFlower, seeds, wheat, bread, apple;
extern Resource coal, ironOre, goldOre, ironIngot, goldIngot;
extern Resource slime, glass, cloth, cloud, gem;


/* Initializes every resource singleton; call once at startup. */
void init_resources();
void init_resource(Resource* resource, char* name, int sprite, int color);
/* Uses the resource on a tile: planting or eating, per kind. */
char resource_interactOn(Resource* resource, TileID tile, Level* level, int xt, int yt, struct _Player* player, int attackDir);

#endif /* ITEM_RESOURCE_RESOURCE_H_ */
