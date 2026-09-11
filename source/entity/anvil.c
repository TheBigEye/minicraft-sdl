/*
 * anvil.c - Anvil furniture (Java: com.mojang.ld22.entity.Anvil).
 *
 * A placeable stationary entity; using it opens the crafting menu
 * with the anvil recipe list (tool crafting and upgrades).
 */
#include "anvil.h"
#include <stdlib.h>
#include "player.h"
#include "../game.h"
#include "../crafting/crafting.h"
#include "../screen/crafting_menu.h"
#include "../gfx/color.h"

#include <string.h>

/* The Anvil vtable (= the Java `Anvil` class): Furniture + use(). */
static const EntityVTable anvil_vtable = {
	.tick           = (vt_tick_fn) furniture_tick,
	.render         = (vt_render_fn) furniture_render,
	.blocks         = (vt_blocks_fn) furniture_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = (vt_touchedBy_fn) furniture_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = (vt_use_fn) anvil_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = (vt_free_fn) furniture_free,
};

/*
 * Spawns an anvil: furniture named "Anvil", sprite 0, iron palette
 * and the small 3x2 collision box all crafting stations share.
 */
void anvil_create(Anvil* anvil){
	char* name = malloc(strlen("Anvil") + 1); //XXX ew
	strcpy(name, "Anvil");

	furniture_create((Furniture *) anvil, name);
	anvil->entity.vt = &anvil_vtable;

    anvil->entity.type = ANVIL;
	anvil->col = getColor4(-1, 000, 111, 222);
	anvil->sprite = 0;
	anvil->entity.xr = 3;
	anvil->entity.yr = 2;
}

/* Opens the crafting menu on the anvil recipe list; always succeeds. */
char anvil_use(Anvil* anvil, struct _Player* player, int attackDir){
	crmenu_recipes = &anvilRecipes;
	game_set_menu(mid_CRAFTING);
	return 1;
}
