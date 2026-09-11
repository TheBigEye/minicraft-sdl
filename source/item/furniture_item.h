/*
 * furniture_item.h - Furniture item (Java: FurnitureItem): carries a
 *                    furniture entity in the inventory and places it.
 */
#ifndef FURNITURE_ITEM_H
#define FURNITURE_ITEM_H
#include "item.h"

/* Wraps an owned furniture entity as an inventory item. */
void furnitureitem_create(Item* item, struct _Furniture* furniture);
int furnitureitem_getColor(Item* item);
int furnitureitem_getSprite(Item* item);
void furnitureitem_renderIcon(Item* item, struct _Screen* screen, int x, int y);
void furnitureitem_renderInventory(Item* item, struct _Screen* screen, int x, int y);
/* Places the furniture on the target tile if passable. */
char furnitureitem_interactOn(Item* item, TileID tile, Level* level, int xt, int yt, struct _Player* player, int attackDir);
#endif // FURNITURE_ITEM_H
