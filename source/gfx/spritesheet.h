/*
 * spritesheet.h - A packed sprite atlas (Java: com.mojang.ld22.gfx.Screen's
 *                 sheet, which the original loads from a PNG).
 *
 * A sheet's pixels are 2-bit shade indices (0..3) computed at build time
 * from the PNG files in assets/ by scripts/spritesheet2c.py, using the same
 * formula as the Java original: (argb & 0xff) / 64.
 *
 * The data lives in read-only memory, embedded in the binary, hence const.
 */
#ifndef GFX_SPRITESHEET_H_
#define GFX_SPRITESHEET_H_

typedef struct SpriteSheet SpriteSheet;

struct SpriteSheet {
    /* Width and height of the whole sheet, in pixels. */
    int width;
    int height;
    /* One 2-bit shade index per pixel; row-major. */
    const unsigned char* pixels;
};

#endif /* GFX_SPRITESHEET_H_ */
