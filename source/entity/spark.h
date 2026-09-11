/*
 * spark.h - Spark projectile fired by the Air Wizard.
 */
#ifndef SPARK_H
#define SPARK_H
#include "entity.h"

struct _AirWizard;
struct _Screen;

typedef struct _Spark{
	Entity entity;
	int lifeTime;         /* ticks before the spark expires */
	double xa, ya;        /* velocity components */
	double xx, yy;        /* sub-pixel position */
	int time;             /* ticks alive */
	struct _AirWizard* owner; /* wizard that fired it (damage source) */
} Spark;

/* Fires a spark from the owner's position with velocity (xa, ya). */
void spark_create(Spark* spark, struct _AirWizard* owner, double xa, double ya);
/* Moves the spark and hurts any mob (but wizards) it touches. */
void spark_tick(Spark* spark);
/* Draws a flickering two-tone spark; blinks near expiry. */
void spark_render(Spark* spark,struct _Screen* screen);
/* Sparks never block movement (Java: isBlockableBy false). */
char spark_isBlockableBy(Spark* spark, struct _Mob* mob);

#endif // SPARK_H
