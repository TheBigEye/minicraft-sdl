/*
 * powergloveitem.h - The power glove item
 *                    (Java: com.mojang.ld22.item.PowerGloveItem): picks
 *                    furniture back up into the inventory.
 */
#ifndef POWERGLOVEITEM_H
#define POWERGLOVEITEM_H 1

#include "../entity/player.h"
#include "../utils/javalang.h"
#include "item.h"

/* Constructor. Java: PowerGloveItem() */
PUBLIC void powergloveitem_create(Item* this);

/* Java: PowerGloveItem.getColor() */
PUBLIC int powergloveitem_get_color(Item* this);

/* Java: PowerGloveItem.getSprite() */
PUBLIC int powergloveitem_get_sprite(Item* this);

/* Java: PowerGloveItem.renderIcon(Screen, int, int) */
PUBLIC void powergloveitem_render_icon(Item* this, Screen* screen, int x, int y);

/* Java: PowerGloveItem.renderInventory(Screen, int, int) */
PUBLIC void powergloveitem_render_inventory(Item* this, Screen* screen, int x, int y);

/* Java: PowerGloveItem.getName() */
PUBLIC String powergloveitem_get_name(Item* this);

/*
 * Punching furniture with the glove picks it up.
 * Java: PowerGloveItem.interact(Player, Entity, int)
 */
PUBLIC boolean powergloveitem_interact(Item* this, Player* player, Entity* entity, int attackDir);

#endif /* POWERGLOVEITEM_H */
