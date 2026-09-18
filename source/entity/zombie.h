/*
 * zombie.h - The Zombie: a walking undead that drops cloth
 *            (Java: com.mojang.ld22.entity.Zombie).
 */
#ifndef ZOMBIE_H
#define ZOMBIE_H 1

#include "mob.h"

#include "../utils/javalang.h"

typedef struct Zombie Zombie;

struct Zombie {
    /* Inheritance: Mob, always the first member. */
    Mob mob;
    /* Direction it is heading in. Java: `private int xa, ya` */
    int xa, ya;
    /* Level, 1 to 4: it drives health, damage and colour. Java: lvl */
    int lvl;
    /* Ticks left wandering in the current direction. Java: randomWalkTime */
    int randomWalkTime;
};

/* Constructor. Java: Zombie(int lvl) */
PUBLIC void zombie_create(Zombie* this, int lvl);

/* Java: Zombie.tick() */
PUBLIC void zombie_tick(Zombie* this);

/* Java: Zombie.render(Screen) */
PUBLIC void zombie_render(Zombie* this, Screen* screen);

/* Java: Zombie.touchedBy(Entity) */
PUBLIC void zombie_touched_by(Zombie* this, Entity* entity);

/* Java: Zombie.die() */
PUBLIC void zombie_die(Zombie* this);

#endif /* ZOMBIE_H */
