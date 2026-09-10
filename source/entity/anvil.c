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

char anvil_use(Anvil* anvil, struct _Player* player, int attackDir){
	crmenu_recipes = &anvilRecipes;
	game_set_menu(mid_CRAFTING);
	return 1;
}
