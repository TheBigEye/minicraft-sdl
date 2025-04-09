#ifndef INVENTORY_MENU_H
#define INVENTORY_MENU_H

#include "menu.h"

extern const menu_vt inventorymenu_vt;

void inventorymenu_tick();
void inventorymenu_init();
void inventorymenu_render(Screen*);


#endif // INVENTORY_MENU_H
