/*
 * resourceitem.h - The resource item
 *                  (Java: com.mojang.ld22.item.ResourceItem): a stackable
 *                  count of one resource kind.
 */
#ifndef RESOURCEITEM_H
#define RESOURCEITEM_H 1

#include "../entity/player.h"
#include "../utils/javalang.h"
#include "item.h"
#include "resource/resource.h"

/**
 * Constructors. Java overloads the constructor on the argument list:
 *
 *     ResourceItem(Resource resource)             -> one unit
 *     ResourceItem(Resource resource, int count)  -> `count` units
 *
 * so there is one name here too, and the two implementations below are
 * picked by how many arguments the call site passes. See javalang.h.
 */
PUBLIC void resourceitem_create_2(Item* this, Resource* resource);
PUBLIC void resourceitem_create_3(Item* this, Resource* resource, int count);
#define resourceitem_create(...)   LANG_OVERLOAD(resourceitem_create, __VA_ARGS__)

/* Java: ResourceItem.getColor() */
PUBLIC int resourceitem_get_color(Item* this);

/* Java: ResourceItem.getSprite() */
PUBLIC int resourceitem_get_sprite(Item* this);

/* Java: ResourceItem.renderIcon(Screen, int, int) */
PUBLIC void resourceitem_render_icon(Item* this, Screen* screen, int x, int y);

/* Java: ResourceItem.renderInventory(Screen, int, int) */
PUBLIC void resourceitem_render_inventory(Item* this, Screen* screen, int x, int y);

/* Java: ResourceItem.getName() */
PUBLIC String resourceitem_get_name(Item* this);

/*
 * Uses one unit through the resource's own tile interaction.
 * Java: ResourceItem.interactOn(Tile, Level, int, int, Player, int)
 */
PUBLIC boolean resourceitem_interact_on(Item* this, TileID tile, Level* level, int xt, int yt, Player* player, int attackDir);

/*
 * True when the stack count has dropped to zero.
 * Java: ResourceItem.isDepleted()
 */
PUBLIC boolean resourceitem_is_depleted(Item* this);

#endif /* RESOURCEITEM_H */
