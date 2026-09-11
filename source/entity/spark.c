/*
 * spark.c - Spark projectile (Java: entity.Spark).
 *
 * Short-lived projectile the Air Wizard fires; hurts any mob it
 * touches except the wizard itself, then expires.
 */
#include "airwizard.h"
#include "spark.h"
#include "../gfx/screen.h"
#include "../utils/arraylist.h"
#include "../level/level.h"
#include "../gfx/color.h"

/* Java: Spark.isBlockableBy(Mob mob) { return false; } */
char spark_isBlockableBy(Spark* spark, Mob* mob) {
	(void) spark; (void) mob;
	return 0;
}

/* The Spark vtable (= the Java `Spark` class). */
static const EntityVTable spark_vtable = {
	.tick           = (vt_tick_fn) spark_tick,
	.render         = (vt_render_fn) spark_render,
	.blocks         = entity_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = entity_touchedBy,
	.isBlockableBy  = (vt_isBlockableBy_fn) spark_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = entity_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = entity_free,
};


/* Spawns the spark at the owner's position with the given velocity
 * and a lifetime of ~10 seconds plus a random extra. */
void spark_create(Spark* spark, AirWizard* owner, double xa, double ya) {
	entity_create(&spark->entity);
	spark->entity.vt = &spark_vtable;

	spark->entity.type = SPARK;
	spark->owner = owner;
	spark->xx = spark->entity.x = owner->mob.entity.x;
	spark->yy = spark->entity.y = owner->mob.entity.y;
	spark->entity.xr = 0;
	spark->entity.yr = 0;
	spark->xa = xa;
	spark->ya = ya;
	spark->time = 0;
	spark->lifeTime = 60 * 10 + random_next_int(&spark->entity.random, 30);
}


/* Ages and moves the spark; hurts mobs it overlaps (excluding the
 * Air Wizard) for 1 damage; removes itself at end of life. */
void spark_tick(Spark* spark) {
	++spark->time;
	if (spark->time >= spark->lifeTime) {
		entity_remove(&spark->entity);
		return;
	}

	spark->xx += spark->xa;
	spark->yy += spark->ya;

	spark->entity.x = (int) spark->xx;
	spark->entity.y = (int) spark->yy;
	ArrayList toHit;
	create_arraylist(&toHit);

	level_getEntities(spark->entity.level, &toHit, spark->entity.x, spark->entity.y, spark->entity.x, spark->entity.y);
	for (int i = 0; i < toHit.size; ++i) {
		Entity* e = toHit.elements[i];
		if (entity_ismob(e) && e->type != AIRWIZARD){
			e->vt->hurt(e, &spark->owner->mob, 1, ((Mob*) e)->dir ^ 1);
		}
	}
	arraylist_remove(&toHit);
}


/* Draws the spark as two flickering tiles; blinks during the last
 * seconds before expiring. */
void spark_render(Spark* spark, Screen* screen){
	if (spark->time >= spark->lifeTime - 6 * 20) {
		if (spark->time / 6 % 2 == 0) return;
	}

	int xt = 8;
	int yt = 13;

	int x = spark->entity.x;
	int y = spark->entity.y;

	render_screen(screen, x - 4, y - 4 - 2, xt + yt * 32, getColor4(-1, 555, 555, 555), random_next_int(&spark->entity.random, 4));
	render_screen(screen, x - 4, y - 4 + 2, xt + yt * 32, getColor4(-1, 000, 000, 000), random_next_int(&spark->entity.random, 4));
}
