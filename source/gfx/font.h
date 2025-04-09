#ifndef GFX_FONT_H_
#define GFX_FONT_H_

#include "screen.h"

extern const char chars[];
extern unsigned char chars_lookup[256];

void font_pre_init();

void font_draw(char* msg, int msgLen, Screen* screen, int x, int y, int col);
void font_renderFrame(Screen* screen, char* title, int titleLength, int x0, int y0, int x1, int y1);


#endif /* GFX_FONT_H_ */
