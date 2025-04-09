#ifndef ZOMBIE_H
#define ZOMBIE_H
#include "mob.h"

struct _Screen;

typedef struct _Zombie{
	Mob mob;
	int xa, ya;
	int lvl;
	int randomWalkTime;
} Zombie;

void zombie_create(Zombie* zombie, int lvl);
void zombie_tick(Zombie* zombie);
void zombie_render(Zombie* zombie, struct _Screen* screen);
void zombie_touchedBy(Zombie* zombie, struct _Entity* entity);
void zombie_die(Zombie* zombie);

#endif // ZOMBIE_H
