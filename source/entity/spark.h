/*
 * spark.h - The projectile the Air Wizard fires
 *           (Java: com.mojang.ld22.entity.Spark).
 */
#ifndef SPARK_H
#define SPARK_H 1

#include "entity.h"

#include "../utils/javalang.h"

struct AirWizard;

typedef struct Spark Spark;

struct Spark {
    /* Inheritance: Entity, always the first member. */
    Entity entity;
    /* Ticks before the spark expires. */
    int lifeTime;
    /* Velocity components. Java: `private double xa, ya` */
    double xa, ya;
    /* Sub-pixel position. Java: `private double xx, yy` */
    double xx, yy;
    /* Ticks alive. */
    int time;
    /* The wizard that fired it, which is the source of the damage. */
    struct AirWizard* owner;
};

/* Fires a spark from the owner's position with velocity (xa, ya). */
PUBLIC void spark_create(Spark* this, struct AirWizard* owner, double xa, double ya);

/* Moves the spark and hurts every mob but the wizards that it touches. */
PUBLIC void spark_tick(Spark* this);

/* Draws a flickering two-tone spark; it blinks near expiry. */
PUBLIC void spark_render(Spark* this, Screen* screen);

/* Sparks never block movement. Java: isBlockableBy(Mob) { return false; } */
PUBLIC boolean spark_is_blockable_by(Spark* this, struct Mob* mob);

#endif /* SPARK_H */
