#ifndef GFX_SPRITESHEET_H_
#define GFX_SPRITESHEET_H_

#include <stdlib.h>
#include <stdio.h>

typedef struct{
	int width;
	int height;
	char* pixels;
} SpriteSheet;

void create_spritesheet(SpriteSheet* sheet, FILE* data, int width, int height);
void delete_spritesheet(SpriteSheet* sheet);


#endif /* GFX_SPRITESHEET_H_ */
