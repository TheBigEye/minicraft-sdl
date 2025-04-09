#ifndef CONTAINER_MENU_H
#define CONTAINER_MENU_H
#include "menu.h"

extern const menu_vt containermenu_vt;
extern struct _Inventory* contmenu_container;
extern char contmenu_title[64];

void containermenu_tick();
void containermenu_init();
void containermenu_render(Screen*);
#endif // CONTAINER_MENU_H
