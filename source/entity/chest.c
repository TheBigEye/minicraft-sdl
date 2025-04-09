#include "chest.h"
#include "player.h"
#include "inventory.h"
#include "../screen/container_menu.h"
#include "../game.h"
#include "../gfx/color.h"

#include <string.h>


void chest_create(Chest* chest){
	char* name = malloc(strlen("Chest") + 1); //XXX ew
	strcpy(name, "Chest");

	furniture_create((Furniture *) chest, name);

	chest->furniture.entity.type = CHEST;
	chest->furniture.col = getColor4(-1, 110, 331, 552);
	chest->furniture.sprite = 1;

	inventory_create(&chest->inventory);
}


char chest_use(Chest* chest, struct _Player* player, int attackDir){
	strcpy(contmenu_title, "Chest");
	contmenu_container = &chest->inventory;
	game_set_menu(mid_CONTAINER);

	// player.game.setMenu(new ContainerMenu(player, "Chest", inventory));
	// TODO container menu

	return 1;
}


void chest_free(Chest* chest){
	furniture_free((Furniture *) chest);
	inventory_free(&chest->inventory);
}
