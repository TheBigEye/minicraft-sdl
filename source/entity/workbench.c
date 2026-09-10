#include "workbench.h"
#include <stdlib.h>
#include "../game.h"
#include "../screen/crafting_menu.h"
#include "../crafting/crafting.h"
#include "../gfx/color.h"

#include <string.h>


/* The Workbench vtable (= the Java `Workbench` class): Furniture + use(). */
static const EntityVTable workbench_vtable = {
	.tick           = (vt_tick_fn) furniture_tick,
	.render         = (vt_render_fn) furniture_render,
	.blocks         = (vt_blocks_fn) furniture_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = (vt_touchedBy_fn) furniture_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = (vt_use_fn) workbench_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = (vt_free_fn) furniture_free,
};

void workbench_create(Workbench* workbench) {
	char* name = malloc(strlen("Workbench") + 1); //XXX ew
	strcpy(name, "Workbench");

	furniture_create((Furniture *) workbench, name);
	workbench->entity.vt = &workbench_vtable;

	workbench->entity.type = WORKBENCH;
	workbench->col = getColor4(-1, 100, 321, 431);
	workbench->sprite = 4;
	workbench->entity.xr = 3;
	workbench->entity.yr = 2;
}


char workbench_use(Workbench* workbench, struct _Player* player, int attackDir){
	crmenu_recipes = &workbenchRecipes;
	game_set_menu(mid_CRAFTING);
	return 1;
}
