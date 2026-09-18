/*
 * inventory.h - The inventory: the item container shared by the player
 *               and the chests
 *               (Java: com.mojang.ld22.entity.Inventory).
 */
#ifndef INVENTORY_H
#define INVENTORY_H 1

#include "../utils/javalang.h"
#include "../utils/arraylist.h"
#include "../item/resource/resource.h"

struct Item;
struct Player;

typedef struct Inventory Inventory;

struct Inventory {
    /* What the inventory holds; a list of Item pointers.
     * Java: `public List<Item> items = new ArrayList<Item>();` */
    ArrayList items;
};

/* Constructor: creates the empty item list. Java: Inventory() */
PUBLIC void inventory_create(Inventory* this);

/**
 * Java: `Inventory.add(Item item)` and `Inventory.add(int slot, Item item)`
 * are one overloaded name, so there is one name here as well:
 *
 * so that inventory_add(inv, item) appends at the end (add_2) and
 * inventory_add(inv, slot, item) inserts at that slot (add_3).
 *
 * Both copy the item, because callers usually hand over a temporary;
 * inventory_add_no_copy() is the C-only variant that stores the pointer as
 * Java stores the reference. Resource items merge into an existing stack of
 * the same type instead of taking a new slot.
 */
PUBLIC void inventory_add_2(Inventory* this, struct Item* item);
PUBLIC void inventory_add_3(Inventory* this, int slot, struct Item* item);
#define inventory_add(...)   LANG_OVERLOAD(inventory_add, __VA_ARGS__)

/*
 * C-only: inserts at `slot` without copying, that is, stores the pointer
 * as given. This is the closer match to Java's add(int, Item); the copying
 * overload above exists because C callers often pass stack temporaries.
 */
PUBLIC void inventory_add_no_copy(Inventory* this, int slot, struct Item* item);

/* Java: Inventory.hasResources(Resource, int) */
PUBLIC boolean inventory_has_resources(Inventory* this, Resource* resource, int count);

/* Java: Inventory.removeResource(Resource, int) */
PUBLIC boolean inventory_remove_resource(Inventory* this, Resource* resource, int count);

/*
 * Java: Inventory.count(Item). Stacked count for resources, matching slot
 * count for everything else.
 */
PUBLIC int inventory_count(Inventory* this, struct Item* item);

/* C-only: releases every stored item and then the list storage. */
PUBLIC void inventory_free(Inventory* this);

#endif /* INVENTORY_H */
