#include "inventory.h"
#include "../utils/arraylist.h"
#include "../item/item.h"
#include "../item/itemids.h"

void inventory_create(Inventory* inv){
	create_arraylist(&inv->items);
}

void inventory_addItem(Inventory* inv, Item* item){
	inventory_addItemIntoSlot(inv, inv->items.size, item);
}

void inventory_addItemIntoSlot_nalloc(Inventory* inv, int slot, Item* item) {
	if(item->id == RESOURCE){
		Item* toTake = item;
		Item* has = inventory_findResource(inv, toTake->add.resource.resource);

		if (!has) {
			Item* add = toTake;
			arraylist_pushTo(&inv->items, slot, add);
		} else {
			has->add.resource.count += toTake->add.resource.count;
		}

	} else {
		Item* add = item;
		arraylist_pushTo(&inv->items, slot, add);
	}
}


void inventory_addItemIntoSlot(Inventory* inv, int slot, Item* item){
	if(item->id == RESOURCE){
		Item* toTake = item;
		Item* has = inventory_findResource(inv, toTake->add.resource.resource);

		if (!has) {
			Item* add = malloc(sizeof(Item));
			*add = *toTake;
			arraylist_pushTo(&inv->items, slot, add);
		} else {
			has->add.resource.count += toTake->add.resource.count;
		}

	} else {
		Item* add = malloc(sizeof(Item));
		*add = *item;
		arraylist_pushTo(&inv->items, slot, add);
	}
}


Item* inventory_findResource(Inventory* inv, Resource* resource){
	for (int i = 0; i < inv->items.size; ++i){
		Item* item = inv->items.elements[i];
		if (item && item->id == RESOURCE){
			if (item->add.resource.resource == resource) return item;
		}
	}
	return 0;
}


uint8_t inventory_hasResources(Inventory* inv, Resource* resource, int count){
	Item* ri = inventory_findResource(inv, resource);
	if (ri) return ri->add.resource.count >= count;
	return 0;
}


uint8_t inventory_removeResource(Inventory* inv, Resource* resource, int count){
	Item* ri = inventory_findResource(inv, resource);

	if (!ri) return 0;
	if (ri->add.resource.count < count) return 0;

	ri->add.resource.count -= count;
	if (ri->add.resource.count <= 0){
		arraylist_removeElement(&inv->items, ri);
		item_free(ri);
		free(ri);
	}

	return 1;
}


int inventory_count(Inventory* inv, Item* item){
	if(item->id == RESOURCE){
		Item* ri = inventory_findResource(inv, item->add.resource.resource);
		if(ri) return ri->add.resource.count;
	}else{
		int count = 0;
		for(int i = 0; i < inv->items.size; ++i){
			if (item_matches(inv->items.elements[i], item)) ++count;
		}
		return count;
	}
	return 0;
}


void inventory_free(Inventory* inv){
	for(int e = 0; e < inv->items.size; ++e){
		item_free(inv->items.elements[e]);
		free(inv->items.elements[e]);
	}
	free(inv->items.elements);
}
