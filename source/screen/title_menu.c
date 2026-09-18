/*
 * title_menu.c - The title screen
 *                (Java: com.mojang.ld22.screen.TitleMenu).
 */
#include "title_menu.h"

#include <stdio.h>
#include <string.h>

#include "../game.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "../utils/javalang.h"
#include "../log.h"
#include "../sound/sound.h"
#include "about_menu.h"
#include "instructions_menu.h"
#include "menu.h"


/* Java: `private static final String[] options` */
PRIVATE char start_game[] = "Start game";
PRIVATE char how_to_play[] = "How to play";
PRIVATE char about[] = "About";

/* The same three options, bracketed, for the highlighted row. */
PRIVATE char s_start_game[] = "> Start Game <";
PRIVATE char s_how_to_play[] = "> How to play <";
PRIVATE char s_about[] = "> About <";

PRIVATE char small_help_msg[] = "(Arrow keys, X and C)";

PRIVATE char* titlemenu_options[] = {start_game, how_to_play, about};
PRIVATE char* s_titlemenu_options[] = {s_start_game, s_how_to_play, s_about};

TitleMenu titlemenu;


/* Java: new TitleMenu(). Installs the methods and starts on the first row. */
PUBLIC void titlemenu_create(TitleMenu* this) {
    this->selected = 0;

    this->menu.tick = titlemenu_tick;
    this->menu.render = titlemenu_render;
    this->menu.init = titlemenu_init;
}


/* Java: TitleMenu.init() */
PUBLIC void titlemenu_init(Menu* this) {
    (void) this;

    LOG_TRACE("title menu initialized");
}


/*
 * Java: TitleMenu.render(Screen).
 *
 * Draws the logo band, the three options (highlighted pair on the
 * selection) and the controls hint.
 */
PUBLIC void titlemenu_render(Menu* this, Screen* screen) {
    TitleMenu* title = (TitleMenu*) this;

    screen->clear(screen, 0);

    int h = 2;
    int w = 13;
    int yo = 24;
    int xo = (screen->w - w * 8) / 2;

    int titleColor = get_color4(0, 10, 131, 551);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            screen->render(screen, xo + x * 8, yo + y * 8, x + (y + 6) * 32, titleColor, 0);
        }
    }

    for (int i = 0; i < 3; ++i) {
        char* option = titlemenu_options[i];
        int col = get_color4(0, 222, 222, 222);

        if (i == title->selected) {
            col = get_color4(0, 555, 555, 555);
            option = s_titlemenu_options[i];
        }

        int optionLength = strlen(option);

        font_draw(option, optionLength, screen, (screen->w - optionLength * 8) / 2, (8 + i) * 8, col);
    }

    font_draw(small_help_msg, strlen(small_help_msg), screen, 0, screen->h - 8, get_color4(0, 111, 111, 111));

    LOG_TRACE("title menu render");
}


/*
 * Java: TitleMenu.tick().
 *
 * Moves the selection with up and down; confirming starts the game or opens
 * the instructions and about menus, remembering this one as the parent to
 * return to.
 */
PUBLIC void titlemenu_tick(Menu* this) {
    TitleMenu* title = (TitleMenu*) this;

    if (up.clicked) --title->selected;
    if (down.clicked) ++title->selected;

    if (title->selected < 0) title->selected = 0;
    /* TODO: dynamic size maybe? */
    if (title->selected > 2) title->selected = 2;

    if (attack.clicked || menu.clicked) {
        if (title->selected == 0) {
            sound_play(SND_TEST);   /* Java: Sound.test.play() */
            isingame = 1;
            game_reset();
            game_set_menu(null);
        }

        if (title->selected == 1) {
            menu_parent = &titlemenu.menu;
            game_set_menu(&instructionsmenu);
        }

        if (title->selected == 2) {
            menu_parent = &titlemenu.menu;
            game_set_menu(&aboutmenu);
        }
    }

    LOG_TRACE("title menu tick");
}
