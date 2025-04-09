#ifndef SCREEN_TITLE_MENU_H_
#define SCREEN_TITLE_MENU_H_
#include "menu.h"

extern const menu_vt titlemenu_vt;

void titlemenu_init();
void titlemenu_render(Screen*);
void titlemenu_tick();



#endif /* SCREEN_TITLE_MENU_H_ */
