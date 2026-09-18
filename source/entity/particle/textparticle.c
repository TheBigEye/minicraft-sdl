/*
 * textparticle.c - The TextParticle
 *                  (Java: com.mojang.ld22.entity.particle.TextParticle).
 *
 * Floating text with simple ballistics: spawned with a random gaussian
 * velocity, it arcs under gravity, bounces on the ground with friction
 * and fades out after 60 ticks. It is drawn twice, for a drop shadow.
 */
#include "textparticle.h"

#include <stdlib.h>
#include <string.h>

#include "../../gfx/color.h"
#include "../../gfx/font.h"


/*
 * Constructor: spawns the text at (x, y) with a small random pop
 * velocity. The particle owns the message, and so frees it, unless the
 * caller clears owns_msg for a static string.
 */
PUBLIC void textparticle_create(TextParticle* this, String message, int x, int y, int col) {
    entity_create(&this->entity);   /* super() */

    /* What TextParticle overrides from Entity, destructor included: it
     * releases the string when the message is its own.
     * Java: class TextParticle extends Entity */
    this->entity.tick   = (entity_tick_fn) textparticle_tick;
    this->entity.render = (entity_render_fn) textparticle_render;
    this->entity.free   = (entity_free_fn) textparticle_free;

    this->entity.type = TEXTPARTICLE;

    this->time = 0;
    this->owns_msg = true;
    this->msg = message;
    this->msgLen = strlen(message);

    this->entity.x = x;
    this->entity.y = y;
    this->col = col;
    this->xx = x;
    this->yy = y;
    this->zz = 2;

    this->xa = this->entity.random.next_gaussian(&this->entity.random) * 0.3;
    this->ya = this->entity.random.next_gaussian(&this->entity.random) * 0.2;
    this->za = this->entity.random.next_float(&this->entity.random) * 0.7 + 2;
}


/* Integrates velocity into position: gravity pulls zz down and ground
 * hits bounce with damping. It removes itself after 60 ticks. */
PUBLIC void textparticle_tick(TextParticle* this) {
    ++this->time;

    if (this->time > 60) {
        entity_remove(&this->entity);
    }

    this->xx += this->xa;
    this->yy += this->ya;
    this->zz += this->za;

    if (this->zz < 0) {
        this->zz = 0;
        this->za *= -0.5;
        this->xa *= 0.6;
        this->ya *= 0.6;
    }

    this->za -= 0.15;

    this->entity.x = (int) this->xx;
    this->entity.y = (int) this->yy;
}


/* Draws the message horizontally centred at its height, first in black
 * one pixel off, as a shadow, then in the particle's own colour. */
PUBLIC void textparticle_render(TextParticle* this, Screen* screen) {
    int x = this->entity.x;
    int y = this->entity.y;

    font_draw(this->msg, this->msgLen, screen, x - this->msgLen * 4 + 1, y - (int) this->zz + 1, get_color4(-1, 0, 0, 0));
    font_draw(this->msg, this->msgLen, screen, x - this->msgLen * 4, y - (int) this->zz, this->col);
}


/* Releases the message buffer, but only when the particle owns it. */
PUBLIC void textparticle_free(TextParticle* this) {
    if (this->owns_msg) {
        delete(this->msg);
    }
}
