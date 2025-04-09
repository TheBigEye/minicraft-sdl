#include "instructions_menu.h"
#include "menu.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "../game.h"
#include "../gfx/color.h"

#include <string.h>

static char s1[] = "HOW TO PLAY";
static char s2[] = "Move your character";
static char s3[] = "with the arrow keys";
static char s4[] = "press C to attack";
static char s5[] = "and X to open the";
static char s6[] = "inventory and to";
static char s7[] = "use items.";
static char s8[] = "Select an item in";
static char s9[] = "the inventory to";
static char s10[] = "equip it.";
static char s11[] = "Kill the air wizard";
static char s12[] = "to win the game!";

const menu_vt instructionsmenu_vt = {
	&instructionsmenu_tick,
	&instructionsmenu_render,
	&instructionsmenu_init
};

void instructionsmenu_tick(){
	if(attack.clicked || menu.clicked){
		game_set_menu(menu_parent);
	}
}
void instructionsmenu_init(){

}
void instructionsmenu_render(Screen* screen){
	clear_screen(screen, 0);
	int col = getColor4(0, 333, 333, 333);

	font_draw(s1, strlen(s1), screen, 4 * 8 + 4, 1 * 8, getColor4(0, 555, 555, 555));

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
