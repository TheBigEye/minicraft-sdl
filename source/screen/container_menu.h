/*
 * container_menu.h - Container menu (Java: ContainerMenu): the
 *                    two-pane chest transfer screen.
 */
#ifndef CONTAINER_MENU_H
#define CONTAINER_MENU_H
#include "menu.h"

extern const menu_vt containermenu_vt;
extern struct _Inventory* contmenu_container; /* Inventory being looted. */
extern char contmenu_title[64];               /* Window caption. */

void containermenu_tick();
void containermenu_init();
void containermenu_render(Screen*);
#endif // CONTAINER_MENU_H
