#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include "entityid.h"

#include "../gfx/screen.h"
#include "../utils/javarandom.h"
#include "../utils/utils.h"

struct _Mob;
struct _Player;
struct _Item;
struct _Level;

typedef struct _Entity{
	EntityId type;
	Random random;
	int x, y;
	int xr, yr;
	struct _Level* level;
	uint8_t removed;
} Entity;


void entity_create(Entity* entity);
void entity_tick(Entity* entity);
void entity_render(Entity* entity, Screen* screen);
void entity_remove(Entity* entity);
void entity_init(Entity* entity, struct _Level* level);
uint8_t entity_intersects(Entity* entity, int x0, int y0, int x1, int y1);
uint8_t entity_blocks(Entity* entity, Entity* other);
//TODO hurt
//TODO hurt
uint8_t entity_move(Entity* entity, int xa, int ya);
uint8_t entity_move2(Entity* entity, int xa, int ya);
//TODO touchedBy
uint8_t entity_canSwim(Entity* entity);
uint8_t entity_interact(Entity* entity, struct _Player* player, struct _Item* item, int attackDir);
//TODO interact
//TODO use
int entity_getLightRadius(Entity* entity);

#endif // ENTITY_H
