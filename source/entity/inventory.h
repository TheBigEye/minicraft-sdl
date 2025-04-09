#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdint.h>
#include "../utils/arraylist.h"
#include "../item/resource/resource.h"

typedef struct _Inventory{
	ArrayList items;
} Inventory;

struct _Item;

void inventory_create(Inventory* inv);
void inventory_addItem(Inventory* inv, struct _Item* item);
void inventory_addItemIntoSlot(Inventory* inv, int slot, struct _Item* item);
struct _Item* inventory_findResource(Inventory* inv, Resource* resource);
uint8_t inventory_hasResources(Inventory* inv, Resource* resource, int count);
uint8_t inventory_removeResource(Inventory* inv, Resource* resource, int count);
int inventory_count(Inventory* inv, struct _Item* item);
void inventory_free(Inventory* inv);

void inventory_addItemIntoSlot_nalloc(Inventory* inv, int slot, struct _Item* item);

#endif // INVENTORY_H
