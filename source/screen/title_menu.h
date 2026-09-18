/*
 * title_menu.h - The title screen
 *                (Java: com.mojang.ld22.screen.TitleMenu): start the game,
 *                how to play, about.
 */
#ifndef SCREEN_TITLE_MENU_H_
#define SCREEN_TITLE_MENU_H_ 1

#include "../utils/javalang.h"
#include "menu.h"

typedef struct TitleMenu TitleMenu;

struct TitleMenu {
    /* Java: extends Menu */
    Menu menu;
    /* Java: `private int selected` */
    int selected;
};

/* The one title screen; Java builds a new TitleMenu() on every visit. */
extern TitleMenu titlemenu;

/* Constructor: installs the TitleMenu methods. Java: new TitleMenu() */
PUBLIC void titlemenu_create(TitleMenu* this);

/* Java: TitleMenu.init() */
PUBLIC void titlemenu_init(Menu* this);

/* Java: TitleMenu.render(Screen) */
PUBLIC void titlemenu_render(Menu* this, Screen* screen);

/* Java: TitleMenu.tick() */
PUBLIC void titlemenu_tick(Menu* this);

#endif /* SCREEN_TITLE_MENU_H_ */
