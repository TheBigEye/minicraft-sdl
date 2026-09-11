/*
 * resourceitem.h - Resource item (Java: ResourceItem): a stackable
 *                  count of one resource kind.
 */
#ifndef RESOURCEITEM_H
#define RESOURCEITEM_H
#include "item.h"
#include "resource/resource.h"

/* Creates a stack holding a single unit. */
void resourceitem_create(Item* item, Resource* resource);
/* Creates a stack with an explicit initial count. */
void resourceitem_create_cnt(Item* item, Resource* resource, int count);
int resourceitem_getColor(Item* item);
int resourceitem_getSprite(Item* item);
void resourceitem_renderIcon(Item* item, Screen* screen, int x, int y);
void resourceitem_renderInventory(Item* item, Screen* screen, int x, int y);
char* resourceitem_getName(Item* item);
/* Uses one unit via the resource's own tile interaction. */
char resourceitem_interactOn(Item* item, TileID tile, Level* level, int xt, int yt, Player* player, int attackDir);
/* True when the stack count has dropped to zero. */
char resourceitem_isDepleted(Item* item);

#endif // RESOURCEITEM_H
