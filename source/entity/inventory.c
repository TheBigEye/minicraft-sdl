/*
 * inventory.c - The inventory (Java: com.mojang.ld22.entity.Inventory).
 *
 * Stores items in a list; resource items of the same type stack into the
 * existing slot instead of taking a new one.
 */
#include "inventory.h"

#include <stdlib.h>

#include "../item/item.h"
#include "../item/itemids.h"
#include "../utils/arraylist.h"


/* Java: private Inventory.findResource(Resource) */
PRIVATE Item* inventory_find_resource(Inventory* this, Resource* resource) {
    for (int i = 0; i < this->items.size; ++i) {
        Item* item = this->items.elements[i];

        if (item && item->id == RESOURCE) {
            if (item->add.resource.resource == resource) return item;
        }
    }

    return null;
}


/* Constructor: creates the empty item list. Java: Inventory() */
PUBLIC void inventory_create(Inventory* this) {
    arraylist_create(&this->items);
}


/* Java: Inventory.add(Item item) { add(items.size(), item); } */
PUBLIC void inventory_add_2(Inventory* this, Item* item) {
    inventory_add_3(this, this->items.size, item);
}


/*
 * Java: Inventory.add(int slot, Item item). Copies the item first, so that
 * callers may pass a temporary.
 */
PUBLIC void inventory_add_3(Inventory* this, int slot, Item* item) {
    if (item->id == RESOURCE) {
        Item* toTake = item;
        Item* has = inventory_find_resource(this, toTake->add.resource.resource);

        if (!has) {
            Item* add = new(Item);

            *add = *toTake;
            this->items.add_to(&this->items, slot, add);
        } else {
            has->add.resource.count += toTake->add.resource.count;
        }
    } else {
        Item* add = new(Item);

        *add = *item;
        this->items.add_to(&this->items, slot, add);
    }
}


/* C-only: Inventory.add(int slot, Item item) without the copy. */
PUBLIC void inventory_add_no_copy(Inventory* this, int slot, Item* item) {
    if (item->id == RESOURCE) {
        Item* toTake = item;
        Item* has = inventory_find_resource(this, toTake->add.resource.resource);

        if (!has) {
            Item* add = toTake;

            this->items.add_to(&this->items, slot, add);
        } else {
            has->add.resource.count += toTake->add.resource.count;
        }
    } else {
        Item* add = item;

        this->items.add_to(&this->items, slot, add);
    }
}


/* Java: Inventory.hasResources(Resource, int) */
PUBLIC boolean inventory_has_resources(Inventory* this, Resource* resource, int count) {
    Item* ri = inventory_find_resource(this, resource);

    if (ri) return ri->add.resource.count >= count;

    return false;
}


/* Java: Inventory.removeResource(Resource, int) */
PUBLIC boolean inventory_remove_resource(Inventory* this, Resource* resource, int count) {
    Item* ri = inventory_find_resource(this, resource);

    if (!ri) return false;
    if (ri->add.resource.count < count) return false;

    ri->add.resource.count -= count;

    if (ri->add.resource.count <= 0) {
        this->items.remove_element(&this->items, ri);
        item_free(ri);
        delete(ri);
    }

    return true;
}


/* Java: Inventory.count(Item) */
PUBLIC int inventory_count(Inventory* this, Item* item) {
    if (item->id == RESOURCE) {
        Item* ri = inventory_find_resource(this, item->add.resource.resource);

        if (ri) return ri->add.resource.count;
    } else {
        int count = 0;

        for (int i = 0; i < this->items.size; ++i) {
            if (item_matches(this->items.elements[i], item)) ++count;
        }

        return count;
    }

    return 0;
}


/* C-only: releases every stored item and then the list storage. */
PUBLIC void inventory_free(Inventory* this) {
    for (int e = 0; e < this->items.size; ++e) {
        item_free(this->items.elements[e]);
        delete(this->items.elements[e]);
    }

    delete(this->items.elements);
}
