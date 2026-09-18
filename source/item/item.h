/*
 * item.h - The Item base class (Java: com.mojang.ld22.item.Item).
 *
 * Items are the inventory entries: resources, tools, furniture carriers
 * and the power glove. Item is a tagged union whose id selects which
 * per-type payload (add.*) is active, and the item_* functions are the
 * shared dispatchers over the type-specific implementations, standing in
 * for the per-subclass overrides that Java gets from inheritance.
 */
#ifndef ITEM_H
#define ITEM_H 1

#include "../gfx/screen.h"
#include "../utils/javalang.h"
#include "../level/level.h"
#include "../level/tile/tileids.h"
#include "../utils/javarandom.h"

#include "itemids.h"
#include "resource/resource.h"
#include "tooltype.h"

struct Player;
struct ItemEntity;

typedef struct Item Item;

struct Item {
    /*
     * Selects which add.* payload is active. This is where Java used
     * instanceof and one subclass per kind.
     */
    ItemID id;

    union {
        struct {
            /* Resource kind this stack holds. Java: ResourceItem.resource */
            Resource* resource;
            /* Remaining units in the stack. Java: ResourceItem.count */
            int count;
        } resource;
        struct {
            /* Owned furniture entity to place. Java: FurnitureItem.furniture */
            struct Furniture* furniture;
            /*
             * Set once dropped into the world. C-only: Java's furniture
             * item simply keeps the reference and the world owns it.
             */
            boolean placed;
        } furniture;
        struct {
            /*
             * Per-tool RNG for bonus rolls. C-only: Java's ToolItem takes
             * the level from the tool and rolls on a shared generator.
             */
            Random random;
            /* Shovel, hoe, sword, pickaxe or axe. Java: ToolItem.type */
            ToolType type;
            /* Material tier (0 wood .. 4 gem). Java: ToolItem.level */
            int level;
        } tool;
    } add;
};

/* --- Dispatchers: route to the active type's implementation --- */

/* Java: Item.getColor() */
PUBLIC int item_get_color(Item* this);

/* Java: Item.getSprite() */
PUBLIC int item_get_sprite(Item* this);

/* Java: Item.onTake(ItemEntity) */
PUBLIC void item_on_take(Item* this, struct ItemEntity* itemEntity);

/* Java: Item.renderInventory(Screen, int, int) */
PUBLIC void item_render_inventory(Item* this, Screen* screen, int x, int y);

/* Java: Item.interact(Player, Entity, int) */
PUBLIC boolean item_interact(Item* this, struct Player* player, Entity* entity, int attackDir);

/* Java: Item.renderIcon(Screen, int, int) */
PUBLIC void item_render_icon(Item* this, Screen* screen, int x, int y);

/* Java: Item.interactOn(Tile, Level, int, int, Player, int) */
PUBLIC boolean item_interact_on(Item* this, TileID tile, Level* level, int xt, int yt, struct Player* player, int attackDir);

/* Depleted items are removed from the inventory after use.
 * Java: Item.isDepleted() */
PUBLIC boolean item_is_depleted(Item* this);

/* Java: Item.canAttack() */
PUBLIC boolean item_can_attack(Item* this);

/* Java: Item.getAttackDamageBonus(Entity) */
PUBLIC int item_get_attack_damage_bonus(Item* this, Entity* entity);

/*
 * Java: Item.getName() returns a String; the port copies the display name
 * into `buf` instead, so that no caller has to free it.
 */
PUBLIC void item_get_name(Item* this, char* buf);

/* Whether two items can share one inventory stack. Java: Item.matches(Item) */
PUBLIC boolean item_matches(Item* this, Item* item);

/* C-only: releases the type-specific storage (furniture entities). */
PUBLIC void item_free(Item* this);

#endif /* ITEM_H */
