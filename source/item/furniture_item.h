#ifndef FURNITURE_ITEM_H
#define FURNITURE_ITEM_H
#include "item.h"

void furnitureitem_create(Item* item, struct _Furniture* furniture);
int furnitureitem_getColor(Item* item);
int furnitureitem_getSprite(Item* item);
void furnitureitem_renderIcon(Item* item, struct _Screen* screen, int x, int y);
void furnitureitem_renderInventory(Item* item, struct _Screen* screen, int x, int y);
char furnitureitem_interactOn(Item* item, TileID tile, Level* level, int xt, int yt, struct _Player* player, int attackDir);
#endif // FURNITURE_ITEM_H
