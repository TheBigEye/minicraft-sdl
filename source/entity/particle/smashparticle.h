/*
 * smashparticle.h - Burst effect shown when something is smashed
 *                   (tiles broken, furniture picked up, ...).
 */
#ifndef SMASHPARTICLE_H
#define SMASHPARTICLE_H
#include "../entity.h"
#include "../../gfx/screen.h"

typedef struct{
	Entity entity;
	int time;   /* ticks alive; the burst lasts 10 */
} SmashParticle;

/* Spawns the burst at pixel (x, y) and plays the smash sound. */
void smashparticle_create(SmashParticle* particle, int x, int y);
/* Ages the burst; removes it after its lifetime. */
void smashparticle_tick(SmashParticle* particle);
/* Draws the four-quadrant mirrored tile burst. */
void smashparticle_render(SmashParticle* particle, Screen* screen);

#endif // SMASHPARTICLE_H
