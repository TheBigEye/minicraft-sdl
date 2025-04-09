#include "furnace.h"
#include "../crafting/crafting.h"
#include "../game.h"
#include "../screen/crafting_menu.h"
#include "../gfx/color.h"

#include <string.h>

void furnace_create(Furnace* furnace){
	char* name = malloc(strlen("Furnace") + 1); //XXX ew
	strcpy(name, "Furnace");

	furniture_create((Furniture *) furnace, name);

	furnace->entity.type = FURNACE;
	furnace->col = getColor4(-1, 000, 222, 333);
	furnace->sprite = 3;
	furnace->entity.xr = 3;
	furnace->entity.yr = 2;
}

char furnace_use(Furnace* furnace, struct _Player* player, int attackDir){
	crmenu_recipes = &furnaceRecipes;
	game_set_menu(mid_CRAFTING);
	return 1;
}
