/*
 * sprite.h - One drawable sprite part.
 *
 * Composite sprites (mobs, furniture, the player) are described as a list
 * of these parts, each pointing at an 8x8 spritesheet tile together with
 * its packed 4-shade colors and its mirror flags.
 *
 * Nothing in the port uses it yet; the renderers blit spritesheet tiles
 * directly. It is kept because it is how the Java original composes its
 * entity sprites, and it is the shape a future port of that part would
 * take.
 */
#ifndef GFX_SPRITE_H_
#define GFX_SPRITE_H_

typedef struct Sprite Sprite;

struct Sprite {
    /* Pixel offset of this part inside the composite sprite. */
    int x;
    int y;
    /* Spritesheet tile index. */
    int img;
    /* Packed 4-shade color value; see color.h. */
    int col;
    /* Mirror flags: BIT_MIRROR_X and BIT_MIRROR_Y. */
    int bits;
};

/* Fills in a sprite part description. */
static inline void create_sprite(Sprite* sprite, int x, int y, int img, int col, int bits) {
    sprite->x    = x;
    sprite->y    = y;
    sprite->img  = img;
    sprite->col  = col;
    sprite->bits = bits;
}

#endif /* GFX_SPRITE_H_ */
