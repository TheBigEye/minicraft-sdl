#ifndef ITEMENTITY_H
#define ITEMENTITY_H
#include "entity.h"
#include "../item/item.h"

typedef struct{
	Entity entity;
	int lifeTime;
	int walkDist;
	int dir;
	int hurtTime;
	int xKnockback, yKnockback;
	double xa, ya, za;
	double xx, yy, zz;
	Item item;
	int time;
} ItemEntity;

void itementity_create(ItemEntity* entity, Item item, int x, int y);
void itementity_render(ItemEntity* item, Screen* screen);
void itementity_tick(ItemEntity* item);
void itementity_touchedBy(ItemEntity* item, Entity* entity);
void itementity_take(ItemEntity* item, Player* player);

#endif // ITEMENTITY_H
