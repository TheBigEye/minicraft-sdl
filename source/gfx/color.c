/*
 * color.c - Function wrappers over the GETCOLOR and GETCOLOR4 macros, so
 *           any translation unit can pack colors without having the macro
 *           in scope.
 *
 * They are deliberately not `inline`: a plain `inline` definition in a .c
 * file carries no external definition under C99, so the callers in other
 * translation units would only link by luck.
 */
#include "color.h"

/* Palette index for one shade triple (the macro, wrapped as a function). */
PUBLIC int get_color(int d) {
    return GETCOLOR(d);
}

/* Packed 4-shade color value (the macro, wrapped as a function). */
PUBLIC int get_color4(int a, int b, int c, int d) {
    return GETCOLOR4(a, b, c, d);
}
