#ifndef GFX_SPRITE_H_
#define GFX_SPRITE_H_

typedef struct{
	int x;
	int y;
	int img;
	int col;
	int bits;
} Sprite;

void create_sprite(Sprite* sprite, int x, int y, int img, int col, int bits){
	sprite->x = x;
	sprite->y = y;
	sprite->img = img;
	sprite->col = col;
	sprite->bits = bits;
}




#endif
