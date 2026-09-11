/*
 * chest.c - Chest furniture (Java: com.mojang.ld22.entity.Chest).
 *
 * Unlike the crafting stations, a chest stores items: it owns an
 * Inventory freed together with the chest, and using it opens the
 * container menu bound to that inventory.
 */
#include "chest.h"
#include <stdlib.h>
#include "player.h"
#include "inventory.h"
#include "../screen/container_menu.h"
#include "../game.h"
#include "../gfx/color.h"

#include <string.h>


/* The Chest vtable (= the Java `Chest` class): Furniture + use() + free(). */
static const EntityVTable chest_vtable = {
	.tick           = (vt_tick_fn) furniture_tick,
	.render         = (vt_render_fn) furniture_render,
	.blocks         = (vt_blocks_fn) furniture_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = (vt_touchedBy_fn) furniture_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = (vt_use_fn) chest_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = (vt_free_fn) chest_free,
};

/* Spawns a chest and creates the empty inventory it stores. */
void chest_create(Chest* chest){
	char* name = malloc(strlen("Chest") + 1); //XXX ew
	strcpy(name, "Chest");

	furniture_create((Furniture *) chest, name);
	chest->furniture.entity.vt = &chest_vtable;

	chest->furniture.entity.type = CHEST;
	chest->furniture.col = getColor4(-1, 110, 331, 552);
	chest->furniture.sprite = 1;

	inventory_create(&chest->inventory);
}


/* Opens the container menu over this chest's own inventory. */
char chest_use(Chest* chest, struct _Player* player, int attackDir){
	strcpy(contmenu_title, "Chest");
	contmenu_container = &chest->inventory;
	game_set_menu(mid_CONTAINER);

	// player.game.setMenu(new ContainerMenu(player, "Chest", inventory));
	// TODO container menu

	return 1;
}


/* Releases the furniture name and the chest inventory. */
void chest_free(Chest* chest){
	furniture_free((Furniture *) chest);
	inventory_free(&chest->inventory);
}
