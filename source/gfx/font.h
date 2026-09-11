/*
 * font.h - 8x8 bitmap font and menu frame rendering.
 *
 * Glyphs live in the spritesheet starting at tile 30. chars[] maps a
 * glyph index to its ASCII character, and chars_lookup[] is the reverse
 * table (ASCII -> glyph index) built once by font_pre_init().
 */
#ifndef GFX_FONT_H_
#define GFX_FONT_H_

#include "screen.h"

/* Printable characters in spritesheet order; entry 0 is a placeholder. */
extern const char chars[];
/* Reverse table: ASCII code -> index in chars[] (0 = not drawable). */
extern unsigned char chars_lookup[256];

/* Builds chars_lookup[]; called once at startup. */
void font_pre_init();

/* Draws `msg` (upper-cased in place) as 8x8 glyphs at pixel (x, y). */
void font_draw(char* msg, int msgLen, Screen* screen, int x, int y, int col);
/* Draws the bordered panel used by menus, with `title` on its top edge. */
void font_renderFrame(Screen* screen, char* title, int titleLength, int x0, int y0, int x1, int y1);


#endif /* GFX_FONT_H_ */
