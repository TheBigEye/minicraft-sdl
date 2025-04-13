#include "itementity.h"
#include "_entity_caller.h"
#include "../gfx/color.h"


void itementity_create(ItemEntity* entity, Item item, int x, int y){
	entity_create(&entity->entity);

	entity->entity.type = ITEMENTITY;
	entity->walkDist = 0;
	entity->dir = 0;
	entity->hurtTime = 0;
	entity->xKnockback = entity->yKnockback = 0;
	entity->time = 0;

	entity->item = item;
	entity->xx = entity->entity.x = x;
	entity->yy = entity->entity.y = y;

	entity->entity.xr = 3;
	entity->entity.yr = 3;

	entity->zz = 2;
	entity->xa = random_next_gaussian(&entity->entity.random) * 0.3;
	entity->ya = random_next_gaussian(&entity->entity.random) * 0.2;
	entity->za = random_next_float(&entity->entity.random) * 0.7 + 1;

	entity->lifeTime = 60*10 + random_next_int(&entity->entity.random, 60);
}


void itementity_tick(ItemEntity* item){
	++item->time;
	if (item->time >= item->lifeTime) {
		entity_remove(&item->entity);
		return;
	}

	item->xx += item->xa;
	item->yy += item->ya;
	item->zz += item->za;

	if (item->zz < 0) {
		item->zz = 0;
		item->za *= -0.5;
		item->xa *= 0.6;
		item->ya *= 0.6;
	}

	item->za -= 0.15;

	int ox = item->entity.x;
	int oy = item->entity.y;
	int nx = item->xx;
	int ny = item->yy;
	int exceptedx = nx - item->entity.x;
	int exceptedy = ny - item->entity.y;
	entity_move(&item->entity, nx - item->entity.x, ny - item->entity.y);

	int gotx = item->entity.x - ox;
	int goty = item->entity.y - oy;
	item->xx += gotx - exceptedx;
	item->yy += goty - exceptedy;

	if (item->hurtTime > 0) --item->hurtTime;
}


void itementity_render(ItemEntity* item, Screen* screen){
	if (item->time >= item->lifeTime - 6 * 20) {
		if(item->time / 6 % 2 == 0) return;
	}

	int x = item->entity.x;
	int y = item->entity.y;

	render_screen(screen, x - 4, y - 4, item_getSprite(&item->item), getColor4(-1, 0, 0, 0), 0);
	render_screen(screen, x - 4, y - 4 - item->zz, item_getSprite(&item->item), item_getColor(&item->item), 0);
}


void itementity_take(ItemEntity* item, Player* player){
	// TODO Sound.pickup.play();

	++player->score;
	item_onTake(&item->item, (struct _ItemEntity *) item);
	entity_remove(&item->entity);
}
