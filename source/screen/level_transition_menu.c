/*
 * level_transition_menu.c - The level transition
 *                           (Java: com.mojang.ld22.screen.LevelTransitionMenu).
 */
#include "level_transition_menu.h"

#include "../game.h"
#include "../gfx/font.h"
#include "../gfx/screen.h"
#include "../inputhandler.h"
#include "menu.h"


LevelTransitionMenu leveltransitionmenu;


/* Java: new LevelTransitionMenu(int dir). Installs the methods; the
 * direction is picked up from the game in init(), as the original does. */
PUBLIC void leveltransitionmenu_create(LevelTransitionMenu* this) {
    this->time = 0;
    this->dir = 0;

    this->menu.tick = leveltransitionmenu_tick;
    this->menu.render = leveltransitionmenu_render;
    this->menu.init = leveltransitionmenu_init;
}


/* Java: LevelTransitionMenu.init() */
PUBLIC void leveltransitionmenu_init(Menu* this) {
    LevelTransitionMenu* lt = (LevelTransitionMenu*) this;

    lt->time = 0;
    lt->dir = game_pendingLevelChange;
}


/* Java: LevelTransitionMenu.render(Screen). Draws a diagonal wipe of black
 * tiles sweeping across the screen. */
PUBLIC void leveltransitionmenu_render(Menu* this, Screen* screen) {
    LevelTransitionMenu* lt = (LevelTransitionMenu*) this;

    for (int x = 0; x < 40; ++x) {
        for (int y = 0; y < 30; ++y) {
            int dd = (y + x % 2 * 2 + x / 3) - lt->time;

            if (dd < 0 && dd > -30) {
                screen->render(screen, x * 8, y * 8, 0, 0, 0);
            } else {
                screen->render(screen, x * 8, screen->h - y * 8 - 9, 0, 0, 0);
            }
        }
    }
}


/* Java: LevelTransitionMenu.tick(). Swaps the level at the halfway point and
 * closes at the end. */
PUBLIC void leveltransitionmenu_tick(Menu* this) {
    LevelTransitionMenu* lt = (LevelTransitionMenu*) this;

    lt->time += 2;

    if (lt->time == 30) game_change_level(lt->dir);
    if (lt->time == 60) game_set_menu(null);
}
