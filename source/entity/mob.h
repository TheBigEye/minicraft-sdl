/*
 * mob.h - The Mob class: the base of everything alive, that is, the
 *         player, the slime, the zombie and the Air Wizard
 *         (Java: com.mojang.ld22.entity.Mob).
 *
 * It adds health, facing, knockback, the cooldown that follows a hit and
 * swimming on top of Entity. Inheritance is by composition: Entity is the
 * FIRST member, so a Mob* also works as an Entity*.
 *
 * The polymorphic methods (tick, hurt, hurt_tile, die, do_hurt,
 * is_swimming and blocks) live in the embedded Entity, which is where the
 * base class already has them; mob_create() swaps them for Mob's. Only
 * the Mob methods that no subclass overrides go here: move(), heal() and
 * findStartPos().
 */
#ifndef MOB_H
#define MOB_H 1

#include "entity.h"

#include "../utils/javalang.h"
#include "../level/tile/tileids.h"

struct Level;

typedef struct Mob Mob;

/* Signatures of the Mob methods. */
typedef boolean (*mob_move_fn)          (Mob* this, int xa, int ya);
typedef boolean (*mob_find_start_pos_fn)(Mob* this, struct Level* level);
typedef void    (*mob_heal_fn)          (Mob* this, int heal);

struct Mob {
    /* Inheritance: Entity, always the first member. */
    Entity entity;

    /* --- methods, installed by mob_create() --- */

    /* Java: Mob.move(int xa, int ya) */
    mob_move_fn move;
    /* Java: Mob.findStartPos(Level) */
    mob_find_start_pos_fn find_start_pos;
    /* Java: Mob.heal(int) */
    mob_heal_fn heal;

    /* --- data --- */

    /* Steps taken; it drives the walk animation. Java: walkDist */
    int walkDist;
    /* Facing: 0 down, 1 up, 2 left, 3 right. Java: dir */
    int dir;
    /* Invulnerability ticks left after a hit. Java: hurtTime */
    int hurtTime;
    /* Pending knockback, in pixels, per axis. Java: xKnockback, yKnockback */
    int xKnockback, yKnockback;
    int maxHealth;
    int health;
    /* Ticks spent in water or lava; it halves the move rate. Java: swimTimer */
    int swimTimer;
    /* Total ticks alive. Java: tickTime */
    int tickTime;
};

/* The C counterpart of Java's `e instanceof Mob`. */
PUBLIC boolean entity_is_mob(Entity* entity);

/*
 * Constructor: the Entity base first, its `super()`, and then the methods
 * Mob overrides plus its own three. Subclasses call this first and only
 * replace what they in turn override.
 */
PUBLIC void mob_create(Mob* this);

/* --- Mob's implementations (Java: the body of the Mob class) --- */

/* Per-tick update: lava damage, death check, hurt cooldown. */
PUBLIC void mob_tick(Mob* this);

/* Removes the mob from the level. Java: Mob.die() */
PUBLIC void mob_die(Mob* this);

/* Movement with knockback, swim pacing and facing update. */
PUBLIC boolean mob_move(Mob* this, int xa, int ya);

/* True while standing in water or lava. Java: Mob.isSwimming() */
PUBLIC boolean mob_is_swimming(Mob* this);

/* A mob blocks the entities that report isBlockableBy(mob). */
PUBLIC boolean mob_blocks(Mob* this, Entity* entity);

/* Restores health, capped at maxHealth, with a green number popup. */
PUBLIC void mob_heal(Mob* this, int heal);

/* Applies damage: red number popup, knockback and hurt cooldown. */
PUBLIC void mob_do_hurt(Mob* this, int damage, int attackDir);

/* Picks a random spawn tile far from the player and from other mobs. */
PUBLIC boolean mob_find_start_pos(Mob* this, struct Level* level);

/*
 * Damage coming from a tile, lava or a cactus: it forwards to do_hurt.
 * Java: Mob.hurt(Tile tile, int x, int y, int damage)
 */
PUBLIC void mob_hurt_tile(Mob* this, TileID tile, int x, int y, int damage);

/*
 * Damage coming from another mob: it forwards to the virtual do_hurt.
 * Java: Mob.hurt(Mob mob, int damage, int attackDir)
 *
 * Java overloads hurt() on its arguments, so the port keeps one name per
 * signature: this one and mob_hurt_tile above. See javalang.h.
 */
PUBLIC void mob_hurt(Mob* this, Mob* by, int damage, int attackDir);

#endif /* MOB_H */
