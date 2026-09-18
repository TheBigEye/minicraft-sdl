/*
 * tile.h - The Tile class (Java: com.mojang.ld22.level.tile.Tile).
 *
 * Every tile type is a Tile instance carrying its own methods, just as in
 * the original, where each one was a subclass (`class RockTile extends
 * Tile`). The base constructor, tile_init(), installs the default
 * implementations and registers the tile in the static `tiles` array, the
 * way Tile(int id) does in Java; each subclass calls tile_init() and then
 * overrides the methods it needs.
 *
 * The level stores ids (one byte per square) and Level.get_tile() hands
 * back the object, exactly as Level.getTile() does in Java:
 *
 *     Tile* t = level->get_tile(level, x, y);
 *     t->stepped_on(t, level, x, y, entity);
 */
#ifndef LEVEL_TILE_TILE_H_
#define LEVEL_TILE_TILE_H_ 1

#include "tileids.h"

#include "../../gfx/screen.h"
#include "../../utils/javarandom.h"
#include "../../utils/javalang.h"

/* Forward declarations: Tile is referenced from all over the tree. */
struct Level;
struct Entity;
struct Mob;
struct Player;
struct Item;

typedef struct Tile Tile;

/* Signatures of the Tile methods. */
typedef void    (*tile_render_fn)            (Tile* this, Screen* screen, struct Level* level, int x, int y);
typedef boolean (*tile_may_pass_fn)          (Tile* this, struct Level* level, int x, int y, struct Entity* e);
typedef int     (*tile_get_light_radius_fn)  (Tile* this, struct Level* level, int x, int y);
typedef void    (*tile_hurt_fn)              (Tile* this, struct Level* level, int x, int y, struct Mob* source, int dmg, int attackDir);
typedef void    (*tile_bumped_into_fn)       (Tile* this, struct Level* level, int xt, int yt, struct Entity* entity);
typedef void    (*tile_tick_fn)              (Tile* this, struct Level* level, int xt, int yt);
typedef void    (*tile_stepped_on_fn)        (Tile* this, struct Level* level, int xt, int yt, struct Entity* entity);
typedef boolean (*tile_interact_fn)          (Tile* this, struct Level* level, int xt, int yt, struct Player* player, struct Item* item, int attackDir);
typedef boolean (*tile_use_fn)               (Tile* this, struct Level* level, int xt, int yt, struct Player* player, int attackDir);
typedef boolean (*tile_connects_to_liquid_fn)(Tile* this);

struct Tile {
    /* --- methods: installed by tile_init(), overridden by the subclasses --- */

    /* Draws the square. Java: render(Screen, Level, int, int) */
    tile_render_fn render;
    /* Can it be walked or swum onto? Java: mayPass(Level, int, int, Entity) */
    tile_may_pass_fn may_pass;
    /* Light it gives off; lava glows. Java: getLightRadius(Level, int, int) */
    tile_get_light_radius_fn get_light_radius;
    /* Mining or hitting damage. Java: hurt(Level, int, int, Mob, int, int) */
    tile_hurt_fn hurt;
    /* Something walked into the square. Java: bumpedInto(Level, int, int, Entity) */
    tile_bumped_into_fn bumped_into;
    /* Occasional update: growth, spreading. Java: tick(Level, int, int) */
    tile_tick_fn tick;
    /* Something stepped onto the square. Java: steppedOn(Level, int, int, Entity) */
    tile_stepped_on_fn stepped_on;
    /* Using an item on the square: a shovel, a hoe... Java: interact(...) */
    tile_interact_fn interact;
    /* The use key on the square. Java: use(Level, int, int, Player, int) */
    tile_use_fn use;
    /* Liquid, for edge drawing? Java: connectsToLiquid() */
    tile_connects_to_liquid_fn connects_to_liquid;

    /* --- data --- */

    /*
     * This tile's own generator, for the randomness inside tick(), just
     * like Java's `protected Random random` in Tile.
     */
    Random random;

    /* Its own id, for the hooks that only receive the Tile. */
    TileID id;

    /* Configuration each subclass fixes in its init. */
    union {
        struct { boolean leadsUp; } stairs;
        struct { TileID onType; TileID growsTo; } sapling;
        struct { struct Resource* toDrop; int color; } ore;
    } add;

    /* Blends its edges with neighbouring grass. */
    boolean connects_to_grass : 1;
    /* Blends its edges with neighbouring sand. */
    boolean connects_to_sand  : 1;
    /* Has a liquid edge: lava. */
    boolean connects_to_lava  : 1;
    /* Has a liquid edge: water. */
    boolean connects_to_water : 1;
};

/*
 * The static tile registry, indexed by id. Java: `public static Tile[] tiles`.
 * tile_init() fills it in, the way `tiles[id] = this` does in Java.
 */
extern Tile* tiles[256];

/* Creates and installs the 23 tile types. Called once at startup. */
PUBLIC void init_tiles(void);

/* --- Implementations of the base Tile class (Java: the body of Tile) --- */
PUBLIC void    tile_init(Tile* this, TileID id);
PUBLIC void    tile_render(Tile* this, Screen* screen, struct Level* level, int x, int y);
PUBLIC boolean tile_may_pass(Tile* this, struct Level* level, int x, int y, struct Entity* e);
PUBLIC int     tile_get_light_radius(Tile* this, struct Level* level, int x, int y);
PUBLIC void    tile_hurt(Tile* this, struct Level* level, int x, int y, struct Mob* source, int dmg, int attackDir);
PUBLIC void    tile_bumped_into(Tile* this, struct Level* level, int xt, int yt, struct Entity* entity);
PUBLIC void    tile_tick(Tile* this, struct Level* level, int xt, int yt);
PUBLIC void    tile_stepped_on(Tile* this, struct Level* level, int xt, int yt, struct Entity* entity);
PUBLIC boolean tile_interact(Tile* this, struct Level* level, int xt, int yt, struct Player* player, struct Item* item, int attackDir);
PUBLIC boolean tile_use(Tile* this, struct Level* level, int xt, int yt, struct Player* player, int attackDir);
PUBLIC boolean tile_connects_to_liquid(Tile* this);

/* Global counter pacing the occasional ticks. Java: the static Tile.tickCount */
extern int tile_tick_count;

#endif /* LEVEL_TILE_TILE_H_ */
