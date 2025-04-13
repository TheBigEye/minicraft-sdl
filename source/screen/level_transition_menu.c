#include "menu.h"
#include "level_transition_menu.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "../game.h"

#include <string.h>

const menu_vt leveltransitionmenu_vt = {
	&leveltransitionmenu_tick,
	&leveltransitionmenu_render,
	&leveltransitionmenu_init
};

static int lt_time;
static int lt_dir;

void leveltransitionmenu_init(){
	lt_time = 0;
	lt_dir = game_pendingLevelChange;
}
void leveltransitionmenu_render(Screen* screen){
	for(int x = 0; x < 40; ++x){
		for(int y = 0; y < 30; ++y){
			int dd = (y + x % 2 * 2 + x/3) - lt_time;
			if(dd < 0 && dd > -30) render_screen(screen, x*8, y*8, 0, 0, 0);
			else render_screen(screen, x*8, screen->h - y*8 - 9, 0, 0, 0);
		}
	}
}
void leveltransitionmenu_tick(){
	lt_time += 2;
	if(lt_time == 30) game_changeLevel(lt_dir);
	if(lt_time == 60) game_set_menu(0);
}
