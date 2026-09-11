/*
 * list_item.h - Shared vtable shape for rows drawn by
 *               menu_render_item_list (Java: ListItem).
 */
#ifndef SCREEN_LIST_ITEM_H_
#define SCREEN_LIST_ITEM_H_
#include "../gfx/screen.h"

typedef struct{
	/* Draws one list row at the given screen position. */
	void (*renderInventory)(Screen* screen, int i, int j);
} list_item_vt;

#endif /* SCREEN_LIST_ITEM_H_ */
