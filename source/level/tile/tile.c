/*
 * tile.c - The base Tile class and the registry of the 23 tile types
 *          (Java: com.mojang.ld22.level.tile.Tile).
 *
 * `tile_storage` holds the instances, which in Java each came out of a
 * `new`, and the static `tiles` array indexes them by id: the constructor
 * tile_init() registers the object in tiles[id], exactly as `tiles[id] =
 * this` did in Java's Tile.
 *
 * The methods here are the body of the base class: the behaviour any tile
 * that does not override them inherits.
 */
#include "tile.h"

#include <string.h>

#include "cactus_tile.h"
#include "cloud_cactus_tile.h"
#include "cloud_tile.h"
#include "dirt_tile.h"
#include "farmland.h"
#include "flower_tile.h"
#include "grass_tile.h"
#include "hard_rock_tile.h"
#include "hole_tile.h"
#include "infinite_fall_tile.h"
#include "lava_tile.h"
#include "ore_tile.h"
#include "rock_tile.h"
#include "sand_tile.h"
#include "sapling_tile.h"
#include "stairs_tile.h"
#include "tree_tile.h"
#include "water_tile.h"
#include "wheat_tile.h"

#include "../../item/resource/resource.h"
#include "../../log.h"
#include "../../utils/utils.h"

/* Global counter pacing the occasional ticks. Java: the static Tile.tickCount */
int tile_tick_count = 0;

/*
 * Storage for the Tile instances. There are at most 256 of them, one per
 * id, which is all a level's map byte can hold.
 */
STATIC Tile tile_storage[256];

/* The static registry, indexed by id. Java: public static Tile[] tiles */
Tile* tiles[256];


/* ------------------------------------------------------------------ */
/* Body of the base Tile class                                        */
/* ------------------------------------------------------------------ */

/* Base render(): a bare tile draws nothing. */
PUBLIC void tile_render(Tile* this, Screen* screen, struct Level* level, int x, int y) {
    (void) this;
    (void) screen;
    (void) level;
    (void) x;
    (void) y;
}


/* Base mayPass(): by default you can cross it. */
PUBLIC boolean tile_may_pass(Tile* this, struct Level* level, int x, int y, struct Entity* e) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;
    (void) e;

    return true;
}


/* Base getLightRadius(): by default it gives off no light. */
PUBLIC int tile_get_light_radius(Tile* this, struct Level* level, int x, int y) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;

    return 0;
}


/* Base hurt(): by default damage does not affect it. */
PUBLIC void tile_hurt(Tile* this, struct Level* level, int x, int y, struct Mob* source, int dmg, int attackDir) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;
    (void) source;
    (void) dmg;
    (void) attackDir;
}


/* Base bumpedInto(): by default nothing happens on contact. */
PUBLIC void tile_bumped_into(Tile* this, struct Level* level, int xt, int yt, struct Entity* entity) {
    (void) this;
    (void) level;
    (void) xt;
    (void) yt;
    (void) entity;
}


/* Base tick(): by default there is no occasional update. */
PUBLIC void tile_tick(Tile* this, struct Level* level, int xt, int yt) {
    (void) this;
    (void) level;
    (void) xt;
    (void) yt;
}


/* Base steppedOn(): by default stepping on it does nothing. */
PUBLIC void tile_stepped_on(Tile* this, struct Level* level, int xt, int yt, struct Entity* entity) {
    (void) this;
    (void) level;
    (void) xt;
    (void) yt;
    (void) entity;
}


/* Base interact(): by default no item interacts with the tile. */
PUBLIC boolean tile_interact(Tile* this, struct Level* level, int xt, int yt, struct Player* player, struct Item* item, int attackDir) {
    (void) this;
    (void) level;
    (void) xt;
    (void) yt;
    (void) player;
    (void) item;
    (void) attackDir;

    return false;
}


/* Base use(): by default the use key does nothing. */
PUBLIC boolean tile_use(Tile* this, struct Level* level, int xt, int yt, struct Player* player, int attackDir) {
    (void) this;
    (void) level;
    (void) xt;
    (void) yt;
    (void) player;
    (void) attackDir;

    return false;
}


/* A liquid, as far as edge drawing is concerned? Java: connectsToLiquid() */
PUBLIC boolean tile_connects_to_liquid(Tile* this) {
    return this->connects_to_water || this->connects_to_lava;
}


/* ------------------------------------------------------------------ */
/* Constructor (Java: Tile(int id))                                    */
/* ------------------------------------------------------------------ */

/*
 * Installs the base class methods, registers the tile in tiles[id] and
 * zeroes its data. Subclasses call this first, their `super(id)`, and then
 * override whatever they need.
 */
PUBLIC void tile_init(Tile* this, TileID id) {
    this->render             = tile_render;
    this->may_pass           = tile_may_pass;
    this->get_light_radius   = tile_get_light_radius;
    this->hurt               = tile_hurt;
    this->bumped_into        = tile_bumped_into;
    this->tick               = tile_tick;
    this->stepped_on         = tile_stepped_on;
    this->interact           = tile_interact;
    this->use                = tile_use;
    this->connects_to_liquid = tile_connects_to_liquid;

    random_create(&this->random);
    random_set_seed(&this->random, get_time_us() / 1000);

    this->id                 = id;
    this->connects_to_grass  = false;
    this->connects_to_sand   = false;
    this->connects_to_lava   = false;
    this->connects_to_water  = false;

    memset(&this->add, 0, sizeof(this->add));

    if (tiles[id] != null) {
        LOG_WARN("Duplicate tile ids! (%d)", (int) id);
    }

    tiles[id] = this;
}


/*
 * Creates the game's 23 tiles. This is Java's set of static fields on Tile
 * (`public static Tile grass = new GrassTile(0); ...`).
 */
PUBLIC void init_tiles(void) {
    for (int i = 0; i < 256; ++i) {
        tiles[i] = null;
    }

    memset(tile_storage, 0, sizeof(tile_storage));

    grasstile_init(&tile_storage[GRASS], GRASS);
    rocktile_init(&tile_storage[ROCK], ROCK);
    watertile_init(&tile_storage[WATER], WATER);
    flowertile_init(&tile_storage[FLOWER], FLOWER);
    treetile_init(&tile_storage[TREE], TREE);
    dirttile_init(&tile_storage[DIRT], DIRT);
    sandtile_init(&tile_storage[SAND], SAND);
    cactustile_init(&tile_storage[CACTUS], CACTUS);
    holetile_init(&tile_storage[HOLE], HOLE);
    saplingtile_init(&tile_storage[TREE_SAPLING], TREE_SAPLING, GRASS, TREE);
    saplingtile_init(&tile_storage[CACTUS_SAPLING], CACTUS_SAPLING, SAND, CACTUS);
    farmlandtile_init(&tile_storage[FARMLAND], FARMLAND);
    wheat_tile_init(&tile_storage[WHEAT], WHEAT);
    lavatile_init(&tile_storage[LAVA], LAVA);
    stairstile_init(&tile_storage[STAIRS_DOWN], STAIRS_DOWN, 0);
    stairstile_init(&tile_storage[STAIRS_UP], STAIRS_UP, 1);
    infinitefalltile_init(&tile_storage[INFINITE_FALL], INFINITE_FALL);
    cloudtile_init(&tile_storage[CLOUD], CLOUD);
    hardrocktile_init(&tile_storage[HARD_ROCK], HARD_ROCK);
    oretile_init(&tile_storage[IRON_ORE], IRON_ORE, &ironOre);
    oretile_init(&tile_storage[GOLD_ORE], GOLD_ORE, &goldOre);
    oretile_init(&tile_storage[GEM_ORE], GEM_ORE, &gem);
    cloudcactustile_init(&tile_storage[CLOUD_CACTUS], CLOUD_CACTUS);
}
