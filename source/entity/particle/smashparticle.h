/*
 * smashparticle.h - The burst shown when something is smashed: a broken
 *                   tile, furniture picked back up...
 *                   (Java: com.mojang.ld22.entity.particle.SmashParticle).
 */
#ifndef SMASHPARTICLE_H
#define SMASHPARTICLE_H 1

#include "../entity.h"

#include "../../gfx/screen.h"
#include "../../utils/javalang.h"

typedef struct SmashParticle SmashParticle;

struct SmashParticle {
    /* Inheritance: Entity, always the first member. */
    Entity entity;
    /* Ticks alive; the burst lasts 10. Java: `private int time` */
    int time;
};

/* Spawns the burst at pixel (x, y) and plays the smash sound. */
PUBLIC void smashparticle_create(SmashParticle* this, int x, int y);

/* Ages the burst and removes it once its lifetime is over. Java: tick() */
PUBLIC void smashparticle_tick(SmashParticle* this);

/* Draws the four-quadrant mirrored tile burst. Java: render(Screen) */
PUBLIC void smashparticle_render(SmashParticle* this, Screen* screen);

#endif /* SMASHPARTICLE_H */
