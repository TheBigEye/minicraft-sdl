#ifndef SMASHPARTICLE_H
#define SMASHPARTICLE_H
#include "../entity.h"
#include "../../gfx/screen.h"

typedef struct{
	Entity entity;
	int time;
} SmashParticle;

void smashparticle_create(SmashParticle* particle, int x, int y);
void smashparticle_tick(SmashParticle* particle);
void smashparticle_render(SmashParticle* particle, Screen* screen);

#endif // SMASHPARTICLE_H
