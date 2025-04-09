#ifndef SCREEN_MENU_H_
#define SCREEN_MENU_H_

#include "../gfx/screen.h"
#include "../utils/arraylist.h"

typedef struct{
	void (*tick)(void);
	void (*render)(Screen*);
	void (*init)(void);
} menu_vt;

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

void init_menus();

void init_menu(enum menu_id menu);
void tick_menu(enum menu_id menu);
void render_menu(enum menu_id menu, Screen* screen);

void menu_render_item_list(Screen* screen, int xo, int yo, int x1, int y1, ArrayList* listItems, int selected, void* callback);

#endif /* SCREEN_MENU_H_ */
