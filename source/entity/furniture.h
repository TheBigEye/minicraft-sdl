/*
 * furniture.h - The base class of the placeable furniture
 *               (Java: com.mojang.ld22.entity.Furniture): the crafting
 *               stations, the chests and the lanterns.
 */
#ifndef FURNITURE_H
#define FURNITURE_H 1

#include "entity.h"

#include "../utils/javalang.h"

struct Player;

typedef struct Furniture Furniture;

struct Furniture {
    /* Inheritance: Entity, always the first member. */
    Entity entity;
    /* Ticks left before it can be pushed again, and the direction of the
     * push. Java: `protected int pushTime` and pushDir. */
    int pushTime, pushDir;
    /* Packed colour and sprite index it is drawn with. */
    int col, sprite;
    /* Display name; the furniture owns it. Java: `protected String name` */
    String name;
    /* Player that has to pick it up next tick. Java: shouldTake */
    struct Player* shouldTake;
};

/*
 * Constructor: the Entity base first, its `super()`, and then the methods
 * Furniture overrides. Its subclasses, that is, Anvil, Chest, Furnace,
 * Lantern, Oven and Workbench, call this and replace only use().
 * Java: Furniture(String name)
 */
PUBLIC void furniture_create(Furniture* this, String name);

/* Java: Furniture.tick() */
PUBLIC void furniture_tick(Furniture* this);

/* Java: Furniture.render(Screen) */
PUBLIC void furniture_render(Furniture* this, Screen* screen);

/* Java: Furniture.touchedBy(Entity) */
PUBLIC void furniture_touched_by(Furniture* this, Entity* entity);

/* Hands the furniture to the player, for the power glove. Java: take(Player) */
PUBLIC void furniture_take(Furniture* this, struct Player* player);

/* Releases the display name. C-only: a Java String needs no freeing. */
PUBLIC void furniture_free(Furniture* this);

/* Furniture always blocks other entities. Java: blocks(Entity) { return true; } */
PUBLIC boolean furniture_blocks(Furniture* this, Entity* other);

/* The C counterpart of Java's `e instanceof Furniture`. */
PUBLIC boolean entity_is_furniture(Entity* entity);

/* Factory: builds a furniture entity by id. Java: new Anvil(), new Chest()... */
PUBLIC Furniture* entity_create_furniture(EntityId id);

/*
 * Deep copy, used when the power glove picks the furniture up: ownership
 * of the name moves to the copy, and a chest copies its inventory too.
 * C-only.
 */
PUBLIC Furniture* furniture_create_copy(Furniture* old);

#endif /* FURNITURE_H */
