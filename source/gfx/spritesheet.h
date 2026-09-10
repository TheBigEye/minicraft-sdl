#ifndef GFX_SPRITESHEET_H_
#define GFX_SPRITESHEET_H_

/*
 * A spritesheet's pixels are 2-bit indices (0..3) computed at build time
 * from the PNG files in assets/ by scripts/spritesheet2c.py (same formula
 * as the original Java: (argb & 0xff) / 64).
 * The data lives in read-only memory (embedded in the binary), hence const.
 */
typedef struct{
	int width;
	int height;
	const unsigned char* pixels;
} SpriteSheet;

#endif /* GFX_SPRITESHEET_H_ */
