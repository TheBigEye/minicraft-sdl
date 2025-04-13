#include "textparticle.h"
#include "../../gfx/font.h"
#include <stdlib.h>
#include "../../gfx/color.h"

#include <string.h>

void textparticle_create(TextParticle* particle, char* message, int x, int y, int col) {
	entity_create(&particle->entity);

	particle->entity.type = TEXTPARTICLE;
	particle->time = 0;
	particle->freee = 1;
	particle->msg = message;
	particle->msgLen = strlen(message);

	particle->entity.x = x;
	particle->entity.y = y;
	particle->col = col;
	particle->xx = x;
	particle->yy = y;
	particle->zz = 2;

	particle->xa = random_next_gaussian(&particle->entity.random) * 0.3;
	particle->ya = random_next_gaussian(&particle->entity.random) * 0.2;
	particle->za = random_next_float(&particle->entity.random) * 0.7 + 2;
}


void textparticle_tick(TextParticle* particle) {
	++particle->time;

	if (particle->time > 60) {
        entity_remove(&particle->entity);
    }

	particle->xx += particle->xa;
	particle->yy += particle->ya;
	particle->zz += particle->za;

	if (particle->zz < 0) {
		particle->zz = 0;
		particle->za *= -0.5;
		particle->xa *= 0.6;
		particle->ya *= 0.6;
	}

	particle->za -= 0.15;
	particle->entity.x = (int) particle->xx;
	particle->entity.y = (int) particle->yy;
}


void textparticle_render(TextParticle* particle, Screen* screen) {
	int x = particle->entity.x;
	int y = particle->entity.y;
	font_draw(particle->msg, particle->msgLen, screen, x - particle->msgLen*4 + 1, y - ((int) particle->zz) + 1, getColor4(-1, 0, 0, 0));
	font_draw(particle->msg, particle->msgLen, screen, x - particle->msgLen*4, y - ((int) particle->zz), particle->col);
}


void textparticle_free(TextParticle* particle) {
	if (particle->freee) {
		free(particle->msg);
	}
}
