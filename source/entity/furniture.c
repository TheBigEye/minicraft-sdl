/*
 * furniture.c - The Furniture base class
 *               (Java: com.mojang.ld22.entity.Furniture).
 *
 * Placeable stationary entities: crafting stations, chests and lanterns.
 * They block movement, render a single sprite and can be picked back up
 * with the power glove into a furniture item.
 */
#include "furniture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../item/furniture_item.h"
#include "../item/item.h"
#include "anvil.h"
#include "chest.h"
#include "entity.h"
#include "furnace.h"
#include "inventory.h"
#include "lantern.h"
#include "oven.h"
#include "player.h"
#include "workbench.h"


/* Furniture always blocks other entities, as in Java. */
PUBLIC boolean furniture_blocks(Furniture* this, Entity* other) {
    /* Java: Furniture.blocks(Entity e) { return true; } */
    (void) this;
    (void) other;

    return true;
}


/* Class-tag test for the six furniture subclasses. */
PUBLIC boolean entity_is_furniture(Entity* entity) {
    switch (entity->type) {
        case ANVIL:
        case CHEST:
        case FURNACE:
        case LANTERN:
        case OVEN:
        case WORKBENCH:
            return true;
        default:
            return false;
    }
}


/* Factory: the C counterpart of Java's new Anvil(), new Chest() and so on. */
PUBLIC Furniture* entity_create_furniture(EntityId id) {
    Furniture* furn = null;

    switch (id) {
        case ANVIL:
            furn = (Furniture*) new(Anvil);
            if (furn) anvil_create((Anvil*) furn);
            break;
        case CHEST:
            furn = (Furniture*) new(Chest);
            if (furn) chest_create((Chest*) furn);
            break;
        case FURNACE:
            furn = (Furniture*) new(Furnace);
            if (furn) furnace_create((Furnace*) furn);
            break;
        case LANTERN:
            furn = (Furniture*) new(Lantern);
            if (furn) lantern_create((Lantern*) furn);
            break;
        case OVEN:
            furn = (Furniture*) new(Oven);
            if (furn) oven_create((Oven*) furn);
            break;
        case WORKBENCH:
            furn = (Furniture*) new(Workbench);
            if (furn) workbench_create((Workbench*) furn);
            break;
        default:
            break;
    }

    return furn;
}


/*
 * Builds an independent copy of a furniture instance, used when a
 * furniture item is placed back into the world.
 */
PUBLIC Furniture* furniture_create_copy(Furniture* old) {
    size_t size;

    switch (old->entity.type) {
        case WORKBENCH: size = sizeof(Workbench); break;
        case LANTERN:   size = sizeof(Lantern);   break;
        case OVEN:      size = sizeof(Oven);      break;
        case FURNACE:   size = sizeof(Furnace);   break;
        case ANVIL:     size = sizeof(Anvil);     break;
        case CHEST:     size = sizeof(Chest);     break;

        default:
            return null;
    }

    Furniture* copy = malloc(size);

    if (!copy) return null;

    memcpy(copy, old, size);

    /* Ownership of the name moves to the copy, without freeing it. */
    old->name = null;

    if (old->entity.type == CHEST) {
        Chest* chest = (Chest*) copy;
        Chest* oldc = (Chest*) old;

        inventory_create(&chest->inventory);

        for (int e = 0; e < oldc->inventory.items.size; ++e) {
            Item* itm = oldc->inventory.items.elements[e];

            inventory_add(&chest->inventory, itm);

            if (itm->id == FURNITURE) {
                itm->add.furniture.furniture = null;
            }
        }
    }

    return copy;
}


/* Constructor: copies the display name and sets the small bounding box. */
PUBLIC void furniture_create(Furniture* this, String name) {
    entity_create(&this->entity);   /* super() */

    /* What Furniture overrides from Entity.
     * Java: class Furniture extends Entity */
    this->entity.tick       = (entity_tick_fn) furniture_tick;
    this->entity.render     = (entity_render_fn) furniture_render;
    this->entity.blocks     = (entity_blocks_fn) furniture_blocks;
    this->entity.touched_by = (entity_touched_by_fn) furniture_touched_by;
    this->entity.free       = (entity_free_fn) furniture_free;

    this->pushTime = 0;
    this->pushDir = -1;
    this->col = 0;
    this->sprite = 0;
    this->shouldTake = null;

    this->name = name;
    this->entity.xr = 3;
    this->entity.yr = 3;
}


/* Base furniture has no per-tick behaviour of its own. */
PUBLIC void furniture_tick(Furniture* this) {
    if (this->shouldTake) {
        Item* item = this->shouldTake->activeItem;

        if (item && item->id == POWERGLOVE) {
            Furniture* cp = furniture_create_copy(this);   /* XXX ew */
            entity_remove(&this->entity);
            inventory_add_no_copy(&this->shouldTake->inventory, 0, item);

            item = new(Item);
            furnitureitem_create(item, cp);
            this->shouldTake->activeItem = item;
        }

        this->shouldTake = null;
    }

    if (this->pushDir == 0) entity_move(&this->entity, 0, 1);
    if (this->pushDir == 1) entity_move(&this->entity, 0, -1);
    if (this->pushDir == 2) entity_move(&this->entity, -1, 0);
    if (this->pushDir == 3) entity_move(&this->entity, 1, 0);

    this->pushDir = -1;

    if (this->pushTime > 0) --this->pushTime;
}


/* Draws the furniture's single sprite at its position. */
PUBLIC void furniture_render(Furniture* this, Screen* screen) {
    int x = this->entity.x;
    int y = this->entity.y;
    int sprite = this->sprite;
    int col = this->col;

    screen->render(screen, x - 8, y - 8 - 4, sprite * 2 + 8 * 32, col, 0);
    screen->render(screen, x - 0, y - 8 - 4, sprite * 2 + 8 * 32 + 1, col, 0);
    screen->render(screen, x - 8, y - 0 - 4, sprite * 2 + 8 * 32 + 32, col, 0);
    screen->render(screen, x - 0, y - 0 - 4, sprite * 2 + 8 * 32 + 33, col, 0);
}


/* A player walking into it pushes it, in the direction it is facing. */
PUBLIC void furniture_touched_by(Furniture* this, Entity* entity) {
    if (entity->type == PLAYER && this->pushTime == 0) {
        this->pushDir = ((Mob*) entity)->dir;
        this->pushTime = 10;
    }
}


/* Pickup handling when the power glove grabs the furniture. */
PUBLIC void furniture_take(Furniture* this, Player* player) {
    this->shouldTake = player;
}


/* Releases the heap-allocated display name. */
PUBLIC void furniture_free(Furniture* this) {
    delete(this->name);
}
