/*
 * smashparticle.c - The SmashParticle
 *                   (Java: com.mojang.ld22.entity.particle.SmashParticle).
 *
 * A white burst of a 2x2 mirrored tile, living 10 ticks where something
 * was smashed. As in the original, spawning it plays the monsterHurt
 * sound.
 */
#include "smashparticle.h"

#include "../../gfx/color.h"
#include "../../sound/sound.h"


/*
 * Constructor: places the burst at (x, y) with age 0 and plays the smash
 * sound. Java: SmashParticle(int x, int y)
 */
PUBLIC void smashparticle_create(SmashParticle* this, int x, int y) {
    entity_create(&this->entity);   /* super() */

    /* What SmashParticle overrides from Entity.
     * Java: class SmashParticle extends Entity */
    this->entity.tick   = (entity_tick_fn) smashparticle_tick;
    this->entity.render = (entity_render_fn) smashparticle_render;

    this->entity.type = SMASHPARTICLE;

    this->time = 0;
    this->entity.x = x;
    this->entity.y = y;

    /* Sound.monsterHurt.play() */
    sound_play(SND_MONSTERHURT);
}


/* Lives exactly 10 ticks, then marks itself removed. */
PUBLIC void smashparticle_tick(SmashParticle* this) {
    ++this->time;

    if (this->time > 10) {
        entity_remove(&this->entity);
    }
}


/* Draws one sheet tile four times, mirrored, forming a symmetric burst. */
PUBLIC void smashparticle_render(SmashParticle* this, Screen* screen) {
    int col = get_color4(-1, 555, 555, 555);
    int x = this->entity.x;
    int y = this->entity.y;

    screen->render(screen, x - 8, y - 8, 5 + 12 * 32, col, 2);
    screen->render(screen, x - 0, y - 8, 5 + 12 * 32, col, 3);
    screen->render(screen, x - 8, y - 0, 5 + 12 * 32, col, 0);
    screen->render(screen, x - 0, y - 0, 5 + 12 * 32, col, 1);
}
