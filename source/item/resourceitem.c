/*
 * resourceitem.c - The resource item
 *                  (Java: com.mojang.ld22.item.ResourceItem).
 */
#include "resourceitem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../gfx/color.h"
#include "../gfx/font.h"
#include "itemids.h"
#include "resource/resource.h"


/* Constructor: a stack of a single unit.
 * Java: ResourceItem(Resource resource) */
PUBLIC void resourceitem_create_2(Item* this, Resource* resource) {
    this->id = RESOURCE;
    this->add.resource.resource = resource;
    this->add.resource.count = 1;
}


/* Constructor: a stack with an explicit initial count.
 * Java: ResourceItem(Resource resource, int count) */
PUBLIC void resourceitem_create_3(Item* this, Resource* resource, int count) {
    this->id = RESOURCE;
    this->add.resource.resource = resource;
    this->add.resource.count = count;
}


/* Java: ResourceItem.getColor() */
PUBLIC int resourceitem_get_color(Item* this) {
    return this->add.resource.resource->color;
}


/* Java: ResourceItem.getSprite() */
PUBLIC int resourceitem_get_sprite(Item* this) {
    return this->add.resource.resource->sprite;
}


/* Java: ResourceItem.renderIcon(Screen, int, int) */
PUBLIC void resourceitem_render_icon(Item* this, Screen* screen, int x, int y) {
    screen->render(screen, x, y, this->add.resource.resource->sprite, this->add.resource.resource->color, 0);
}


/*
 * Java: ResourceItem.renderInventory(Screen, int, int).
 *
 * Draws icon, name and stack count, with the count clamped to 999 so that
 * it always fits the three digits the row reserves for it.
 */
PUBLIC void resourceitem_render_inventory(Item* this, Screen* screen, int x, int y) {
    screen->render(screen, x, y, this->add.resource.resource->sprite, this->add.resource.resource->color, 0);
    font_draw(this->add.resource.resource->name, strlen(this->add.resource.resource->name), screen, x + 32, y, get_color4(-1, 555, 555, 555));

    int cc = this->add.resource.count;

    if (cc > 999) cc = 999;

    /*
     * Wide enough for any int, not just for the clamped range: the clamp
     * above is what keeps the row readable, but sprintf has to be safe
     * regardless of what the count holds.
     */
    char num[16] = {0};

    sprintf(num, "%d", cc);
    font_draw(num, 3, screen, x + 8, y, get_color4(-1, 444, 444, 444));
}


/* Java: ResourceItem.getName() */
PUBLIC String resourceitem_get_name(Item* this) {
    return this->add.resource.resource->name;
}


/*
 * Java: ResourceItem.interactOn(Tile, Level, int, int, Player, int).
 *
 * Delegates to the resource's tile interaction and consumes one unit from
 * the stack when it succeeds.
 */
PUBLIC boolean resourceitem_interact_on(Item* this, TileID tile, Level* level, int xt, int yt, Player* player, int attackDir) {
    if (resource_interact_on(this->add.resource.resource, tile, level, xt, yt, player, attackDir)) {
        --this->add.resource.count;

        return true;
    }

    return false;
}


/* Java: ResourceItem.isDepleted() */
PUBLIC boolean resourceitem_is_depleted(Item* this) {
    return this->add.resource.count <= 0;
}
