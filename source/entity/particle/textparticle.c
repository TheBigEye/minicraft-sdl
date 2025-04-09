#include "textparticle.h"
#include "../../gfx/font.h"
#include <stdlib.h>
#include "../../gfx/color.h"

#include <string.h>

void textparticle_create(TextParticle* entity, char* message, int x, int y, int col){
	entity_create(&entity->entity);
	entity->entity.type = TEXTPARTICLE;
	entity->time = 0;
	entity->freee = 1;
	entity->msg = message;
	entity->msgLen = strlen(message);
	entity->entity.x = x;
	entity->entity.y = y;
	entity->col = col;
	entity->xx = x;
	entity->yy = y;
	entity->zz = 2;

	entity->xa = random_next_gaussian(&entity->entity.random) * 0.3;
	entity->ya = random_next_gaussian(&entity->entity.random) * 0.2;
	entity->za = random_next_float(&entity->entity.random) * 0.7 + 2;
}

void textparticle_tick(TextParticle* entity){
	++entity->time;
	if(entity->time > 60) entity_remove((Entity *) entity);

	entity->xx += entity->xa;
	entity->yy += entity->ya;
	entity->zz += entity->za;
	if(entity->zz < 0){
		entity->zz = 0;
		entity->za *= -0.5;
		entity->xa *= 0.6;
		entity->ya *= 0.6;
	}
	entity->za -= 0.15;
	entity->entity.x = (int) entity->xx;
	entity->entity.y = (int) entity->yy;
}
void textparticle_render(TextParticle* entity, Screen* screen){
	int x = entity->entity.x;
	int y = entity->entity.y;
	font_draw(entity->msg, entity->msgLen, screen, x - entity->msgLen*4 + 1, y - (int)entity->zz + 1, getColor4(-1, 0, 0, 0));
	font_draw(entity->msg, entity->msgLen, screen, x - entity->msgLen*4, y - (int)entity->zz, entity->col);
}

void textparticle_free(TextParticle* entity){
	if(entity->freee){
		free(entity->msg);
	}
}
