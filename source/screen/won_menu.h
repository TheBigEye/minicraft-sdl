/*
 * won_menu.h - The victory screen
 *              (Java: com.mojang.ld22.screen.WonMenu): score summary shown
 *              after beating the air wizard.
 */
#ifndef SCREEN_WON_MENU_H_
#define SCREEN_WON_MENU_H_ 1

#include "../utils/javalang.h"
#include "menu.h"

typedef struct WonMenu WonMenu;

struct WonMenu {
    /* Java: extends Menu */
    Menu menu;
    /* Ticks the screen still ignores input for. Java: `private int inputDelay` */
    int inputDelay;
};

/* The one victory screen; Java builds a new WonMenu() on every win. */
extern WonMenu wonmenu;

/* Constructor: installs the WonMenu methods. Java: new WonMenu() */
PUBLIC void wonmenu_create(WonMenu* this);

/* Java: WonMenu.init() */
PUBLIC void wonmenu_init(Menu* this);

/* Java: WonMenu.render(Screen) */
PUBLIC void wonmenu_render(Menu* this, Screen* screen);

/* Java: WonMenu.tick() */
PUBLIC void wonmenu_tick(Menu* this);

#endif /* SCREEN_WON_MENU_H_ */
