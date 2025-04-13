#include "workbench.h"
#include "../game.h"
#include "../screen/crafting_menu.h"
#include "../crafting/crafting.h"
#include "../gfx/color.h"

#include <string.h>


void workbench_create(Workbench* workbench) {
	char* name = malloc(strlen("Workbench") + 1); //XXX ew
	strcpy(name, "Workbench");

	furniture_create((Furniture *) workbench, name);

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
