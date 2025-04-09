#include "../inputhandler.h"
#include "../game.h"
#include "../gfx/font.h"
#include "../gfx/color.h"
#include "dead_menu.h"

#include <string.h>

const menu_vt deadmenu_vt = {
	&deadmenu_tick,
	&deadmenu_render,
	&deadmenu_init
};

int deadmenu_inputDelay = 60;

void deadmenu_tick(){
	if(deadmenu_inputDelay > 0){
		--deadmenu_inputDelay;
	}else if(attack.clicked || menu.clicked){
		game_set_menu(mid_TITLE);
	}
}
void deadmenu_init(){
	deadmenu_inputDelay = 60;
}

static char udiedawman[] = "You died! Aww!";

void deadmenu_render(Screen* screen){
	char vd[] = "";
	char timeString[256];

	font_renderFrame(screen, vd, 0, 1, 3, 18, 9);
	font_draw(udiedawman, strlen(udiedawman), screen, 2 * 8, 4 * 8, getColor4(-1, 555, 555, 555));

	int seconds = game_gameTime / 60;
	int minutes = seconds / 60;
	int hours = minutes / 60;
	minutes %= 60;
	seconds %= 60;

	if (hours > 0) {

		if(seconds < 10){
			sprintf(timeString, "%dh0%dm\00", hours, minutes);
		}else{
			sprintf(timeString, "%dh%dm\00", hours, minutes);
		}
	} else {
		if(seconds < 10){
			sprintf(timeString, "%dm 0%ds\00", minutes, seconds);
		}else{
			sprintf(timeString, "%dm %ds\00", minutes, seconds);
		}
	}
	char time[] = "Time:";
	char scor[] = "Score:";
	char prec[] = "Press C to lose";
	char score[32];
	sprintf(score, "%d\00", game_player->score);

	font_draw(time, 5, screen, 2 * 8, 5 * 8, getColor4(-1, 555, 555, 555));
	font_draw(timeString, strlen(timeString), screen, (2 + 5) * 8, 5 * 8, getColor4(-1, 550, 550, 550));
	font_draw(scor, 6, screen, 2 * 8, 6 * 8, getColor4(-1, 555, 555, 555));
	font_draw(score, strlen(score), screen, (2 + 6) * 8, 6 * 8, getColor4(-1, 550, 550, 550));
	font_draw(prec, strlen(prec), screen, 2 * 8, 8 * 8, getColor4(-1, 333, 333, 333));
}
