/*
 * screen.h - The Screen class (Java: com.mojang.ld22.gfx.Screen).
 *
 * The game's software drawing surface: a buffer of w*h palette indices, one
 * byte per pixel. Everything (tiles, mobs, items and text) is blitted here
 * from the spritesheet; the finished buffer is expanded to 32-bit color and
 * scaled when it is presented in the SDL window.
 *
 * The struct stays visible in the header because the game holds two screens
 * by value (game_screen and game_lightScreen), not through pointers.
 *
 * Methods, as in the original:
 *
 *     screen->render(screen, xp, yp, tile, colors, bits);
 *     screen->clear(screen, color);
 */
#ifndef GFX_SCREEN_H_
#define GFX_SCREEN_H_

#include "spritesheet.h"
#include "../utils/javalang.h"

/* Mirror flags accepted by the `bits` argument of render(). */
#define BIT_MIRROR_X 0x01
#define BIT_MIRROR_Y 0x02

/*
 * The 4x4 Bayer threshold matrix used to dither darkness. In Java this was
 * a static field of Screen.
 */
extern const int dither[16];

typedef struct Screen Screen;

/* Signatures of the Screen methods. */
typedef void (*screen_render_fn)      (Screen* this, int xp, int yp, int tile, int colors, int bits);
typedef void (*screen_clear_fn)       (Screen* this, int color);
typedef void (*screen_set_offset_fn)  (Screen* this, int x, int y);
typedef void (*screen_overlay_fn)     (Screen* this, Screen* other, int xa, int ya);
typedef void (*screen_render_light_fn)(Screen* this, int x, int y, int r);
typedef void (*screen_free_fn)        (Screen* this);

struct Screen {
    /* --- methods, installed by screen_create() --- */

    /* Blits one 8x8 sprite. Java: render(int, int, int, int, int) */
    screen_render_fn render;
    /* Fills the whole buffer with one color. Java: clear(int) */
    screen_clear_fn clear;
    /* Moves the camera: every blit is shifted by (-x, -y). Java: setOffset */
    screen_set_offset_fn set_offset;
    /* Applies the light map on top, with dithering. Java: overlay */
    screen_overlay_fn overlay;
    /* Adds a circular light to the light map. Java: renderLight */
    screen_render_light_fn render_light;
    /* C destructor: frees the pixel buffer. */
    screen_free_fn free;

    /* --- data --- */

    /* Camera offset subtracted from every blit. */
    int xOffset;
    int yOffset;
    int w;
    int h;
    /* Palette indices, w*h bytes. */
    unsigned char* pixels;
    /* Cached w*h, so the loops do not recompute it. */
    int pixelsSize;
    /* The spritesheet render() blits from. */
    const SpriteSheet* sheet;
};

/* Constructor: allocates the buffer and binds the sheet. Java: Screen(w, h, sheet) */
PUBLIC void screen_create(Screen* this, int w, int h, const SpriteSheet* sheet);

/* The method implementations. */
PUBLIC void screen_render(Screen* this, int xp, int yp, int tile, int colors, int bits);
PUBLIC void screen_clear(Screen* this, int color);
PUBLIC void screen_set_offset(Screen* this, int x, int y);
PUBLIC void screen_overlay(Screen* this, Screen* other, int xa, int ya);
PUBLIC void screen_render_light(Screen* this, int x, int y, int r);
PUBLIC void screen_free(Screen* this);

#endif /* GFX_SCREEN_H_ */
