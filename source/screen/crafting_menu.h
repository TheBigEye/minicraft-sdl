#ifndef CRAFTING_MENU_H
#define CRAFTING_MENU_H
#include "menu.h"
extern const menu_vt craftingmenu_vt;
extern ArrayList* crmenu_recipes;
void craftingmenu_tick();
void craftingmenu_init();
void craftingmenu_render(Screen*);
#endif // CRAFTING_MENU_H
