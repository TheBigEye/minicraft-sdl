/*
 * resource.h - The materials of the game
 *              (Java: com.mojang.ld22.item.resource.Resource).
 *
 * Resources are the materials (wood, stone, ores, food, ...), defined as
 * singletons and initialized once at startup. Edible and plantable
 * resources carry extra state in add.*, standing in for the FoodResource
 * and PlantableResource subclasses.
 */
#ifndef ITEM_RESOURCE_RESOURCE_H_
#define ITEM_RESOURCE_RESOURCE_H_

#include "../../utils/javalang.h"
#include "../../level/tile/tileids.h"
#include "food_resource.h"
#include "plantable_resource.h"

struct Level;
struct Player;

typedef struct Resource Resource;

struct Resource {
    /*
     * Fixed buffer, not a String: the singletons are static and their
     * names are set once. Java: `public final String name`, and the
     * constructor rejects anything longer than six characters.
     */
    char name[7];
    /* Item-row sprite index. Java: `public final int sprite` */
    int sprite;
    /* Sprite palette. Java: `public final int color` */
    int color;
    union {
        /* Healing and stamina values for edibles. Java: FoodResource */
        res_food food;
        /* Planting rules for plantables. Java: PlantableResource */
        res_plantable plantable;
    } add;
};

/* The singletons, one per material. Java: `public static Resource wood`, ... */
extern Resource wood, stone, flower, acorn, dirt, sand, cactusFlower, seeds, wheat, bread, apple;
extern Resource coal, ironOre, goldOre, ironIngot, goldIngot;
extern Resource slime, glass, cloth, cloud, gem;

/* Constructor: Java: Resource(String name, int sprite, int color) */
PUBLIC void init_resource(Resource* this, const char* name, int sprite, int color);

/* Initializes every resource singleton; call once at startup. */
PUBLIC void init_resources(void);

/*
 * Uses the resource on a tile: planting or eating, depending on the kind.
 * Java: Resource.interactOn(Tile, Level, int, int, Player, int)
 */
PUBLIC boolean resource_interact_on(Resource* this, TileID tile, struct Level* level, int xt, int yt, struct Player* player, int attackDir);

#endif /* ITEM_RESOURCE_RESOURCE_H_ */
