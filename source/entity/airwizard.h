/*
 * airwizard.h - The Air Wizard, the boss of the game
 *               (Java: com.mojang.ld22.entity.AirWizard).
 *
 * A 2000-HP wizard roaming the sky island; defeating it wins the game. It
 * attacks by firing spark projectiles in patterns.
 */
#ifndef AIRWIZARD_H
#define AIRWIZARD_H 1

#include "mob.h"

#include "../utils/javalang.h"

typedef struct AirWizard AirWizard;

struct AirWizard {
    /* Inheritance: Mob, always the first member. */
    Mob mob;
    /* Current walking velocity. Java: `private int xa, ya` */
    int xa, ya;
    /* Ticks until it picks a new random direction. Java: randomWalkTime */
    int randomWalkTime;
    /* Countdown to the next attack cycle. Java: `private int attackDelay` */
    int attackDelay;
    /* Ticks left in the current cycle. Java: `private int attackTime` */
    int attackTime;
    /* Which spark pattern is being fired. Java: `private int attackType` */
    int attackType;
};

/* Constructor. Java: AirWizard() */
PUBLIC void airwizard_create(AirWizard* this);

/* Java: AirWizard.tick() */
PUBLIC void airwizard_tick(AirWizard* this);

/* Java: AirWizard.doHurt(int damage, int attackDir) */
PUBLIC void airwizard_do_hurt(AirWizard* this, int damage, int attackDir);

/* Java: AirWizard.render(Screen) */
PUBLIC void airwizard_render(AirWizard* this, Screen* screen);

/* Java: AirWizard.touchedBy(Entity) */
PUBLIC void airwizard_touched_by(AirWizard* this, Entity* entity);

/* Java: AirWizard.die() */
PUBLIC void airwizard_die(AirWizard* this);

#endif /* AIRWIZARD_H */
