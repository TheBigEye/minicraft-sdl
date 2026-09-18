/*
 * entity.h - The Entity class (Java: com.mojang.ld22.entity.Entity).
 *
 * Everything that is drawn or interacted with in the world (mobs,
 * furniture, dropped items, particles) starts from an Entity. Inheritance
 * is by composition: Entity is always the FIRST member of the struct, so a
 * pointer to the subclass also works as a pointer to Entity:
 *
 *     Entity  <- Mob <- Player / Slime / Zombie / AirWizard
 *     Entity  <- Furniture
 *     Entity  <- ItemEntity / Spark / SmashParticle / TextParticle
 *
 * Each object carries its own methods, installed by its create():
 * entity_create() puts in the base class ones and every subclass overrides
 * the ones it needs, just as Java's subclasses override the methods of
 * their parent. There are no vtables.
 *
 * The methods the original Java never overrides (init, intersects, move,
 * move2, interact, remove) stay as plain functions.
 */
#ifndef ENTITY_H
#define ENTITY_H 1

#include "entityid.h"

#include "../gfx/screen.h"
#include "../utils/javalang.h"
#include "../level/tile/tileids.h"
#include "../utils/javarandom.h"
#include "../utils/utils.h"

/* Forward declarations: Entity is referenced from all over the tree. */
struct Mob;
struct Player;
struct Item;
struct ItemEntity;
struct Level;

typedef struct Entity Entity;

/* Signatures of the Entity methods. */
typedef void    (*entity_tick_fn)            (Entity* this);
typedef void    (*entity_render_fn)          (Entity* this, Screen* screen);
typedef boolean (*entity_blocks_fn)          (Entity* this, Entity* other);
typedef void    (*entity_hurt_fn)            (Entity* this, struct Mob* source, int dmg, int attackDir);
typedef void    (*entity_hurt_tile_fn)       (Entity* this, TileID tile, int x, int y, int dmg);
typedef void    (*entity_touched_by_fn)      (Entity* this, Entity* other);
typedef boolean (*entity_is_blockable_by_fn) (Entity* this, struct Mob* mob);
typedef void    (*entity_touch_item_fn)      (Entity* this, struct ItemEntity* item);
typedef boolean (*entity_can_swim_fn)        (Entity* this);
typedef boolean (*entity_use_fn)             (Entity* this, struct Player* player, int attackDir);
typedef int     (*entity_get_light_radius_fn)(Entity* this);
typedef void    (*entity_die_fn)             (Entity* this);
typedef void    (*entity_do_hurt_fn)         (Entity* this, int damage, int attackDir);
typedef boolean (*entity_is_swimming_fn)     (Entity* this);
typedef void    (*entity_free_fn)            (Entity* this);

struct Entity {
    /* --- methods: installed by entity_create(), overridden by subclasses --- */

    /* Java: Entity.tick() */
    entity_tick_fn tick;
    /* Java: Entity.render(Screen) */
    entity_render_fn render;
    /* Java: Entity.blocks(Entity) */
    entity_blocks_fn blocks;
    /*
     * Java: Entity.hurt(Mob mob, int dmg, int attackDir).
     *
     * Java overloads hurt() on its arguments, hurt(Mob, int, int) and
     * hurt(Tile, int, int, int), so the port keeps one slot per
     * signature: this one and hurt_tile below. See javalang.h: the macros
     * there give a single name at the call site when that is wanted.
     */
    entity_hurt_fn hurt;
    /* Java: Entity.hurt(Tile, int, int, int), the second hurt() overload. */
    entity_hurt_tile_fn hurt_tile;
    /* Java: Entity.touchedBy(Entity) */
    entity_touched_by_fn touched_by;
    /* Java: Entity.isBlockableBy(Mob) */
    entity_is_blockable_by_fn is_blockable_by;
    /* Java: Entity.touchItem(ItemEntity) */
    entity_touch_item_fn touch_item;
    /* Java: Entity.canSwim() */
    entity_can_swim_fn can_swim;
    /* Java: Entity.use(Player, int) */
    entity_use_fn use;
    /* Java: Entity.getLightRadius() */
    entity_get_light_radius_fn get_light_radius;
    /* Java: Mob.die() */
    entity_die_fn die;
    /* Java: Mob.doHurt(int, int) */
    entity_do_hurt_fn do_hurt;
    /* Java: Mob.isSwimming() */
    entity_is_swimming_fn is_swimming;
    /* Frees what the object owns, not the struct itself. C-only. */
    entity_free_fn free;

    /* --- data --- */

    /* Class tag: this is where Java used instanceof. */
    EntityId type;
    /* Java: `protected Random random`, shared with every subclass. */
    Random random;
    /* Position, in pixels. */
    int x, y;
    /* Half the bounding box, in pixels. Java: `protected int xr, yr`. */
    int xr, yr;
    struct Level* level;
    /* Java: `public boolean removed` */
    boolean removed;
};

/* Constructor: installs the base class methods. Java: Entity() */
PUBLIC void entity_create(Entity* this);

/*
 * The methods the original Java never overrides, so they stay as plain
 * functions instead of taking a slot in every subclass.
 */
PUBLIC void    entity_remove(Entity* this);
PUBLIC void    entity_init(Entity* this, struct Level* level);
PUBLIC boolean entity_intersects(Entity* this, int x0, int y0, int x1, int y1);
PUBLIC boolean entity_move(Entity* this, int xa, int ya);
PUBLIC boolean entity_move2(Entity* this, int xa, int ya);
PUBLIC boolean entity_interact(Entity* this, struct Player* player, struct Item* item, int attackDir);

/*
 * The base class implementations, that is, the body of Java's Entity.
 * They are declared here because the subclasses need them by name, both
 * to inherit them and to call them from their own overrides.
 */
PUBLIC void    entity_tick(Entity* this);
PUBLIC void    entity_render(Entity* this, Screen* screen);
PUBLIC boolean entity_blocks(Entity* this, Entity* other);
PUBLIC void    entity_hurt(Entity* this, struct Mob* source, int dmg, int attackDir);
PUBLIC void    entity_hurt_tile(Entity* this, TileID tile, int x, int y, int dmg);
PUBLIC void    entity_touched_by(Entity* this, Entity* other);
PUBLIC boolean entity_is_blockable_by(Entity* this, struct Mob* mob);
PUBLIC void    entity_touch_item(Entity* this, struct ItemEntity* item);
PUBLIC boolean entity_can_swim(Entity* this);
PUBLIC boolean entity_use(Entity* this, struct Player* player, int attackDir);
PUBLIC int     entity_get_light_radius(Entity* this);
PUBLIC void    entity_die(Entity* this);
PUBLIC void    entity_do_hurt(Entity* this, int damage, int attackDir);
PUBLIC boolean entity_is_swimming(Entity* this);
PUBLIC void    entity_free(Entity* this);

#endif /* ENTITY_H */
