/*
 * textparticle.h - The floating text effect: damage numbers, pickup
 *                  labels (Java: com.mojang.ld22.entity.particle.TextParticle).
 */
#ifndef TEXTPARTICLE_H
#define TEXTPARTICLE_H 1

#include "../entity.h"

#include "../../gfx/screen.h"
#include "../../utils/javalang.h"

typedef struct TextParticle TextParticle;

struct TextParticle {
    /* Inheritance: Entity, always the first member. */
    Entity entity;
    /*
     * Whether this particle owns the message and so has to free it.
     * C-only: a Java String needs no such bookkeeping.
     */
    boolean owns_msg;
    /* The text to draw. Java: `private String msg` */
    String msg;
    /* Length of msg, cached for the centring maths. C-only. */
    int msgLen;
    /* Packed colour of the text. Java: `private int col` */
    int col;
    /* Ticks alive; the particle lasts 60. Java: `private int time` */
    int time;
    /* Velocity; za is the vertical bounce. Java: `private double xa, ya, za` */
    float xa, ya, za;
    /* Sub-pixel position; zz is the height above ground. Java: xx, yy, zz */
    float xx, yy, zz;
};

/* Spawns floating text at (x, y); it takes ownership of `message`. */
PUBLIC void textparticle_create(TextParticle* this, String message, int x, int y, int col);

/* Advances the ballistic motion: gravity plus the ground bounce. */
PUBLIC void textparticle_tick(TextParticle* this);

/* Draws the text centred, with a black shadow one pixel behind. */
PUBLIC void textparticle_render(TextParticle* this, Screen* screen);

/* Frees the message string when the particle owns it. C-only. */
PUBLIC void textparticle_free(TextParticle* this);

#endif /* TEXTPARTICLE_H */
