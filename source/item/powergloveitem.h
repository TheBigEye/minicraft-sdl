/*
 * powergloveitem.h - Power glove item (Java: PowerGloveItem): picks
 *                    furniture back up into the inventory.
 */
#ifndef POWERGLOVEITEM_H
#define POWERGLOVEITEM_H
#include "item.h"

void powergloveitem_create(Item* item);
int powergloveitem_getColor(Item* item);
int powergloveitem_getSprite(Item* item);
void powergloveitem_renderIcon(Item* item, Screen* screen, int x, int y);
void powergloveitem_renderInventory(Item* item, Screen* screen, int x, int y);
char* powergloveitem_getName(Item* item);
/* Punching furniture with the glove picks it up. */
uint8_t powergloveitem_interact(Item* item, Player* player, Entity* entity, int attackDir);

#endif // POWERGLOVEITEM_H
