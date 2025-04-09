#ifndef FURNITURE_H
#define FURNITURE_H
#include "entity.h"

struct _Player;

typedef struct _Furniture{
	Entity entity;
	int pushTime, pushDir;
	int col, sprite;
	char* name;
	struct _Player* shouldTake;
} Furniture;

void furniture_create(Furniture* furniture, char* name);
void furniture_tick(Furniture* furniture);
void furniture_render(Furniture* furniture, Screen* screen);
void furniture_touchedBy(Furniture* furniture, Entity* entity);
void furniture_take(Furniture* furniture, struct _Player* player);
void furniture_free(Furniture* furniture);

#endif // FURNITURE_H
