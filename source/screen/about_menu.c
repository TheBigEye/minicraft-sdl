#include "about_menu.h"
#include "menu.h"

#include "../game.h"
#include "../inputhandler.h"
#include "../gfx/color.h"
#include "../gfx/font.h"

#include <string.h>

static char s1[] = "About Minicraft"; //i made the worst str2upper func
static char s2[] = "Minicraft was made";
static char s3[] = "by Markus Persson";
static char s4[] = "For the 22'nd ludum";
static char s5[] = "dare competition in";
static char s6[] = "december 2011.";
static char s7[] = "it is dedicated to";
static char s8[] = "my father. <3";

const menu_vt aboutmenu_vt = {
	&aboutmenu_tick,
	&aboutmenu_render,
	&aboutmenu_init
};

void aboutmenu_tick(){
	if(attack.clicked || menu.clicked){
		game_set_menu(menu_parent);
	}
}

void aboutmenu_init(){

}

void aboutmenu_render(Screen* screen){
	clear_screen(screen, 0);
	int col = getColor4(0, 333, 333, 333);
	font_draw(s1, strlen(s1), screen, 2 * 8 + 4, 1 * 8, getColor4(0, 555, 555, 555));

	font_draw(s2, strlen(s2), screen, 0 * 8 + 4, 3 * 8, col);
	font_draw(s3, strlen(s3), screen, 0 * 8 + 4, 4 * 8, col);
	font_draw(s4, strlen(s4), screen, 0 * 8 + 4, 5 * 8, col);
	font_draw(s5, strlen(s5), screen, 0 * 8 + 4, 6 * 8, col);
	font_draw(s6, strlen(s6), screen, 0 * 8 + 4, 7 * 8, col);
	font_draw(s7, strlen(s7), screen, 0 * 8 + 4, 9 * 8, col);
	font_draw(s8, strlen(s8), screen, 0 * 8 + 4, 10 * 8, col);
}
