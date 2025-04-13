#include "spritesheet.h"

void create_spritesheet(SpriteSheet* sheet, FILE* data, int width, int height){
	sheet->width = width;
	sheet->height = height;
	sheet->pixels = malloc(width * height);
	fread(sheet->pixels, 1, width * height, data);
}

void delete_spritesheet(SpriteSheet* sheet){
	free(sheet->pixels);
}
