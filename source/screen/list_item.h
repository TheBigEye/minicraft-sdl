#ifndef SCREEN_LIST_ITEM_H_
#define SCREEN_LIST_ITEM_H_
#include "../gfx/screen.h"

typedef struct{
	void (*renderInventory)(Screen* screen, int i, int j);
} list_item_vt;

#endif /* SCREEN_LIST_ITEM_H_ */
