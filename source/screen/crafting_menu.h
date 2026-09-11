/*
 * crafting_menu.h - Crafting menu (Java: CraftingMenu): station
 *                   recipe browser with have/cost panels.
 */
#ifndef CRAFTING_MENU_H
#define CRAFTING_MENU_H
#include "menu.h"
extern const menu_vt craftingmenu_vt;
extern ArrayList* crmenu_recipes; /* Station recipe list to browse. */
void craftingmenu_tick();
void craftingmenu_init();
void craftingmenu_render(Screen*);
#endif // CRAFTING_MENU_H
