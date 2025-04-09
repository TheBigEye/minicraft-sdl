#ifndef GFX_SCREEN_H_
#define GFX_SCREEN_H_

#include "spritesheet.h"

extern const int BIT_MIRROR_X;
extern const int BIT_MIRROR_Y;
extern const int dither[];

typedef struct _Screen{
	int xOffset;
	int yOffset;

	int w;
	int h;
	unsigned char* pixels;
	int pixelsSize;

	SpriteSheet* sheet;
} Screen;


void create_screen(Screen* screen, int w, int h, SpriteSheet* sheet);
void clear_screen(Screen* screen, int color);
void render_screen(Screen* screen, int xp, int yp, int tile, int colors, int bits);
void screen_set_offset(Screen* screen, int x, int y);
void screen_overlay(Screen* screen, Screen* screen2, int xa, int ya);
void screen_render_light(Screen* screen, int x, int y, int r);
void delete_screen(Screen* screen);

#endif
