#ifndef TEXTPARTICLE_H
#define TEXTPARTICLE_H

#include "../entity.h"
#include "../../gfx/screen.h"

typedef struct{
	Entity entity;
	char freee;
	char* msg;
	int msgLen;
	int col;
	int time;
	float xa, ya, za;
	float xx, yy, zz;
} TextParticle;

void textparticle_create(TextParticle* entity, char* message, int x, int y, int col);
void textparticle_tick(TextParticle* entity);
void textparticle_render(TextParticle* entity, Screen* screen);
void textparticle_free(TextParticle* entity);

#endif // TEXTPARTICLE_H
