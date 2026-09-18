/*
 * font.c - Text and menu-frame rendering on top of the sprite blitter.
 */
#include "font.h"

#include <string.h>

#include "../utils/utils.h"
#include "color.h"
#include "screen.h"

/* Characters in glyph-sheet order; the first entry is never drawn. */
const char chars[] = "AABCDEFGHIJKLMNOPQRSTUVWXYZ      0123456789.,!?'\"-+=/\\%()<>:;     ";

/* ASCII -> glyph index, filled in by font_pre_init(). */
unsigned char chars_lookup[256];


/* Fills the reverse lookup table, so drawing needs no linear search. */
PUBLIC void font_pre_init() {
    size_t d = strlen(chars);

    for (size_t i = d - 1; i > 0; --i) {
        chars_lookup[(int) chars[i]] = (unsigned char) i;
    }

    chars_lookup[0] = 0;
}


/*
 * Draws `msg` at pixel (x, y) in color `col`. The message is upper-cased in
 * place, because the sheet only holds capitals; each character blits the
 * 8x8 glyph at sheet tile 30 + index. Characters without a glyph are
 * skipped, leaving a blank space.
 */
PUBLIC void font_draw(char* msg, int msgLen, Screen* screen, int x, int y, int col) {
    str_to_upper(msg, msgLen);

    for (int i = 0; i < msgLen; ++i) {
        int ix = chars_lookup[(unsigned char) msg[i]];

        if (ix > 0) {
            screen->render(screen, x + (i * 8), y, (ix - 1) + 30 * 32, col, 0);
        }
    }
}


/*
 * Draws the framed panel used by menus and dialogs: the four corner tiles,
 * the edge tiles between them and a flat interior fill, all in the menu's
 * gray palette. The title is then drawn along the top edge.
 * (x0, y0)-(x1, y1) are the tile coordinates of the frame rectangle.
 */
PUBLIC void font_render_frame(Screen* screen, char* title, int titleLength, int x0, int y0, int x1, int y1) {
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            if (x == x0 && y == y0) {
                screen->render(screen, (x * 8), (y * 8), 0 + 13 * 32, get_color4(-1, 1, 5, 445), 0);

            } else if (x == x1 && y == y0) {
                screen->render(screen, (x * 8), (y * 8), 0 + 13 * 32, get_color4(-1, 1, 5, 445), 1);

            } else if (x == x0 && y == y1) {
                screen->render(screen, (x * 8), (y * 8), 0 + 13 * 32, get_color4(-1, 1, 5, 445), 2);

            } else if (x == x1 && y == y1) {
                screen->render(screen, (x * 8), (y * 8), 0 + 13 * 32, get_color4(-1, 1, 5, 445), 3);

            } else if (y == y0) {
                screen->render(screen, (x * 8), (y * 8), 1 + 13 * 32, get_color4(-1, 1, 5, 445), 0);

            } else if (y == y1) {
                screen->render(screen, (x * 8), (y * 8), 1 + 13 * 32, get_color4(-1, 1, 5, 445), 2);

            } else if (x == x0) {
                screen->render(screen, (x * 8), (y * 8), 2 + 13 * 32, get_color4(-1, 1, 5, 445), 0);

            } else if (x == x1) {
                screen->render(screen, (x * 8), (y * 8), 2 + 13 * 32, get_color4(-1, 1, 5, 445), 1);

            } else {
                screen->render(screen, (x * 8), (y * 8), 2 + 13 * 32, get_color4(5, 5, 5, 5), 1);
            }
        }
    }

    font_draw(title, titleLength, screen, (x0 * 8) + 8, (y0 * 8), get_color4(5, 5, 5, 550));
}
