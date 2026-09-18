/*
 * chest.c - The Chest furniture (Java: com.mojang.ld22.entity.Chest).
 *
 * Unlike the crafting stations, a chest stores items: it owns an Inventory
 * that is freed together with the chest, and using it opens the container
 * menu bound to that inventory.
 */
#include "chest.h"

#include <stdlib.h>
#include <string.h>

#include "../game.h"
#include "../gfx/color.h"
#include "../screen/container_menu.h"
#include "inventory.h"
#include "player.h"


/* Constructor: spawns a chest and creates the empty inventory it stores. */
PUBLIC void chest_create(Chest* this) {
    /* XXX ew: the name has to outlive this call, so it is heap-allocated. */
    String name = new_array(char, strlen("Chest") + 1);

    strcpy(name, "Chest");

    furniture_create(&this->furniture, name);

    /* Java: class Chest extends Furniture, plus its own destructor, which
     * releases the inventory. */
    this->furniture.entity.use  = (entity_use_fn) chest_use;
    this->furniture.entity.free = (entity_free_fn) chest_free;

    this->furniture.entity.type = CHEST;
    this->furniture.col = get_color4(-1, 110, 331, 552);
    this->furniture.sprite = 1;

    inventory_create(&this->inventory);
}


/* Opens the container menu over this chest's own inventory. */
PUBLIC boolean chest_use(Chest* this, struct Player* player, int attackDir) {
    (void) player;
    (void) attackDir;

    strcpy(contmenu_title, "Chest");
    contmenu_container = &this->inventory;
    game_set_menu(mid_CONTAINER);

    /* Java: player.game.setMenu(new ContainerMenu(player, "Chest", inventory)); */

    return true;
}


/* Releases the furniture name and the chest inventory. */
PUBLIC void chest_free(Chest* this) {
    furniture_free(&this->furniture);
    inventory_free(&this->inventory);
}
