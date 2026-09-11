/*
 * item.h - Item base (Java: Item).
 *
 * Items are inventory entries: resources, tools, furniture carriers
 * and the power glove. Item is a tagged union whose id selects which
 * per-type payload (add.*) is active, and the item_* functions act
 * as the shared dispatcher over the type-specific implementations.
 */
#ifndef ITEM_H
#define ITEM_H

#include "../utils/javarandom.h"
#include "../gfx/screen.h"
#include <stdint.h>
#include "../entity/entity.h"
#include "../level/tile/tileids.h"
#include "../level/level.h"
#include "itemids.h"
#include "resource/resource.h"
#include "../entity/player.h"
#include "../utils/javarandom.h"
#include "tooltype.h"

typedef struct _Item{
	ItemID id;  /* Selects which add.* payload is active. */
	union{
		struct{
			Resource* resource; /* Resource kind this stack holds. */
			int count;          /* Remaining units in the stack. */
		} resource;
		struct{
			struct _Furniture* furniture; /* Owned furniture entity to place. */
			uint8_t placed;               /* Set once dropped into the world. */
		} furniture;
		struct{
			Random random;   /* Per-tool RNG for bonus rolls. */
			ToolType type;   /* Shovel, hoe, sword, pickaxe or axe. */
			int level;       /* Material tier (0 wood .. 4 gem). */
		} tool;
	} add;
} Item;
struct _Player;
struct _ItemEntity;

/* Dispatchers: route to the active type's implementation. */
int item_getColor(Item* item);
int item_getSprite(Item* item);
void item_onTake(Item* item, struct _ItemEntity* itemEntity);
void item_renderInventory(Item* item, Screen* screen, int x, int y);
uint8_t item_interact(Item* item, struct _Player* player, Entity* entity, int attackDir);
void item_renderIcon(Item* item, Screen* screen, int x, int y);
uint8_t item_interactOn(Item* item, TileID tile, Level* level, int xt, int yt, struct _Player* player, int attackDir);
/* Depleted items are removed from the inventory after use. */
uint8_t item_isDepleted(Item* item);
uint8_t item_canAttack(Item* item);
int item_getAttackDamageBonus(Item* item, Entity* entity);
/* Copies the display name into buf. */
void item_getName(Item* item, char* buf);
uint8_t matches(Item* item, Item* item2);
/* Releases type-specific storage (furniture entities). */
void item_free(Item* item);

/* Whether two items can share one inventory stack. */
uint8_t item_matches(Item* item, Item* item2);


#endif // ITEM_H
