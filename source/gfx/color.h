#ifndef GFX_COLOR_H_
#define GFX_COLOR_H_

#define GETCOLOR(d) (d < 0 ? 255 : ((d / 100 % 10)*36 + (d / 10 % 10)*6 + (d % 10)))
#define GETCOLOR4(a, b, c, d) ((GETCOLOR(d) << 24) + (GETCOLOR(c) << 16) + (GETCOLOR(b) << 8) + GETCOLOR(a))

int getColor(int d);

int getColor4(int a, int b, int c, int d);

#endif
