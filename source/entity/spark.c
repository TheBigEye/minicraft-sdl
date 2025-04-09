#include "airwizard.h"
#include "spark.h"
#include "../gfx/screen.h"
#include "../utils/arraylist.h"
#include "../level/level.h"
#include "_entity_caller.h"
#include "../gfx/color.h"


void spark_create(Spark* spark, AirWizard* owner, double xa, double ya) {
	entity_create((Entity *) spark);

	spark->entity.type = SPARK;
	spark->owner = owner;
	spark->xx = spark->entity.x = owner->mob.entity.x;
	spark->yy = spark->entity.y = owner->mob.entity.y;
	spark->entity.xr = 0;
	spark->entity.yr = 0;
	spark->xa = xa;
	spark->ya = ya;
	spark->time = 0;
	spark->lifeTime = 60*10 + random_next_int(&spark->entity.random, 30);
}


void spark_tick(Spark* spark) {
	++spark->time;
	if (spark->time >= spark->lifeTime) {
		entity_remove((Entity *) spark);
		return;
	}

	spark->xx += spark->xa;
	spark->yy += spark->ya;

	spark->entity.x = (int) spark->xx;
	spark->entity.y = (int) spark->yy;
	ArrayList toHit;
	create_arraylist(&toHit);

	level_getEntities(spark->entity.level, &toHit, spark->entity.x, spark->entity.y, spark->entity.x, spark->entity.y);
	for(int i = 0; i < toHit.size; ++i){
		Entity* e = toHit.elements[i];
		if (entity_ismob(e) && e->type != AIRWIZARD){
			call_entity_hurt(e, (Mob *) spark->owner, 1, ((Mob*)e)->dir ^ 1);
		}
	}
	arraylist_remove(&toHit);
}


void spark_render(Spark* spark, Screen* screen){
	if (spark->time >= spark->lifeTime - 6 * 20){
		if (spark->time / 6 % 2 == 0) return;
	}

	int xt = 8;
	int yt = 13;
	int x = spark->entity.x;
	int y = spark->entity.y;
	render_screen(screen, x-4, y-4-2, xt + yt*32, getColor4(-1, 555, 555, 555), random_next_int(&spark->entity.random, 4));
	render_screen(screen, x-4, y-4+2, xt + yt*32, getColor4(-1, 000, 000, 000), random_next_int(&spark->entity.random, 4));
}
