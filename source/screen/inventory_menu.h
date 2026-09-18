/*
 * inventory_menu.h - The inventory menu
 *                    (Java: com.mojang.ld22.screen.InventoryMenu): browse
 *                    and equip the carried items.
 */
#ifndef INVENTORY_MENU_H
#define INVENTORY_MENU_H 1

#include "../utils/javalang.h"
#include "menu.h"

typedef struct InventoryMenu InventoryMenu;

struct InventoryMenu {
    /* Java: extends Menu */
    Menu menu;
    /* Java: `private int selected` */
    int selected;
};

/* The one inventory screen; Java builds a new InventoryMenu() per open. */
extern InventoryMenu inventorymenu;

/* Constructor: installs the InventoryMenu methods. Java: new InventoryMenu() */
PUBLIC void inventorymenu_create(InventoryMenu* this);

/* Java: InventoryMenu.tick() */
PUBLIC void inventorymenu_tick(Menu* this);

/* Java: InventoryMenu.init() */
PUBLIC void inventorymenu_init(Menu* this);

/* Java: InventoryMenu.render(Screen) */
PUBLIC void inventorymenu_render(Menu* this, Screen* screen);

#endif /* INVENTORY_MENU_H */
