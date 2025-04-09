#include "inventory_menu.h"
#include "../inputhandler.h"
#include "../game.h"
#include "../gfx/font.h"
#include "../item/item.h"
#include "../utils/arraylist.h"

#include <string.h>

const menu_vt inventorymenu_vt = {
	&inventorymenu_tick,
	&inventorymenu_render,
	&inventorymenu_init
};

int inventorymenu_selected;

void inventorymenu_tick(){
	if(menu.clicked) game_set_menu(0);

	if(up.clicked) --inventorymenu_selected;
	if(down.clicked) ++inventorymenu_selected;

	int len = game_player->inventory.items.size;
	if(len == 0) inventorymenu_selected = 0;
	if(inventorymenu_selected < 0) inventorymenu_selected += len;
	if(inventorymenu_selected >= len) inventorymenu_selected -= len;

	if(attack.clicked && len > 0){
		Item* item = arraylist_removeId(&game_player->inventory.items, inventorymenu_selected);
		game_player->activeItem = item;
		game_set_menu(0);
	}
}
void inventorymenu_init(){
	inventorymenu_selected = 0;
	if(game_player->activeItem){
		arraylist_pushTo(&game_player->inventory.items, 0, game_player->activeItem);
		game_player->activeItem = 0;
	}
}

void inventorymenu_render(Screen* screen){
	char inv[] = "inventory";
	font_renderFrame(screen, inv, strlen(inv), 1, 1, 12, 11);
	menu_render_item_list(screen, 1, 1, 12, 11, &game_player->inventory.items, inventorymenu_selected, item_renderInventory);
}
