/*
 * textparticle.h - Floating text effect (damage numbers, pickup labels).
 */
#ifndef TEXTPARTICLE_H
#define TEXTPARTICLE_H

#include "../entity.h"
#include "../../gfx/screen.h"

typedef struct{
	Entity entity;
	char freee;   /* whether this particle owns (must free) msg */
	char* msg;
	int msgLen;
	int col;      /* packed color of the text */
	int time;     /* ticks alive; the particle lasts 60 */
	float xa, ya, za; /* velocity components (za is vertical bounce) */
	float xx, yy, zz; /* sub-pixel position, zz = height above ground */
} TextParticle;

/* Spawns floating text at (x, y); takes ownership of `message`. */
void textparticle_create(TextParticle* entity, char* message, int x, int y, int col);
/* Advances the ballistic motion (gravity + ground bounce). */
void textparticle_tick(TextParticle* entity);
/* Draws the text centered, with a black shadow one pixel behind. */
void textparticle_render(TextParticle* entity, Screen* screen);
/* Frees the message string when the particle owns it. */
void textparticle_free(TextParticle* entity);

#endif // TEXTPARTICLE_H
