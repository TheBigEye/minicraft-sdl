/*
 * screen.h - Software rendering surface of the game.
 *
 * A Screen is a w*h buffer of palette indices (one byte per pixel).
 * Everything (tiles, sprites, text) is blitted into it from the
 * spritesheet; the finished buffer is later expanded to 32-bit color
 * and scaled up when presented to the SDL window.
 */
#ifndef GFX_SCREEN_H_
#define GFX_SCREEN_H_

#include "spritesheet.h"

/* Flag bit for render_screen(): flip the sprite horizontally. */
extern const int BIT_MIRROR_X;
/* Flag bit for render_screen(): flip the sprite vertically. */
extern const int BIT_MIRROR_Y;
/* 4x4 ordered-dither threshold matrix used by the light overlay. */
extern const int dither[];

typedef struct _Screen{
	int xOffset;   /* camera shift applied to every blit (pixels) */
	int yOffset;

	int w;
	int h;
	unsigned char* pixels;     /* palette-index framebuffer, w*h bytes */
	int pixelsSize;            /* == w*h, cached for the fill loops */

	const SpriteSheet* sheet;  /* sprite source used by render_screen() */
} Screen;


/* Allocates the pixel buffer and attaches the spritesheet. */
void create_screen(Screen* screen, int w, int h, const SpriteSheet* sheet);
/* Fills the whole buffer with one palette index. */
void clear_screen(Screen* screen, int color);
/* Blits an 8x8 sheet tile at (xp, yp) with packed colors and mirror bits. */
void render_screen(Screen* screen, int xp, int yp, int tile, int colors, int bits);
/* Moves the camera: all subsequent blits are shifted by (-x, -y). */
void screen_set_offset(Screen* screen, int x, int y);
/* Dithers the light-map screen onto this screen (darkness overlay). */
void screen_overlay(Screen* screen, Screen* screen2, int xa, int ya);
/* Adds a radial light of radius r at (x, y) to a light-map screen. */
void screen_render_light(Screen* screen, int x, int y, int r);
/* Frees the pixel buffer. */
void delete_screen(Screen* screen);

#endif
