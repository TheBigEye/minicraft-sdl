/*
 * container_menu.c - The container menu
 *                    (Java: com.mojang.ld22.screen.ContainerMenu).
 */
#include "container_menu.h"

#include <stdlib.h>
#include <string.h>

#include "../entity/inventory.h"
#include "../entity/player.h"
#include "../game.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "../item/item.h"
#include "menu.h"


Inventory* contmenu_container = null;
char contmenu_title[64] = {0};

ContainerMenu containermenu;


/* Java: new ContainerMenu(). Installs the methods and resets both panes. */
PUBLIC void containermenu_create(ContainerMenu* this) {
    this->window = 0;
    this->oSelected = 0;
    this->selected = 0;

    this->menu.tick = containermenu_tick;
    this->menu.render = containermenu_render;
    this->menu.init = containermenu_init;
}


/*
 * Java: ContainerMenu.tick().
 *
 * Left and right swap the active pane, and its selection, with the other
 * pane's; up and down move within the pane; confirming moves the selected
 * item into the opposite inventory, at the slot it remembers.
 */
PUBLIC void containermenu_tick(Menu* this) {
    ContainerMenu* cont = (ContainerMenu*) this;

    if (menu.clicked) game_set_menu(null);

    if (left.clicked) {
        cont->window = 0;

        int tmp = cont->selected;

        cont->selected = cont->oSelected;
        cont->oSelected = tmp;
    }

    if (right.clicked) {
        cont->window = 1;

        int tmp = cont->selected;

        cont->selected = cont->oSelected;
        cont->oSelected = tmp;
    }

    Inventory* i = cont->window == 1 ? &game_player->inventory : contmenu_container;
    Inventory* i2 = cont->window == 0 ? &game_player->inventory : contmenu_container;

    int len = i->items.size;

    if (cont->selected < 0) cont->selected = 0;
    if (cont->selected >= len) cont->selected = len - 1;

    if (up.clicked) --cont->selected;
    if (down.clicked) ++cont->selected;

    if (len == 0) cont->selected = 0;
    if (cont->selected < 0) cont->selected += len;
    if (cont->selected >= len) cont->selected -= len;

    if (attack.clicked && len > 0) {
        Item* item = i->items.remove_at(&i->items, cont->selected);

        inventory_add(i2, cont->oSelected, item);

        /*
         * C-only: inventory_add() copied the item into the other
         * inventory, so the original allocation is released here. Java
         * just drops the reference.
         */
        delete(item);

        if (cont->selected >= i->items.size) cont->selected = i->items.size - 1;
    }
}


/* Java: ContainerMenu.init(). Resets pane and selections to the container
 * side. */
PUBLIC void containermenu_init(Menu* this) {
    ContainerMenu* cont = (ContainerMenu*) this;

    cont->window = 0;
    cont->oSelected = 0;
    cont->selected = 0;
}


/*
 * Java: ContainerMenu.render(Screen).
 *
 * Draws the container and the inventory frames side by side; when the
 * inventory pane is active the whole screen scrolls half a frame.
 */
PUBLIC void containermenu_render(Menu* this, Screen* screen) {
    ContainerMenu* cont = (ContainerMenu*) this;

    if (cont->window == 1) screen_set_offset(screen, 8 * 8, 0);

    font_render_frame(screen, contmenu_title, strlen(contmenu_title), 1, 1, 14, 11);
    menu_render_item_list(screen, 1, 1, 14, 11, &contmenu_container->items, cont->window == 0 ? cont->selected : -cont->oSelected - 1, item_render_inventory);

    char s[] = "inventory";

    font_render_frame(screen, s, strlen(s), 15, 1, 15 + 11, 11);
    menu_render_item_list(screen, 15, 1, 15 + 11, 11, &game_player->inventory.items, cont->window == 1 ? cont->selected : -cont->oSelected - 1, item_render_inventory);

    screen_set_offset(screen, 0, 0);
}
