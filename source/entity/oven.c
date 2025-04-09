#include "oven.h"
#include "furniture.h"
#include "../game.h"
#include "../crafting/crafting.h"
#include "../screen/crafting_menu.h"
#include "../gfx/color.h"

#include <string.h>

void oven_create(Oven* oven){
	char* name = malloc(strlen("Oven") + 1); //XXX ew
	strcpy(name, "Oven");

	furniture_create((Furniture *) oven, name);

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
