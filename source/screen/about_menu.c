/*
 * about_menu.c - The about screen
 *                (Java: com.mojang.ld22.screen.AboutMenu).
 */
#include "about_menu.h"

#include <string.h>

#include "../game.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "menu.h"


/* The credits, one line per draw call. */
PRIVATE char s1[] = "About Minicraft";
PRIVATE char s2[] = "Minicraft was made";
PRIVATE char s3[] = "by Markus Persson";
PRIVATE char s4[] = "For the 22'nd ludum";
PRIVATE char s5[] = "dare competition in";
PRIVATE char s6[] = "december 2011.";
PRIVATE char s7[] = "it is dedicated to";
PRIVATE char s8[] = "my father. <3";

AboutMenu aboutmenu;


/* Java: new AboutMenu(). Installs the methods. */
PUBLIC void aboutmenu_create(AboutMenu* this) {
    this->menu.tick = aboutmenu_tick;
    this->menu.render = aboutmenu_render;
    this->menu.init = aboutmenu_init;
}


/* Java: AboutMenu.tick(). Any confirm returns to the parent menu. */
PUBLIC void aboutmenu_tick(Menu* this) {
    (void) this;

    if (attack.clicked || menu.clicked) {
        game_set_menu(menu_parent);
    }
}


/* Java: AboutMenu.init() */
PUBLIC void aboutmenu_init(Menu* this) {
    (void) this;
}


/* Java: AboutMenu.render(Screen). Draws the static credits lines. */
PUBLIC void aboutmenu_render(Menu* this, Screen* screen) {
    (void) this;

    screen->clear(screen, 0);

    int col = get_color4(0, 333, 333, 333);

    font_draw(s1, strlen(s1), screen, 2 * 8 + 4, 1 * 8, get_color4(0, 555, 555, 555));

    font_draw(s2, strlen(s2), screen, 0 * 8 + 4, 3 * 8, col);
    font_draw(s3, strlen(s3), screen, 0 * 8 + 4, 4 * 8, col);
    font_draw(s4, strlen(s4), screen, 0 * 8 + 4, 5 * 8, col);
    font_draw(s5, strlen(s5), screen, 0 * 8 + 4, 6 * 8, col);
    font_draw(s6, strlen(s6), screen, 0 * 8 + 4, 7 * 8, col);
    font_draw(s7, strlen(s7), screen, 0 * 8 + 4, 9 * 8, col);
    font_draw(s8, strlen(s8), screen, 0 * 8 + 4, 10 * 8, col);
}
