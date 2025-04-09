#include "menu.h"
#include "../inputhandler.h"
#include "title_menu.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../game.h"

#include <string.h>

char start_game[] = "Start game";
char how_to_play[] = "How to play";
char about[] = "About";

char s_start_game[] = "> Start Game <";
char s_how_to_play[] = "> How to play <";
char s_about[] = "> About <";

char small_help_msg[] = "(Arrow keys, X and C)";

char* titlemenu_options[] = {start_game, how_to_play, about};
char* s_titlemenu_options[] = {s_start_game, s_how_to_play, s_about};

int titlemenu_selected = 0;

const menu_vt titlemenu_vt = {
	&titlemenu_tick,
	&titlemenu_render,
	&titlemenu_init
};


void titlemenu_init() {
	// printf("Title Menu Initialized!\n");
}


void titlemenu_render(Screen* screen) {
	clear_screen(screen, 0);

	int h = 2;
	int w = 13;
    int yo = 24;
    int xo = (screen->w - w * 8) / 2;
    
	int titleColor = getColor4(0, 10, 131, 551);

	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			render_screen(screen, xo + x * 8, yo + y * 8, x + (y + 6) * 32, titleColor, 0);
		}
	}

	for (int i = 0; i < 3; ++i) {
		char* option = titlemenu_options[i];
		int col = getColor4(0, 222, 222, 222);

		if (i == titlemenu_selected) {
			col = getColor4(0, 555, 555, 555);
			option = s_titlemenu_options[i];
		}

		int optionLength = strlen(option);
		font_draw(option, optionLength, screen, (screen->w - optionLength * 8) / 2, (8 + i) * 8, col);
	}

	font_draw(small_help_msg, strlen(small_help_msg), screen, 0, screen->h - 8, getColor4(0, 111, 111, 111));

	// printf("Title Menu Render!\n");
}


void titlemenu_tick() {
	if (up.clicked) --titlemenu_selected;
	if (down.clicked) ++titlemenu_selected;

	if (titlemenu_selected < 0) titlemenu_selected = 0;
	if (titlemenu_selected > 2) titlemenu_selected = 2; // TODO: dynamic size maybe?

	if (attack.clicked || menu.clicked) {
		if (titlemenu_selected == 0) {
			// TODO: Sound.test.play();
			isingame = 1;
			game_reset();
			game_set_menu(0);
		}

		if (titlemenu_selected == 1) {
			menu_parent = mid_TITLE;
			game_set_menu(mid_INSTRUCTIONS);
		}

		if (titlemenu_selected == 2) {
			menu_parent = mid_TITLE;
			game_set_menu(mid_ABOUT);
		}
	}

	// printf("Title Menu Tick!\n");
}
