/*
 * itementity.h - A dropped item lying in the world, waiting to be picked
 *                up by a player (Java: com.mojang.ld22.entity.ItemEntity).
 */
#ifndef ITEMENTITY_H
#define ITEMENTITY_H 1

#include "entity.h"

#include "../utils/javalang.h"
#include "../item/item.h"
#include "player.h"

typedef struct ItemEntity ItemEntity;

struct ItemEntity {
    /* Inheritance: Entity, always the first member. */
    Entity entity;

    /* Ticks before it despawns. Java: `public int lifeTime` */
    int lifeTime;
    /* Animation counter, which drives the bob. Java: walkDist */
    int walkDist;
    /* Facing: unused here, kept for parity. Java: dir */
    int dir;
    /* Ticks it cannot be taken for after being dropped. Java: hurtTime */
    int hurtTime;
    /* Java: xKnockback, yKnockback */
    int xKnockback, yKnockback;
    /* Velocity of the pop. Java: xa, ya, za */
    double xa, ya, za;
    /* Real position, ahead of the Entity's integer one. Java: xx, yy, zz */
    double xx, yy, zz;
    /* The item it carries. Java: `public Item item` */
    Item item;
    /* Ticks since it was dropped. Java: `public int time` */
    int time;
};

/* Constructor. Java: ItemEntity(Item item, int x, int y) */
PUBLIC void itementity_create(ItemEntity* this, Item item, int x, int y);

/* Java: ItemEntity.render(Screen) */
PUBLIC void itementity_render(ItemEntity* this, Screen* screen);

/* Java: ItemEntity.tick() */
PUBLIC void itementity_tick(ItemEntity* this);

/* Java: ItemEntity.touchedBy(Entity) */
PUBLIC void itementity_touched_by(ItemEntity* this, Entity* entity);

/* Java: ItemEntity.take(Player) */
PUBLIC void itementity_take(ItemEntity* this, Player* player);

/* Java: ItemEntity.isBlockableBy(Mob mob) { return false; } */
PUBLIC boolean itementity_is_blockable_by(ItemEntity* this, struct Mob* mob);

#endif /* ITEMENTITY_H */
