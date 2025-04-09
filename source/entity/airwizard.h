#ifndef AIRWIZARD_H
#define AIRWIZARD_H

#include "mob.h"

struct _Screen;

typedef struct _AirWizard{
	Mob mob;
	int xa, ya;
	int randomWalkTime;
	int attackDelay;
	int attackTime;
	int attackType;
} AirWizard;

void airwizard_create(AirWizard* wizard);
void airwizard_tick(AirWizard* wizard);
void airwizard_doHurt(AirWizard* wizard, int damage, int attackDir);
void airwizard_render(AirWizard* wizard, struct _Screen* screen);
void airwizard_touchedBy(AirWizard* wizard, Entity* entity);
void airwizard_die(AirWizard* wizard);


#endif // AIRWIZARD_H
