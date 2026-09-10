#include "furniture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "player.h"
#include "../item/item.h"
#include "inventory.h"
#include "../item/furniture_item.h"

#include "anvil.h"
#include "chest.h"
#include "furnace.h"
#include "lantern.h"
#include "oven.h"
#include "workbench.h"

/* Java: Furniture.blocks(Entity e) { return true; } */
char furniture_blocks(Furniture* furniture, Entity* other) {
	(void) furniture; (void) other;
	return 1;
}

/* The Furniture vtable (= the Java `Furniture` class). Subclasses inherit
 * these entries and override `use` / `getLightRadius` / `free` as needed. */
const EntityVTable furniture_vtable = {
	.tick           = (vt_tick_fn) furniture_tick,
	.render         = (vt_render_fn) furniture_render,
	.blocks         = (vt_blocks_fn) furniture_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = (vt_touchedBy_fn) furniture_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = entity_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = (vt_free_fn) furniture_free,
};

char entity_isfurniture(Entity* entity) {
	/* The C equivalent of Java's `e instanceof Furniture` checks. */
	switch (entity->type) {
		case ANVIL:
		case CHEST:
		case FURNACE:
		case LANTERN:
		case OVEN:
		case WORKBENCH:
			return 1;
		default:
			return 0;
	}
}

Furniture* entity_createFurniture(EntityId id) {
	/* Factory: the C equivalent of Java's `new Anvil()`, `new Chest()`, ... */
	Furniture* furn = 0;
	switch (id) {
		case ANVIL:
			furn = malloc(sizeof(Anvil));
			if (furn) anvil_create((Anvil *) furn);
			break;
		case CHEST:
			furn = malloc(sizeof(Chest));
			if (furn) chest_create((Chest *) furn);
			break;
		case FURNACE:
			furn = malloc(sizeof(Furnace));
			if (furn) furnace_create((Furnace *) furn);
			break;
		case LANTERN:
			furn = malloc(sizeof(Lantern));
			if (furn) lantern_create((Lantern *) furn);
			break;
		case OVEN:
			furn = malloc(sizeof(Oven));
			if (furn) oven_create((Oven *) furn);
			break;
		case WORKBENCH:
			furn = malloc(sizeof(Workbench));
			if (furn) workbench_create((Workbench *) furn);
			break;
		default:
			break;
	}

	return furn;
}

Furniture* furniture_create_copy(Furniture* old) {
	/* C-specific helper: deep-copies a furniture entity (chests copy their
	 * inventory). Used when furniture is picked up with the power glove. */
	size_t size;
	switch (old->entity.type) {
		case WORKBENCH: size = sizeof(Workbench); break;
		case LANTERN:   size = sizeof(Lantern);   break;
		case OVEN:      size = sizeof(Oven);      break;
		case FURNACE:   size = sizeof(Furnace);   break;
		case ANVIL:     size = sizeof(Anvil);     break;
		case CHEST:     size = sizeof(Chest);     break;

		default:
			return 0;
	}

	Furniture* copy = malloc(size);
	if (!copy) return 0;
	memcpy(copy, old, size);
	old->name = 0;

	if (old->entity.type == CHEST) {
		Chest* chest = (Chest*) copy;
		Chest* oldc = (Chest*) old;

		inventory_create(&chest->inventory);
		for (int e = 0; e < oldc->inventory.items.size; ++e) {
			Item* itm = oldc->inventory.items.elements[e];
			inventory_addItem(&chest->inventory, itm);

			if (itm->id == FURNITURE) {
				itm->add.furniture.furniture = 0;
			}
		}
	}

	return copy;
}

void furniture_create(Furniture* furniture, char* name){
	entity_create((Entity *) furniture);
	furniture->entity.vt = &furniture_vtable; /* subclasses override with their own vtable */

	furniture->pushTime = 0;
	furniture->pushDir = -1;
	furniture->col = 0;
	furniture->sprite = 0;
	furniture->shouldTake = 0;

	furniture->name = name;
	furniture->entity.xr = 3;
	furniture->entity.yr = 3;
}


void furniture_tick(Furniture* furniture){
	if (furniture->shouldTake){
		Item* item = furniture->shouldTake->activeItem;
		if (item && item->id == POWERGLOVE){
			Furniture* cp = furniture_create_copy(furniture); // XXX ew
			entity_remove((Entity *) furniture);
			inventory_addItemIntoSlot_nalloc(&furniture->shouldTake->inventory, 0, item);
			item = malloc(sizeof(Item));
			furnitureitem_create(item, cp);
			furniture->shouldTake->activeItem = item;
		}

		furniture->shouldTake = 0;
	}

	if (furniture->pushDir == 0) entity_move((Entity *) furniture, 0, 1);
	if (furniture->pushDir == 1) entity_move((Entity *) furniture, 0, -1);
	if (furniture->pushDir == 2) entity_move((Entity *) furniture, -1, 0);
	if (furniture->pushDir == 3) entity_move((Entity *) furniture, 1, 0);

	furniture->pushDir = -1;

	if (furniture->pushTime > 0) --furniture->pushTime;
}


void furniture_render(Furniture* furniture, Screen* screen){
	int x = furniture->entity.x;
	int y = furniture->entity.y;
	int sprite = furniture->sprite;
	int col = furniture->col;

	render_screen(screen, x - 8, y - 8 - 4, sprite * 2 + 8 * 32, col, 0);
	render_screen(screen, x - 0, y - 8 - 4, sprite * 2 + 8 * 32 + 1, col, 0);
	render_screen(screen, x - 8, y - 0 - 4, sprite * 2 + 8 * 32 + 32, col, 0);
	render_screen(screen, x - 0, y - 0 - 4, sprite * 2 + 8 * 32 + 33, col, 0);
}


void furniture_touchedBy(Furniture* furniture, Entity* entity){
	if(entity->type == PLAYER && furniture->pushTime == 0){
		furniture->pushDir = ((Mob*) entity)->dir;
		furniture->pushTime = 10;
	}
}


void furniture_take(Furniture* furniture, Player* player){
	furniture->shouldTake = player;
}


void furniture_free(Furniture* furniture){
	free(furniture->name);
}
