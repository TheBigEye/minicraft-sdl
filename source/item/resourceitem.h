#ifndef RESOURCEITEM_H
#define RESOURCEITEM_H
#include "item.h"
#include "resource/resource.h"

void resourceitem_create(Item* item, Resource* resource);
void resourceitem_create_cnt(Item* item, Resource* resource, int count);
int resourceitem_getColor(Item* item);
int resourceitem_getSprite(Item* item);
void resourceitem_renderIcon(Item* item, Screen* screen, int x, int y);
void resourceitem_renderInventory(Item* item, Screen* screen, int x, int y);
char* resourceitem_getName(Item* item);
char resourceitem_interactOn(Item* item, TileID tile, Level* level, int xt, int yt, Player* player, int attackDir);
char resourceitem_isDepleted(Item* item);

#endif // RESOURCEITEM_H
