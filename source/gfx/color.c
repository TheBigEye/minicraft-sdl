/*
 * color.c - Function wrappers over the GETCOLOR/GETCOLOR4 macros so any
 *           translation unit can pack colors without including color.h
 *           internals.
 */
#include "color.h"

/* Palette index for one shade triple (macro wrapped as a function). */
inline int getColor(int d) {
	return GETCOLOR(d);
}

/* Packed 4-shade color value (macro wrapped as a function). */
inline int getColor4(int a, int b, int c, int d) {
	return GETCOLOR4(a, b, c, d);
}
