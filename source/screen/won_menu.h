#ifndef WON_MENU_H
#define WON_MENU_H
#include "menu.h"

extern const menu_vt wonmenu_vt;

void wonmenu_init();
void wonmenu_render(Screen*);
void wonmenu_tick();

#endif // WON_MENU_H
