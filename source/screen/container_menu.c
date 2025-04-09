#include "menu.h"
#include "container_menu.h"
#include "../game.h"
#include "../inputhandler.h"
#include "../entity/inventory.h"
#include "../gfx/font.h"
#include "../item/item.h"

#include <string.h>

const menu_vt containermenu_vt = {
	&containermenu_tick,
	&containermenu_render,
	&containermenu_init
};

static int window = 0;
static int oSelected = 0;
static int selected = 0;

Inventory* contmenu_container = 0;
char contmenu_title[64] = {0};

void containermenu_tick(){
	if(menu.clicked) game_set_menu(0);

	if(left.clicked){
		window = 0;
		int tmp = selected;
		selected = oSelected;
		oSelected = tmp;
	}

	if(right.clicked){
		window = 1;
		int tmp = selected;
		selected = oSelected;
		oSelected = tmp;
	}

	Inventory* i = window == 1 ? &game_player->inventory : contmenu_container;
	Inventory* i2 = window == 0 ? &game_player->inventory : contmenu_container;

	int len = i->items.size;
	if(selected < 0) selected = 0;
	if(selected >= len) selected = len-1;

	if(up.clicked) --selected;
	if(down.clicked) ++selected;

	if(len == 0) selected = 0;
	if(selected < 0) selected += len;
	if(selected >= len) selected -= len;

	if(attack.clicked && len > 0){
		Item* item = arraylist_removeId(&i->items, selected);
		inventory_addItemIntoSlot(i2, oSelected, item);


		free(item);

		if(selected >= i->items.size) selected = i->items.size - 1;
	}
}
void containermenu_init(){
	window = oSelected = selected = 0;
}
void containermenu_render(Screen* screen){
	if(window == 1) screen_set_offset(screen, 6*8, 0);
	font_renderFrame(screen, contmenu_title, strlen(contmenu_title), 1, 1, 12, 11);
	menu_render_item_list(screen, 1, 1, 12, 11, &contmenu_container->items, window == 0 ? selected : -oSelected - 1, item_renderInventory);
	char s[] = "inventory";
	font_renderFrame(screen, s, strlen(s), 13, 1, 13+11, 11);
	menu_render_item_list(screen, 13, 1, 13+11, 11, &game_player->inventory.items, window == 1 ? selected : -oSelected - 1, item_renderInventory);
	screen_set_offset(screen, 0, 0);
}
