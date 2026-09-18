/*
 * about_menu.h - The about screen
 *                (Java: com.mojang.ld22.screen.AboutMenu): credits text.
 */
#ifndef SCREEN_ABOUT_MENU_H_
#define SCREEN_ABOUT_MENU_H_ 1

#include "../utils/javalang.h"
#include "menu.h"

typedef struct AboutMenu AboutMenu;

struct AboutMenu {
    /* Java: extends Menu */
    Menu menu;
};

/* The one about screen; Java builds a new AboutMenu() on every visit. */
extern AboutMenu aboutmenu;

/* Constructor: installs the AboutMenu methods. Java: new AboutMenu() */
PUBLIC void aboutmenu_create(AboutMenu* this);

/* Java: AboutMenu.tick() */
PUBLIC void aboutmenu_tick(Menu* this);

/* Java: AboutMenu.init() */
PUBLIC void aboutmenu_init(Menu* this);

/* Java: AboutMenu.render(Screen) */
PUBLIC void aboutmenu_render(Menu* this, Screen* screen);

#endif /* SCREEN_ABOUT_MENU_H_ */
