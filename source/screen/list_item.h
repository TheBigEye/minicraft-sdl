/*
 * list_item.h - The shared shape of the rows drawn by
 *               menu_render_item_list (Java: com.mojang.ld22.screen.ListItem).
 */
#ifndef SCREEN_LIST_ITEM_H_
#define SCREEN_LIST_ITEM_H_ 1

#include "../gfx/screen.h"
#include "../utils/javalang.h"

typedef struct list_item {
    /* Draws one list row at the given screen position.
     * Java: ListItem.renderInventory(Screen, int, int) */
    void (*renderInventory)(Screen* screen, int i, int j);
} list_item;

#endif /* SCREEN_LIST_ITEM_H_ */
