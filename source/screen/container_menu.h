/*
 * container_menu.h - The container menu
 *                    (Java: com.mojang.ld22.screen.ContainerMenu): the
 *                    two-pane chest transfer screen.
 */
#ifndef CONTAINER_MENU_H
#define CONTAINER_MENU_H 1

#include "../entity/inventory.h"
#include "../utils/javalang.h"
#include "menu.h"

typedef struct ContainerMenu ContainerMenu;

struct ContainerMenu {
    /* Java: extends Menu */
    Menu menu;
    /* Active pane: 0 the container, 1 the inventory. Java: `private int window` */
    int window;
    /* Selection in the inactive pane. Java: `private int oSelected` */
    int oSelected;
    /* Selection in the active pane. Java: `private int selected` */
    int selected;
};

/* The one container screen; Java builds a new ContainerMenu() per chest. */
extern ContainerMenu containermenu;

/*
 * Inventory being looted. Java: `private Inventory container`
 *
 * The port sets it just before opening the menu, together with the title,
 * which is the constructor's job in Java.
 */
extern Inventory* contmenu_container;
/* Window caption. Java: `private String title` */
extern char contmenu_title[64];

/* Constructor: installs the ContainerMenu methods. Java: new ContainerMenu() */
PUBLIC void containermenu_create(ContainerMenu* this);

/* Java: ContainerMenu.tick() */
PUBLIC void containermenu_tick(Menu* this);

/* Java: ContainerMenu.init() */
PUBLIC void containermenu_init(Menu* this);

/* Java: ContainerMenu.render(Screen) */
PUBLIC void containermenu_render(Menu* this, Screen* screen);

#endif /* CONTAINER_MENU_H */
