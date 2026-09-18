/*
 * dead_menu.h - The death screen
 *               (Java: com.mojang.ld22.screen.DeadMenu): score summary
 *               with a short input lockout.
 */
#ifndef SCREEN_DEAD_MENU_H_
#define SCREEN_DEAD_MENU_H_ 1

#include "../utils/javalang.h"
#include "menu.h"

typedef struct DeadMenu DeadMenu;

struct DeadMenu {
    /* Java: extends Menu */
    Menu menu;
    /* Ticks the screen still ignores input for. Java: `private int inputDelay` */
    int inputDelay;
};

/* The one death screen; Java builds a new DeadMenu() on every death. */
extern DeadMenu deadmenu;

/* Constructor: installs the DeadMenu methods. Java: new DeadMenu() */
PUBLIC void deadmenu_create(DeadMenu* this);

/* Java: DeadMenu.tick() */
PUBLIC void deadmenu_tick(Menu* this);

/* Java: DeadMenu.init() */
PUBLIC void deadmenu_init(Menu* this);

/* Java: DeadMenu.render(Screen) */
PUBLIC void deadmenu_render(Menu* this, Screen* screen);

#endif /* SCREEN_DEAD_MENU_H_ */
