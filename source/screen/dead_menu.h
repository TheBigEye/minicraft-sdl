#ifndef DEAD_MENU_H
#define DEAD_MENU_H
#include "menu.h"
extern const menu_vt deadmenu_vt;

void deadmenu_tick();
void deadmenu_init();
void deadmenu_render(Screen*);
#endif // DEAD_MENU_H
