/*
 * instructions_menu.h - The how-to-play screen
 *                       (Java: com.mojang.ld22.screen.InstructionsMenu).
 */
#ifndef SCREEN_INSTRUCTIONS_MENU_H_
#define SCREEN_INSTRUCTIONS_MENU_H_ 1

#include "../utils/javalang.h"
#include "menu.h"

typedef struct InstructionsMenu InstructionsMenu;

struct InstructionsMenu {
    /* Java: extends Menu */
    Menu menu;
};

/* The one how-to-play screen; Java builds a new one on every visit. */
extern InstructionsMenu instructionsmenu;

/* Constructor: installs the methods. Java: new InstructionsMenu() */
PUBLIC void instructionsmenu_create(InstructionsMenu* this);

/* Java: InstructionsMenu.tick() */
PUBLIC void instructionsmenu_tick(Menu* this);

/* Java: InstructionsMenu.init() */
PUBLIC void instructionsmenu_init(Menu* this);

/* Java: InstructionsMenu.render(Screen) */
PUBLIC void instructionsmenu_render(Menu* this, Screen* screen);

#endif /* SCREEN_INSTRUCTIONS_MENU_H_ */
