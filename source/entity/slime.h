#ifndef SLIME_H
#define SLIME_H

#include "mob.h"

typedef struct _Slime{
	Mob mob;
	int xa, ya;
	int jumpTime;
	int lvl;
} Slime;

void slime_create(Slime* slime, int lvl);
void slime_tick(Slime* slime);
void slime_die(Slime* slime);
void slime_render(Slime* slime, struct _Screen* screen);
void slime_touchedBy(Slime* slime, struct _Entity* entity);
#endif // SLIME_H
