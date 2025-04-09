#ifndef LEVEL_LEVEL_H_
#define LEVEL_LEVEL_H_ 1

#include "../entity/entity.h"
#include "../gfx/screen.h"
#include "../utils/arraylist.h"
#include "../utils/javarandom.h"


typedef struct _Level{
	Random random;
	int w, h;
	unsigned char* tiles;
	unsigned char* data;
	ArrayList* entitiesInTiles;
	int grassColor;
	int dirtColor;
	int sandColor;
	int depth;
	int monsterDensity;
	ArrayList entities;
} Level;


void level_trySpawn(Level* level, int count);
void level_init(Level* lvl, int w, int h, int level, Level* parent);
unsigned char level_get_tile(Level* level, int x, int y);
unsigned char level_get_data(Level* level, int x, int y);
void level_renderBackground(Level* level, Screen* screen, int xScroll, int yScroll);
void level_tick(Level* level);
void level_set_tile(Level* level, int x, int y, int id, int data);
void level_set_data(Level* level, int x, int y, int val);
void level_renderSprites(Level* level, Screen* screen, int xScroll, int yScroll);
void level_removeEntity(Level* level, int x, int y, Entity* entity);
void level_removeEntity1(Level* level, Entity* e);
void level_addEntity(Level* level, Entity* entity);
void level_insertEntity(Level* level, int x, int y, Entity* entity);
void level_getEntities(Level* level, ArrayList* list, int x0, int y0, int x1, int y1);
void renderLight(Level* level, Screen* screen, int xScroll, int yScroll);

void level_free(Level* lvl);

#endif /* LEVEL_LEVEL_H_ */
