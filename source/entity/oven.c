#include "oven.h"
#include <stdlib.h>
#include "furniture.h"
#include "../game.h"
#include "../crafting/crafting.h"
#include "../screen/crafting_menu.h"
#include "../gfx/color.h"

#include <string.h>

/* The Oven vtable (= the Java `Oven` class): Furniture + use(). */
static const EntityVTable oven_vtable = {
	.tick           = (vt_tick_fn) furniture_tick,
	.render         = (vt_render_fn) furniture_render,
	.blocks         = (vt_blocks_fn) furniture_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = (vt_touchedBy_fn) furniture_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = (vt_use_fn) oven_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = (vt_free_fn) furniture_free,
};

void oven_create(Oven* oven){
	char* name = malloc(strlen("Oven") + 1); //XXX ew
	strcpy(name, "Oven");

	furniture_create((Furniture *) oven, name);
	oven->entity.vt = &oven_vtable;

	oven->entity.type = OVEN;
	oven->col = getColor4(-1, 000, 332, 442);
	oven->sprite = 2;
	oven->entity.xr = 3;
	oven->entity.yr = 2;
}

char oven_use(Oven* oven, struct _Player* player, int attackDir){
	crmenu_recipes = &ovenRecipes;
	game_set_menu(mid_CRAFTING);
	return 1;
}
