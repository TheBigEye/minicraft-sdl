/*
 * resource.c - The resource singletons and how they are used
 *              (Java: Resource, FoodResource and PlantableResource).
 */
#include "resource.h"

#include <stdio.h>
#include <string.h>

#include "../../entity/player.h"
#include "../../gfx/color.h"
#include "../../log.h"
#include "food_resource.h"
#include "plantable_resource.h"


Resource wood;
Resource stone;
Resource flower;
Resource acorn;
Resource dirt;
Resource sand;
Resource cactusFlower;
Resource seeds;
Resource wheat;
Resource bread;
Resource apple;

Resource coal;
Resource ironOre;
Resource goldOre;
Resource ironIngot;
Resource goldIngot;

Resource slime;
Resource glass;
Resource cloth;
Resource cloud;
Resource gem;


/* Tiles each plantable resource may be planted on.
 * Java: the `Tile... sourceTiles` varargs of PlantableResource. */
PRIVATE TileID flower_sources[] = {GRASS};
PRIVATE TileID acorn_sources[] = {GRASS};
PRIVATE TileID dirt_sources[] = {HOLE, WATER, LAVA};
PRIVATE TileID sand_sources[] = {GRASS, DIRT};
PRIVATE TileID cactus_sources[] = {SAND};
PRIVATE TileID seeds_sources[] = {FARMLAND};
PRIVATE TileID cloud_sources[] = {INFINITE_FALL};


/*
 * Constructor: Java: Resource(String name, int sprite, int color).
 *
 * Java throws a RuntimeException when the name is longer than six
 * characters; the port logs the same condition and truncates instead, so
 * that a bad name cannot take the game down at startup.
 */
PUBLIC void init_resource(Resource* this, const char* name, int sprite, int color) {
    size_t len = strlen(name);

    if (len > 6) {
        LOG_ERROR("resource name '%s' cannot be longer than six characters", name);
        len = 6;
    }

    memset(this->name, 0, sizeof(this->name));
    memcpy(this->name, name, len); /* The NUL padding was zeroed just above. */

    this->sprite = sprite;
    this->color = color;
}


/* Fills every resource singleton with its name, sprite, color and food or
 * plantable extras. Java: the static initializers of class Resource. */
PUBLIC void init_resources(void) {
    init_resource(&wood, "Wood", 1 + 4 * 32, get_color4(-1, 200, 531, 430));
    init_resource(&stone, "Stone", 2 + 4 * 32, get_color4(-1, 111, 333, 555));

    init_plantable_resource(&flower, "Flower", 0 + 4 * 32, get_color4(-1, 10, 444, 330), FLOWER, flower_sources, sizeof(flower_sources) / sizeof(TileID));
    init_plantable_resource(&acorn, "Acorn", 3 + 4 * 32, get_color4(-1, 100, 531, 320), TREE_SAPLING, acorn_sources, sizeof(acorn_sources) / sizeof(TileID));
    init_plantable_resource(&dirt, "Dirt", 2 + 4 * 32, get_color4(-1, 100, 322, 432), DIRT, dirt_sources, sizeof(dirt_sources) / sizeof(TileID));
    init_plantable_resource(&sand, "Sand", 2 + 4 * 32, get_color4(-1, 110, 440, 550), SAND, sand_sources, sizeof(sand_sources) / sizeof(TileID));
    init_plantable_resource(&cactusFlower, "Cactus", 4 + 4 * 32, get_color4(-1, 10, 40, 50), CACTUS_SAPLING, cactus_sources, sizeof(cactus_sources) / sizeof(TileID));
    init_plantable_resource(&seeds, "Seeds", 5 + 4 * 32, get_color4(-1, 10, 40, 50), WHEAT, seeds_sources, sizeof(seeds_sources) / sizeof(TileID));

    init_resource(&wheat, "Wheat", 6 + 4 * 32, get_color4(-1, 110, 330, 550));

    init_food_resource(&bread, "Bread", 8 + 4 * 32, get_color4(-1, 110, 330, 550), 2, 5);
    init_food_resource(&apple, "Apple", 9 + 4 * 32, get_color4(-1, 100, 300, 500), 1, 5);

    init_resource(&coal, "COAL", 10 + 4 * 32, get_color4(-1, 000, 111, 111));
    init_resource(&ironOre, "I.ORE", 10 + 4 * 32, get_color4(-1, 100, 322, 544));
    init_resource(&goldOre, "G.ORE", 10 + 4 * 32, get_color4(-1, 110, 440, 553));
    init_resource(&ironIngot, "IRON", 11 + 4 * 32, get_color4(-1, 100, 322, 544));
    init_resource(&goldIngot, "GOLD", 11 + 4 * 32, get_color4(-1, 110, 330, 553));

    init_resource(&slime, "SLIME", 10 + 4 * 32, get_color4(-1, 10, 30, 50));
    init_resource(&glass, "glass", 12 + 4 * 32, get_color4(-1, 555, 555, 555));
    init_resource(&cloth, "cloth", 1 + 4 * 32, get_color4(-1, 25, 252, 141));

    init_plantable_resource(&cloud, "cloud", 2 + 4 * 32, get_color4(-1, 222, 555, 444), CLOUD, cloud_sources, sizeof(cloud_sources) / sizeof(TileID));

    init_resource(&gem, "gem", 13 + 4 * 32, get_color4(-1, 101, 404, 545));
}


/*
 * Using a resource on a tile
 * (Java: Resource.interactOn(Tile, Level, int, int, Player, int), which the
 * FoodResource and PlantableResource subclasses override).
 *
 * Plantables replace one of their source tiles with the target tile;
 * edibles heal the player at a stamina cost. Everything else cannot be used
 * this way.
 */
PUBLIC boolean resource_interact_on(Resource* this, TileID tile, struct Level* level, int xt, int yt, struct Player* player, int attackDir) {
    (void) attackDir;

    LOG_TRACE("resource interactOn: %s on tile %d", this->name, tile);

    if (this == &cloud || this == &flower || this == &acorn || this == &dirt || this == &sand || this == &cactusFlower || this == &seeds) {
        for (int i = 0; i < this->add.plantable.sourceTilesSize; ++i) {
            if (this->add.plantable.sourceTiles[i] == tile) {
                level->set_tile(level, xt, yt, tiles[this->add.plantable.targetTile], 0);

                return true;
            }
        }

        return false;
    } else if (this == &bread || this == &apple) {
        if (player->mob.health < player->mob.maxHealth && player_pay_stamina(player, this->add.food.staminaCost)) {
            mob_heal(&player->mob, this->add.food.heal);

            return true;
        }

        return false;
    }

    return false;
}
