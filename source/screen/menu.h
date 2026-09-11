/*
 * menu.h - Menu framework (Java: Screen): a small vtable registry so
 *          game.c can drive any menu screen through one interface.
 */
#ifndef SCREEN_MENU_H_
#define SCREEN_MENU_H_

#include "../gfx/screen.h"
#include "../utils/arraylist.h"

/* Menu operations shared by every screen implementation. */
typedef struct{
	void (*tick)(void);
	void (*render)(Screen*);
	void (*init)(void);
} menu_vt;

/* Menu to return to when a child screen is dismissed. */
extern enum menu_id menu_parent;

enum menu_id{
	mid_TITLE = 1,
	mid_ABOUT,
	mid_CONTAINER,
	mid_CRAFTING,
	mid_DEAD,
	mid_INSTRUCTIONS,
	mid_LEVEL_TRANSITION,
	mid_WON,
	mid_INVENTORY
};

/* Fills the menu registry with every screen's vtable; call once. */
void init_menus();

void init_menu(enum menu_id menu);
void tick_menu(enum menu_id menu);
void render_menu(enum menu_id menu, Screen* screen);

/* Draws a scrollable list of items with a selection cursor; used by
 * the inventory, crafting and container menus. */
void menu_render_item_list(Screen* screen, int xo, int yo, int x1, int y1, ArrayList* listItems, int selected, void* callback);

#endif /* SCREEN_MENU_H_ */
