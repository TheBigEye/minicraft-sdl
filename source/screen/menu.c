#include "menu.h"
#include "../utils/arraylist.h"
#include "list_item.h"
#include "../gfx/font.h"
#include "../gfx/screen.h"
#include "../gfx/color.h"

#include "title_menu.h"
#include "about_menu.h"
#include "instructions_menu.h"
#include "inventory_menu.h"
#include "dead_menu.h"
#include "level_transition_menu.h"
#include "crafting_menu.h"
#include "container_menu.h"
#include "won_menu.h"

#include <string.h>
#include <stdio.h>

#define MENUS_SIZE 16

char _next[] = ">";
char _prev[] = "<";

menu_vt* menus[MENUS_SIZE];

enum menu_id menu_parent;

void init_menus() {

	for (int i = 0; i < MENUS_SIZE; ++i) {
		menus[i] = 0;
	}

	menus[mid_TITLE] = &titlemenu_vt;
	menus[mid_ABOUT] = &aboutmenu_vt;
	menus[mid_INSTRUCTIONS] = &instructionsmenu_vt;
	menus[mid_INVENTORY] = &inventorymenu_vt;
	menus[mid_DEAD] = &deadmenu_vt;
	menus[mid_LEVEL_TRANSITION] = &leveltransitionmenu_vt;
	menus[mid_CRAFTING] = &craftingmenu_vt;
	menus[mid_CONTAINER] = &containermenu_vt;
	menus[mid_WON] = &wonmenu_vt;
}


void tick_menu(enum menu_id menu) {
	if (menus[menu]) {
		menus[menu]->tick();
	} else {
		printf("Tried ticking non existent menu(%d)\n", menu);
	}
}


void render_menu(enum menu_id menu, Screen* screen) {
	if (menus[menu]) {
		menus[menu]->render(screen);
	} else {
		printf("Tried rendering non existent menu(%d)\n", menu);
	}
}


void init_menu(enum menu_id menu) {
	if (menus[menu]) {
		menus[menu]->init();
	} else {
		printf("Tried init non existent menu(%d)\n", menu);
	}
}


void menu_render_item_list(Screen* screen, int xo, int yo, int x1, int y1, ArrayList* listItems, int selected, void* callback) {
	char renderCursor = 1;

	if (selected < 0) {
		selected = -selected - 1;
		renderCursor = 0;
	}

	int w = x1 - xo;
	int h = y1 - yo - 1;
	int i0 = 0;
	int i1 = listItems->size;

	if (i1 > h) i1 = h;

	int io = selected - h / 2;

	if (io > listItems->size - h) {
		io = listItems->size - h;
	}

	if (io < 0) io = 0;

	for (int i = i0; i < i1; ++i) {
		void* item = arraylist_get(listItems, i + io);
		((void (*) (void*, Screen*, int, int)) (callback))(item, screen, (1 + xo) * 8, (i + 1 + yo) * 8);
	}

	if (renderCursor) {
		int yy = selected + 1 - io + yo;
		font_draw(">", 1, screen, (xo + 0) * 8, yy * 8, getColor4(5, 555, 555, 555));
		font_draw("<", 1, screen, (xo + w) * 8, yy * 8, getColor4(5, 555, 555, 555));
	}
}
