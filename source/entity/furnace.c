/*
 * furnace.c - Furnace furniture (Java: com.mojang.ld22.entity.Furnace).
 *
 * A placeable stationary entity; using it opens the crafting menu
 * with the furnace recipe list (smelting ore into metal).
 */
#include "furnace.h"
#include <stdlib.h>
#include "../crafting/crafting.h"
#include "../game.h"
#include "../screen/crafting_menu.h"
#include "../gfx/color.h"

#include <string.h>

/* The Furnace vtable (= the Java `Furnace` class): Furniture + use(). */
static const EntityVTable furnace_vtable = {
	.tick           = (vt_tick_fn) furniture_tick,
	.render         = (vt_render_fn) furniture_render,
	.blocks         = (vt_blocks_fn) furniture_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = (vt_touchedBy_fn) furniture_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = (vt_use_fn) furnace_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = (vt_free_fn) furniture_free,
};

/*
 * Spawns a furnace: furniture named "Furnace", sprite 3, stone/fire
 * palette and the small 3x2 collision box of crafting stations.
 */
void furnace_create(Furnace* furnace){
	char* name = malloc(strlen("Furnace") + 1); //XXX ew
	strcpy(name, "Furnace");

	furniture_create((Furniture *) furnace, name);
	furnace->entity.vt = &furnace_vtable;

	furnace->entity.type = FURNACE;
	furnace->col = getColor4(-1, 000, 222, 333);
	furnace->sprite = 3;
	furnace->entity.xr = 3;
	furnace->entity.yr = 2;
}

/* Opens the crafting menu on the furnace recipe list; always succeeds. */
char furnace_use(Furnace* furnace, struct _Player* player, int attackDir){
	crmenu_recipes = &furnaceRecipes;
	game_set_menu(mid_CRAFTING);
	return 1;
}
