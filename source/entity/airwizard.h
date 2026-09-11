/*
 * airwizard.h - The Air Wizard: boss mob of the game.
 *
 * A 2000-HP wizard roaming the sky island; defeating it wins the
 * game. It attacks by firing spark projectiles in patterns.
 */
#ifndef AIRWIZARD_H
#define AIRWIZARD_H

#include "mob.h"

struct _Screen;

typedef struct _AirWizard{
	Mob mob;
	int xa, ya;           /* current walking velocity */
	int randomWalkTime;   /* ticks until a new random direction */
	int attackDelay;      /* countdown to the next attack cycle */
	int attackTime;       /* ticks left in the current cycle */
	int attackType;       /* which spark pattern is being fired */
} AirWizard;

void airwizard_create(AirWizard* wizard);
void airwizard_tick(AirWizard* wizard);
void airwizard_doHurt(AirWizard* wizard, int damage, int attackDir);
void airwizard_render(AirWizard* wizard, struct _Screen* screen);
void airwizard_touchedBy(AirWizard* wizard, Entity* entity);
void airwizard_die(AirWizard* wizard);


#endif // AIRWIZARD_H
