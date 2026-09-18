/*
 * dead_menu.c - The death screen
 *               (Java: com.mojang.ld22.screen.DeadMenu).
 */
#include "dead_menu.h"

#include <stdio.h>
#include <string.h>

#include "../entity/player.h"
#include "../game.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "title_menu.h"
#include "menu.h"


PRIVATE char udiedawman[] = "You died! Aww!";

DeadMenu deadmenu;


/* Java: new DeadMenu(). Installs the methods and arms the input lockout. */
PUBLIC void deadmenu_create(DeadMenu* this) {
    this->inputDelay = 60;

    this->menu.tick = deadmenu_tick;
    this->menu.render = deadmenu_render;
    this->menu.init = deadmenu_init;
}


/* Java: DeadMenu.tick(). Ignores input for the first 60 ticks, then returns
 * to the title. */
PUBLIC void deadmenu_tick(Menu* this) {
    DeadMenu* dead = (DeadMenu*) this;

    if (dead->inputDelay > 0) {
        --dead->inputDelay;
    } else if (attack.clicked || menu.clicked) {
        game_set_menu(&titlemenu);
    }
}


/* Java: DeadMenu.init() */
PUBLIC void deadmenu_init(Menu* this) {
    ((DeadMenu*) this)->inputDelay = 60;
}


/* Java: DeadMenu.render(Screen). Draws the death message with the play time
 * and the score. */
PUBLIC void deadmenu_render(Menu* this, Screen* screen) {
    (void) this;

    char timeString[256];

    font_render_frame(screen, "", 0, 1, 3, 18, 9);
    font_draw(udiedawman, strlen(udiedawman), screen, 2 * 8, 4 * 8, get_color4(-1, 555, 555, 555));

    int seconds = game_gameTime / 60;
    int minutes = seconds / 60;
    int hours = minutes / 60;

    minutes %= 60;
    seconds %= 60;

    if (hours > 0) {
        if (seconds < 10) {
            sprintf(timeString, "%dh0%dm", hours, minutes);
        } else {
            sprintf(timeString, "%dh%dm", hours, minutes);
        }
    } else {
        if (seconds < 10) {
            sprintf(timeString, "%dm 0%ds", minutes, seconds);
        } else {
            sprintf(timeString, "%dm %ds", minutes, seconds);
        }
    }

    char time[] = "Time:";
    char scor[] = "Score:";
    char prec[] = "Press C to lose";
    char score[32];

    sprintf(score, "%d", game_player->score);

    font_draw(time, 5, screen, 2 * 8, 5 * 8, get_color4(-1, 555, 555, 555));
    font_draw(timeString, strlen(timeString), screen, (2 + 5) * 8, 5 * 8, get_color4(-1, 550, 550, 550));
    font_draw(scor, 6, screen, 2 * 8, 6 * 8, get_color4(-1, 555, 555, 555));
    font_draw(score, strlen(score), screen, (2 + 6) * 8, 6 * 8, get_color4(-1, 550, 550, 550));
    font_draw(prec, strlen(prec), screen, 2 * 8, 8 * 8, get_color4(-1, 333, 333, 333));
}
