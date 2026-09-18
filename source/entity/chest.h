/*
 * chest.h - The Chest: a placeable container with an inventory of its
 *           own that persists in the world
 *           (Java: com.mojang.ld22.entity.Chest).
 */
#ifndef CHEST_H
#define CHEST_H 1

#include "furniture.h"
#include "inventory.h"

#include "../utils/javalang.h"

struct Player;

typedef struct Chest Chest;

struct Chest {
    /* Inheritance: Furniture, always the first member. */
    Furniture furniture;
    /* What the chest stores. Java: `public Inventory inventory` */
    Inventory inventory;
};

/* Constructor. Java: Chest() */
PUBLIC void chest_create(Chest* this);

/* Java: Chest.use(Player, int) */
PUBLIC boolean chest_use(Chest* this, struct Player* player, int attackDir);

/* Releases the name and the inventory. Java: Chest's own destructor. */
PUBLIC void chest_free(Chest* this);

#endif /* CHEST_H */
