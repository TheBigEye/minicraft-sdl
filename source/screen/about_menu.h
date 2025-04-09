#ifndef SCREEN_ABOUT_MENU_H_
#define SCREEN_ABOUT_MENU_H_
#include "menu.h"

extern const menu_vt aboutmenu_vt;

void aboutmenu_tick();
void aboutmenu_init();
void aboutmenu_render(Screen*);


#endif /* SCREEN_ABOUT_MENU_H_ */
