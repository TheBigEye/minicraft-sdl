#include "anvil.h"
#include "player.h"
#include "../game.h"
#include "../crafting/crafting.h"
#include "../screen/crafting_menu.h"
#include "../gfx/color.h"

#include <string.h>

void anvil_create(Anvil* anvil){
	char* name = malloc(strlen("Anvil") + 1); //XXX ew
	strcpy(name, "Anvil");

	furniture_create((Furniture *) anvil, name);

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
