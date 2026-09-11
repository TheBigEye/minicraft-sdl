/*
 * textparticle.c - TextParticle (Java: entity.particle.TextParticle).
 *
 * Floating text with simple ballistics: spawned with a random gaussian
 * velocity, it arcs under gravity, bounces on the ground with friction
 * and fades out after 60 ticks. Rendered twice for a drop-shadow look.
 */
#include "textparticle.h"
#include "../../gfx/font.h"
#include <stdlib.h>
#include "../../gfx/color.h"

#include <string.h>

/* The TextParticle vtable (= the Java `TextParticle` class). */
static const EntityVTable textparticle_vtable = {
	.tick           = (vt_tick_fn) textparticle_tick,
	.render         = (vt_render_fn) textparticle_render,
	.blocks         = entity_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = entity_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = entity_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = (vt_free_fn) textparticle_free,
};

/*
 * Spawns the text at (x, y) with a small random pop velocity. The
 * message pointer is owned by the particle (freee=1) unless the caller
 * clears the flag for static strings.
 */
void textparticle_create(TextParticle* particle, char* message, int x, int y, int col) {
	entity_create(&particle->entity);
	particle->entity.vt = &textparticle_vtable;

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


/* Integrates velocity into position; gravity pulls zz down and ground
 * hits bounce with damping. Removes itself after 60 ticks. */
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


/* Draws the message horizontally centered at its height, first in black
 * one pixel offset (shadow), then in the particle's color. */
void textparticle_render(TextParticle* particle, Screen* screen) {
	int x = particle->entity.x;
	int y = particle->entity.y;
	font_draw(particle->msg, particle->msgLen, screen, x - particle->msgLen*4 + 1, y - ((int) particle->zz) + 1, getColor4(-1, 0, 0, 0));
	font_draw(particle->msg, particle->msgLen, screen, x - particle->msgLen*4, y - ((int) particle->zz), particle->col);
}


/* Releases the message buffer when the particle owns it. */
void textparticle_free(TextParticle* particle) {
	if (particle->freee) {
		free(particle->msg);
	}
}
