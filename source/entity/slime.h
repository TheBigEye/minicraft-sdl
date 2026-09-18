/*
 * slime.h - The Slime: a hopping blob that drops slime resources
 *           (Java: com.mojang.ld22.entity.Slime).
 */
#ifndef SLIME_H
#define SLIME_H 1

#include "mob.h"

#include "../utils/javalang.h"

typedef struct Slime Slime;

struct Slime {
    /* Inheritance: Mob, always the first member. */
    Mob mob;
    /* Direction it is heading in. Java: `private int xa, ya` */
    int xa, ya;
    /* Ticks left in the current hop. Java: `private int jumpTime` */
    int jumpTime;
    /* Level, 1 to 4: it drives health, damage and colour. Java: lvl */
    int lvl;
};

/* Constructor. Java: Slime(int lvl) */
PUBLIC void slime_create(Slime* this, int lvl);

/* Java: Slime.tick() */
PUBLIC void slime_tick(Slime* this);

/* Java: Slime.die() */
PUBLIC void slime_die(Slime* this);

/* Java: Slime.render(Screen) */
PUBLIC void slime_render(Slime* this, Screen* screen);

/* Java: Slime.touchedBy(Entity) */
PUBLIC void slime_touched_by(Slime* this, Entity* entity);

#endif /* SLIME_H */
