/*
 * inventory_menu.c - The inventory menu
 *                    (Java: com.mojang.ld22.screen.InventoryMenu).
 */
#include "inventory_menu.h"

#include <stdlib.h>
#include <string.h>

#include "../entity/player.h"
#include "../game.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "../item/item.h"
#include "../utils/arraylist.h"
#include "menu.h"


InventoryMenu inventorymenu;


/* Java: new InventoryMenu(). Installs the methods and selects the first row. */
PUBLIC void inventorymenu_create(InventoryMenu* this) {
    this->selected = 0;

    this->menu.tick = inventorymenu_tick;
    this->menu.render = inventorymenu_render;
    this->menu.init = inventorymenu_init;
}


/*
 * Java: InventoryMenu.tick().
 *
 * Up and down cycle the items, wrapping around; confirming equips the
 * selected item as the active one and closes the menu.
 */
PUBLIC void inventorymenu_tick(Menu* this) {
    InventoryMenu* inv = (InventoryMenu*) this;

    if (menu.clicked) game_set_menu(null);

    if (up.clicked) --inv->selected;
    if (down.clicked) ++inv->selected;

    int len = game_player->inventory.items.size;

    if (len == 0) inv->selected = 0;
    if (inv->selected < 0) inv->selected += len;
    if (inv->selected >= len) inv->selected -= len;

    if (attack.clicked && len > 0) {
        Item* item = game_player->inventory.items.remove_at(&game_player->inventory.items, inv->selected);

        game_player->activeItem = item;
        game_set_menu(null);
    }
}


/*
 * Java: InventoryMenu.init().
 *
 * Returns the active item to the top of the inventory, so that it shows in
 * the list while browsing.
 */
PUBLIC void inventorymenu_init(Menu* this) {
    ((InventoryMenu*) this)->selected = 0;

    if (game_player->activeItem) {
        game_player->inventory.items.add_to(&game_player->inventory.items, 0, game_player->activeItem);
        game_player->activeItem = null;
    }
}


/* Java: InventoryMenu.render(Screen). Draws the framed item list with the
 * selection cursor. */
PUBLIC void inventorymenu_render(Menu* this, Screen* screen) {
    InventoryMenu* inv = (InventoryMenu*) this;

    char invLabel[] = "inventory";

    font_render_frame(screen, invLabel, strlen(invLabel), 1, 1, 14, 11);
    menu_render_item_list(screen, 1, 1, 14, 11, &game_player->inventory.items, inv->selected, item_render_inventory);
}
