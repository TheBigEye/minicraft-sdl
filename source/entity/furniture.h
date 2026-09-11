/*
 * furniture.h - Base class of placeable furniture (Java:
 *               entity.Furniture): stations, chests and lanterns.
 */
#ifndef FURNITURE_H
#define FURNITURE_H
#include "entity.h"

struct _Player;

typedef struct _Furniture{
	Entity entity;
	int pushTime, pushDir;
	int col, sprite;
	char* name;
	struct _Player* shouldTake;
} Furniture;

/* Furniture's vtable (the Java `Furniture` class); subclass vtables inherit it. */
extern const EntityVTable furniture_vtable;

void furniture_create(Furniture* furniture, char* name);
void furniture_tick(Furniture* furniture);
void furniture_render(Furniture* furniture, Screen* screen);
void furniture_touchedBy(Furniture* furniture, Entity* entity);
void furniture_take(Furniture* furniture, struct _Player* player);
void furniture_free(Furniture* furniture);
char furniture_blocks(Furniture* furniture, Entity* other);

/* The C equivalent of Java's `e instanceof Furniture`. */
char entity_isfurniture(Entity* entity);

/* Factory: creates a furniture entity by id (Java: `new Anvil()` etc.). */
Furniture* entity_createFurniture(EntityId id);

/* C-specific: deep-copies furniture (used by the power glove pick-up). */
Furniture* furniture_create_copy(Furniture* old);

#endif // FURNITURE_H
