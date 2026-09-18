/*
 * color.h - Packed 4-shade color values of the original game.
 *
 * Colors are written as 3- or 4-digit decimal literals where each digit is
 * a shade level 0-5 (for example 445, or 550). GETCOLOR turns one such
 * digit triple into an index of the 216-color palette built at startup, and
 * GETCOLOR4 packs four of those indices, one per sprite shade, into a
 * single 32-bit value. Palette index 255 means "transparent".
 */
#ifndef GFX_COLOR_H_
#define GFX_COLOR_H_

#include "../utils/javalang.h"

/* One shade triple -> palette index; negative input means transparent. */
#define GETCOLOR(d) (d < 0 ? 255 : ((d / 100 % 10) * 36 + (d / 10 % 10) * 6 + (d % 10)))

/* Four shade triples -> the 4 bytes picked by sprite shades 0..3. */
#define GETCOLOR4(a, b, c, d) ((GETCOLOR(d) << 24) + (GETCOLOR(c) << 16) + (GETCOLOR(b) << 8) + GETCOLOR(a))

/* Function form of GETCOLOR, for translation units without the macro. */
PUBLIC int get_color(int d);

/* Function form of GETCOLOR4. */
PUBLIC int get_color4(int a, int b, int c, int d);

#endif /* GFX_COLOR_H_ */
