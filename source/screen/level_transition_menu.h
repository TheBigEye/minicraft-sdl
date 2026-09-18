/*
 * level_transition_menu.h - The level transition
 *                           (Java: com.mojang.ld22.screen.LevelTransitionMenu):
 *                           the wipe animation played between depths.
 */
#ifndef SCREEN_LEVEL_TRANSITION_MENU_H_
#define SCREEN_LEVEL_TRANSITION_MENU_H_ 1

#include "../utils/javalang.h"
#include "menu.h"

typedef struct LevelTransitionMenu LevelTransitionMenu;

struct LevelTransitionMenu {
    /* Java: extends Menu */
    Menu menu;
    /* Frames elapsed since the wipe started. Java: `private int time` */
    int time;
    /*
     * Pending depth change, that is, the direction of the stairs.
     * Java: `private int dir`
     */
    int dir;
};

/* The one transition screen; Java builds a new one per stairs use. */
extern LevelTransitionMenu leveltransitionmenu;

/* Constructor: installs the methods. Java: new LevelTransitionMenu(int dir) */
PUBLIC void leveltransitionmenu_create(LevelTransitionMenu* this);

/* Java: LevelTransitionMenu.init() */
PUBLIC void leveltransitionmenu_init(Menu* this);

/* Java: LevelTransitionMenu.render(Screen) */
PUBLIC void leveltransitionmenu_render(Menu* this, Screen* screen);

/* Java: LevelTransitionMenu.tick() */
PUBLIC void leveltransitionmenu_tick(Menu* this);

#endif /* SCREEN_LEVEL_TRANSITION_MENU_H_ */
