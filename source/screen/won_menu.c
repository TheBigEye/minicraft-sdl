#include "won_menu.h"
#include "../gfx/screen.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "../game.h"

#include <string.h>

const menu_vt wonmenu_vt = {
	wonmenu_tick,
	wonmenu_render,
	wonmenu_init
};

static int inputDelay = 60;

void wonmenu_init(){
	inputDelay = 60;
}
void wonmenu_render(Screen* screen){
	char msg[] = "You won! Yay!";
	char timeString[256];
	font_renderFrame(screen, "", 0, 1, 3, 18, 9);
	font_draw(msg, strlen(msg), screen, 2*8, 4*8, getColor4(-1, 555, 555, 555));

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
	char prec[] = "Press C to win";
	char score[32];
	sprintf(score, "%d\00", game_player->score);
	font_draw(time, 5, screen, 2 * 8, 5 * 8, getColor4(-1, 555, 555, 555));
	font_draw(timeString, strlen(timeString), screen, (2 + 5) * 8, 5 * 8, getColor4(-1, 550, 550, 550));
	font_draw(scor, 6, screen, 2 * 8, 6 * 8, getColor4(-1, 555, 555, 555));
	font_draw(score, strlen(score), screen, (2 + 6) * 8, 6 * 8, getColor4(-1, 550, 550, 550));
	font_draw(prec, strlen(prec), screen, 2 * 8, 8 * 8, getColor4(-1, 333, 333, 333));

}
void wonmenu_tick(){
	if(inputDelay > 0){
		--inputDelay;
	}else if(attack.clicked || menu.clicked){
		game_set_menu(mid_TITLE);
	}
}
