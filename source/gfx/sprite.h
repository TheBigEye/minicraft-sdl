/*
 * sprite.h - One drawable sprite part.
 *
 * Composite sprites (mobs, furniture, the player) are described as a
 * list of these parts, each pointing at an 8x8 spritesheet tile with
 * its packed 4-shade colors and mirror flags.
 */
#ifndef GFX_SPRITE_H_
#define GFX_SPRITE_H_

typedef struct{
	int x;    /* pixel offset of this part inside the composite sprite */
	int y;
	int img;  /* spritesheet tile index */
	int col;  /* packed 4-shade color value (see color.h) */
	int bits; /* mirror flags (BIT_MIRROR_X / BIT_MIRROR_Y) */
} Sprite;

/* Fills in a sprite part description. */
void create_sprite(Sprite* sprite, int x, int y, int img, int col, int bits){
	sprite->x = x;
	sprite->y = y;
	sprite->img = img;
	sprite->col = col;
	sprite->bits = bits;
}




#endif
