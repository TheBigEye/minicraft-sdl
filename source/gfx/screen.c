/*
 * screen.c - Implementation of the Screen class: the game's blitter.
 *
 * Every pixel goes through render(): tiles, mobs, items and text are all
 * 8x8 spritesheet tiles blitted into a buffer of palette indices, with
 * per-pixel transparency and optional mirroring.
 */
#include "screen.h"

#include "../log.h"

#include <stdlib.h>

/* The 4x4 threshold matrix used to dither darkness. */
const int dither[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};


/* Constructor: installs the methods, allocates the buffer and clears it. */
PUBLIC void screen_create(Screen* this, int w, int h, const SpriteSheet* sheet) {
    /*
     * The pixel buffer is w * h bytes. The dimensions are checked first:
     * `int * int` is not provably positive, and casting a negative product
     * to size_t trips the -Walloc-size-larger-than= that GCC enables by
     * default. See level_create() for the longer version of this note.
     */
    if (w <= 0 || h <= 0) {
        LOG_ERROR("screen_create: invalid dimensions %dx%d", w, h);
        return;
    }

    int pixels_count = w * h;

    if (pixels_count <= 0) {
        LOG_ERROR("screen_create: %dx%d overflows the pixel count", w, h);
        return;
    }

    this->render       = screen_render;
    this->clear        = screen_clear;
    this->set_offset   = screen_set_offset;
    this->overlay      = screen_overlay;
    this->render_light = screen_render_light;
    this->free         = screen_free;

    this->sheet = sheet;
    this->w     = w;
    this->h     = h;

    this->pixels = new_array(unsigned char, pixels_count);

    this->pixelsSize = pixels_count;
    this->xOffset    = 0;
    this->yOffset    = 0;
}


/*
 * Blits one sprite.
 *
 * `bits` may carry BIT_MIRROR_X / BIT_MIRROR_Y to flip the sprite. Each
 * spritesheet pixel is a 2-bit shade (0..3) that selects one of the four
 * palette bytes packed into `colors`; shade 255 is transparent and leaves
 * the destination alone. Pixels falling outside the screen are dropped.
 */
PUBLIC void screen_render(Screen* this, int xp, int yp, int tile, int colors, int bits) {
    xp -= this->xOffset;
    yp -= this->yOffset;

    boolean mirrorX = bits & BIT_MIRROR_X;
    boolean mirrorY = bits & BIT_MIRROR_Y;

    /* Coordinates of the sprite inside the 32x32 tile sheet. */
    int xTile = tile % 32;
    int yTile = tile / 32;
    int toffs = xTile * 8 + yTile * 8 * this->sheet->width;

    for (int y = 0; y < 8; ++y) {
        int ys = mirrorY ? 7 - y : y;

        if (y + yp < 0 || y + yp >= this->h) continue;

        for (int x = 0; x < 8; ++x) {
            if (x + xp < 0 || x + xp >= this->w) continue;

            int xs = mirrorX ? 7 - x : x;

            unsigned char col = (colors >> (this->sheet->pixels[xs + ys * this->sheet->width + toffs] * 8)) & 255;

            if (col < 255) {
                this->pixels[(x + xp) + (y + yp) * this->w] = col;
            }
        }
    }
}


/* Fills the whole buffer with the low byte of `color`, a palette index. */
PUBLIC void screen_clear(Screen* this, int color) {
    for (int i = 0; i < this->pixelsSize; ++i) {
        this->pixels[i] = color & 255;
    }
}


/* Sets the camera offset that is subtracted from every blit. */
PUBLIC void screen_set_offset(Screen* this, int x, int y) {
    this->xOffset = x;
    this->yOffset = y;
}


/*
 * Applies the light map (`other`) on top of this screen: wherever the light
 * is low, pixels are dithered to black with an ordered 4x4 pattern, shifted
 * by xa/ya so the dither does not crawl as the camera moves. This is what
 * gives caves their soft darkness.
 */
PUBLIC void screen_overlay(Screen* this, Screen* other, int xa, int ya) {
    int i = 0;
    unsigned char* oPixels = other->pixels;

    for (int y = 0; y < this->h; ++y) {
        for (int x = 0; x < this->w; ++x) {
            if (oPixels[i] / 10 <= dither[((x + xa) & 3) + ((y + ya) & 3) * 4]) {
                this->pixels[i] = 0;
            }

            ++i;
        }
    }
}


/*
 * Adds a circular light of radius r centred on (x, y) to a light map.
 * Brightness falls off with the square of the distance, and light only ever
 * accumulates: it never lowers a value that is already there.
 */
PUBLIC void screen_render_light(Screen* this, int x, int y, int r) {
    x -= this->xOffset;
    y -= this->yOffset;

    int x0 = x - r;
    int x1 = x + r;
    int y0 = y - r;
    int y1 = y + r;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > this->w) x1 = this->w;
    if (y1 > this->h) y1 = this->h;

    for (int yy = y0; yy < y1; ++yy) {
        int yd = yy - y;
        yd = yd * yd;

        for (int xx = x0; xx < x1; ++xx) {
            int xd   = xx - x;
            int dist = xd * xd + yd;

            if (dist <= r * r) {
                int br = 255 - dist * 255 / (r * r);

                if (this->pixels[xx + yy * this->w] < br) {
                    this->pixels[xx + yy * this->w] = br;
                }
            }
        }
    }
}


/* Destructor: frees the pixel buffer. The struct itself belongs to the caller. */
PUBLIC void screen_free(Screen* this) {
    free(this->pixels);
}
