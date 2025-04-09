#ifndef CHEST_H
#define CHEST_H
#include "furniture.h"
#include "inventory.h"

typedef struct _Chest{
	Furniture furniture;
	Inventory inventory;
} Chest;

void chest_create(Chest* chest);
char chest_use(Chest* chest, struct _Player* player, int attackDir);
void chest_free(Chest* chest);

#endif // CHEST_H
