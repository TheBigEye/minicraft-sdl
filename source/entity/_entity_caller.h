#ifndef _ENTITY_CALLER_H
#define _ENTITY_CALLER_H
#include "entity.h"
#include "itementity.h"
#include "../gfx/screen.h"

struct _Furniture;

void call_entity_tick(Entity* entity);
void call_entity_render(Entity* entity, Screen* screen);
char call_entity_isSwimming(Entity* entity);
uint8_t call_entity_canSwim(Entity* entity);
void call_entity_free(Entity* entity);
void call_entity_touchedBy(Entity* entity, Entity* e);
char call_entity_blocks(Entity* entity, Entity* e);
void call_entity_touchItem(Entity* entity, ItemEntity* item);
void call_entity_die(Entity* mob);
char call_entity_use(Entity* entity, Player* player, int attackDir);
struct _Furniture* furniture_create_copy(struct _Furniture* old);
int call_entity_getLightRadius(Entity* entity);
void call_entity_hurtTile(Entity* entity, TileID tile, int x, int y, int damage);
void call_entity_hurt(Entity* entity, Mob* mob, int damage, int attackDir);
struct _Furniture* entity_createFurniture(EntityId id);
char entity_ismob(Entity* entity);

void call_entity_doHurt(Entity* entity, int damage, int attackDir);
char entity_isfurniture(Entity* entity);

#endif // _ENTITY_CALLER_H
