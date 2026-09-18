/*
 * instructions_menu.c - The how-to-play screen
 *                       (Java: com.mojang.ld22.screen.InstructionsMenu).
 */
#include "instructions_menu.h"

#include <string.h>

#include "../game.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "menu.h"


/* The instructions, one line per draw call. */
PRIVATE char s1[] = "HOW TO PLAY";
PRIVATE char s2[] = "Move your character";
PRIVATE char s3[] = "with the arrow keys";
PRIVATE char s4[] = "press C to attack";
PRIVATE char s5[] = "and X to open the";
PRIVATE char s6[] = "inventory and to";
PRIVATE char s7[] = "use items.";
PRIVATE char s8[] = "Select an item in";
PRIVATE char s9[] = "the inventory to";
PRIVATE char s10[] = "equip it.";
PRIVATE char s11[] = "Kill the air wizard";
PRIVATE char s12[] = "to win the game!";

InstructionsMenu instructionsmenu;


/* Java: new InstructionsMenu(). Installs the methods. */
PUBLIC void instructionsmenu_create(InstructionsMenu* this) {
    this->menu.tick = instructionsmenu_tick;
    this->menu.render = instructionsmenu_render;
    this->menu.init = instructionsmenu_init;
}


/* Java: InstructionsMenu.tick(). Any confirm returns to the parent menu. */
PUBLIC void instructionsmenu_tick(Menu* this) {
    (void) this;

    if (attack.clicked || menu.clicked) {
        game_set_menu(menu_parent);
    }
}


/* Java: InstructionsMenu.init() */
PUBLIC void instructionsmenu_init(Menu* this) {
    (void) this;
}


/* Java: InstructionsMenu.render(Screen). Draws the static instruction
 * lines. */
PUBLIC void instructionsmenu_render(Menu* this, Screen* screen) {
    (void) this;

    screen->clear(screen, 0);

    int col = get_color4(0, 333, 333, 333);

    font_draw(s1, strlen(s1), screen, 4 * 8 + 4, 1 * 8, get_color4(0, 555, 555, 555));

    font_draw(s2, strlen(s2), screen, 0 * 8 + 4, 3 * 8, col);
    font_draw(s3, strlen(s3), screen, 0 * 8 + 4, 4 * 8, col);
    font_draw(s4, strlen(s4), screen, 0 * 8 + 4, 5 * 8, col);
    font_draw(s5, strlen(s5), screen, 0 * 8 + 4, 6 * 8, col);
    font_draw(s6, strlen(s6), screen, 0 * 8 + 4, 7 * 8, col);
    font_draw(s7, strlen(s7), screen, 0 * 8 + 4, 8 * 8, col);
    font_draw(s8, strlen(s8), screen, 0 * 8 + 4, 9 * 8, col);
    font_draw(s9, strlen(s9), screen, 0 * 8 + 4, 10 * 8, col);
    font_draw(s10, strlen(s10), screen, 0 * 8 + 4, 11 * 8, col);
    font_draw(s11, strlen(s11), screen, 0 * 8 + 4, 12 * 8, col);
    font_draw(s12, strlen(s12), screen, 0 * 8 + 4, 13 * 8, col);
}
