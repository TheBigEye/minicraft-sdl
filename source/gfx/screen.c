/*
 * screen.c - The core blitter and light-map handling.
 *
 * All game visuals pass through render_screen(): tiles, mobs, items and
 * text are just 8x8 spritesheet tiles blitted into a palette-index
 * buffer, with per-pixel transparency and optional mirroring.
 */
#include "screen.h"

#include <stdlib.h>

/* Mirror flags accepted in render_screen()'s `bits` argument. */
const int BIT_MIRROR_X = 0x01;
const int BIT_MIRROR_Y = 0x02;
/* Bayer-like 4x4 threshold matrix for the darkness dithering. */
const int dither[] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};

/* Allocates the w*h palette-index buffer, cleared to black (index 0). */
void create_screen(Screen* screen, int w, int h, const SpriteSheet* sheet) {
	screen->sheet = sheet;
	screen->w = w;
	screen->h = h;

	screen->pixels = malloc((w * h));

	for(int i = 0; i < w * h; ++i) {
        screen->pixels[i] = 0x000000;
    }

	screen->pixelsSize = w * h;
}


/* Fills every pixel with the low byte of `color` (a palette index). */
void clear_screen(Screen* screen, int color){
	for (int i = 0; i < screen->pixelsSize; ++i) {
		screen->pixels[i] = color & 255;
	}
}


/*
 * Blits the 8x8 sheet tile `tile` at pixel (xp, yp), relative to the
 * screen offset. `bits` may set BIT_MIRROR_X / BIT_MIRROR_Y to flip the
 * sprite. Every sheet pixel is a 2-bit shade (0..3) selecting one of the
 * four palette bytes packed in `colors`; shade byte 255 is transparent
 * and left untouched. Pixels outside the screen bounds are skipped.
 */
void render_screen(Screen* screen, int xp, int yp, int tile, int colors, int bits) {
	xp -= screen->xOffset;
	yp -= screen->yOffset;
	char mirrorX = (bits & BIT_MIRROR_X);
	char mirrorY = (bits & BIT_MIRROR_Y);

	/* Tile coordinates inside the 32x32-tile spritesheet. */
	int xTile = tile % 32;
	int yTile = tile / 32;
	int toffs = xTile * 8 + yTile * 8 * screen->sheet->width;

	for (int y = 0; y < 8; ++y) {
		int ys = y;
		if (mirrorY) ys = 7 - y;
		if (y + yp < 0 || y + yp >= screen->h) continue;

		for (int x = 0; x < 8; ++x) {
			if (x + xp < 0 || x + xp >= screen->w) continue;

			int xs = x;
			if (mirrorX) {
                xs = 7 - x;
            }

			/* Shade 0..3 picks one of the four bytes packed in `colors`. */
			unsigned char col = (colors >> (screen->sheet->pixels[xs + ys * screen->sheet->width + toffs] * 8)) & 255;
			if (col < 255) {
                screen->pixels[(x + xp) + (y + yp) * screen->w] = col;
            }
		}
	}
}


/* Sets the camera offset subtracted from every blit position. */
void screen_set_offset(Screen* screen, int x, int y) {
	screen->xOffset = x;
	screen->yOffset = y;
}


/*
 * Applies the light map (`screen2`) over this screen: where the light
 * value is low, pixels are dithered to black with a 4x4 ordered pattern
 * (shifted by xa/ya so the dither stays put while the camera moves),
 * producing the smooth darkness of caves and night.
 */
void screen_overlay(Screen* screen, Screen* screen2, int xa, int ya){
	int i = 0;
	unsigned char* oPixels = screen2->pixels;

	for (int y = 0; y < screen->h; ++y) {
		for (int x = 0; x < screen->w; ++x) {
			if (oPixels[i] / 10 <= dither[((x + xa) & 3) + ((y + ya) & 3) * 4]){
				screen->pixels[i] = 0;
			}
			++i;
		}
	}
}


/*
 * Adds a circular light of radius r centered at (x, y) to a light-map
 * screen. Brightness falls off with the squared distance to the center
 * and only ever raises the accumulated light value, never lowers it.
 */
void screen_render_light(Screen* screen, int x, int y, int r){
	x -= screen->xOffset;
	y -= screen->yOffset;

	int x0 = x - r;
	int x1 = x + r;
	int y0 = y - r;
	int y1 = y + r;

	/* Clip the bounding box to the screen. */
	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 > screen->w) x1 = screen->w;
	if (y1 > screen->h) y1 = screen->h;

	for (int yy = y0; yy < y1; ++yy) {
		int yd = yy - y;
		yd = yd * yd;
		for (int xx = x0; xx < x1; ++xx) {
			int xd = xx - x;
			int dist = xd * xd + yd;

			if (dist <= r * r) {
				int br = 255 - dist * 255 / (r * r);
				if (screen->pixels[xx + yy * screen->w] < br) {
                    screen->pixels[xx + yy * screen->w] = br;
                }
			}
		}
	}
}


/* Frees the pixel buffer; the Screen struct itself is caller-owned. */
void delete_screen(Screen* screen){
	free(screen->pixels);
}
