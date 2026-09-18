/*
 * crafting_menu.h - The crafting menu
 *                   (Java: com.mojang.ld22.screen.CraftingMenu): the
 *                   station recipe browser with its have/cost panels.
 */
#ifndef CRAFTING_MENU_H
#define CRAFTING_MENU_H 1

#include "../utils/javalang.h"
#include "../utils/arraylist.h"
#include "menu.h"

typedef struct CraftingMenu CraftingMenu;

struct CraftingMenu {
    /* Java: extends Menu */
    Menu menu;
    /* Java: `private int selected` */
    int selected;
};

/* The one crafting screen; Java builds a new CraftingMenu() per station. */
extern CraftingMenu craftingmenu;

/*
 * Station recipe list to browse.
 *
 * Java: the `List<Recipe> recipes` the constructor receives. The port sets
 * it just before opening the menu, which is the same thing without
 * building a new screen each time.
 */
extern ArrayList* crmenu_recipes;

/* Constructor: installs the CraftingMenu methods. Java: new CraftingMenu() */
PUBLIC void craftingmenu_create(CraftingMenu* this);

/* Java: CraftingMenu.tick() */
PUBLIC void craftingmenu_tick(Menu* this);

/* Java: CraftingMenu.init() */
PUBLIC void craftingmenu_init(Menu* this);

/* Java: CraftingMenu.render(Screen) */
PUBLIC void craftingmenu_render(Menu* this, Screen* screen);

#endif /* CRAFTING_MENU_H */
