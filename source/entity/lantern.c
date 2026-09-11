/*
 * lantern.c - Lantern furniture (Java: com.mojang.ld22.entity.Lantern).
 *
 * A placeable light source: its only addition over plain Furniture
 * is a light radius of 8, lighting caves and night around it.
 */
#include "lantern.h"
#include <stdlib.h>
#include "../gfx/color.h"

#include <string.h>

/* Java: Lantern.getLightRadius() { return 8; } */
int lantern_getLightRadius(Lantern* lantern) {
	(void) lantern;
	return 8;
}

/* The Lantern vtable (= the Java `Lantern` class): Furniture + light. */
static const EntityVTable lantern_vtable = {
	.tick           = (vt_tick_fn) furniture_tick,
	.render         = (vt_render_fn) furniture_render,
	.blocks         = (vt_blocks_fn) furniture_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = (vt_touchedBy_fn) furniture_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = entity_use,
	.getLightRadius = (vt_getLightRadius_fn) lantern_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = (vt_free_fn) furniture_free,
};

/*
 * Spawns a lantern: furniture named "Lantern", sprite 5 and a bright
 * yellow palette; light radius comes from the vtable override.
 */
void lantern_create(Lantern* lantern){
	char* name = malloc(strlen("Lantern") + 1); //XXX ew
	strcpy(name, "Lantern");

	furniture_create((Furniture *) lantern, name);
	lantern->entity.vt = &lantern_vtable;

	lantern->entity.type = LANTERN;
	lantern->col = getColor4(-1, 000, 111, 555);
	lantern->sprite = 5;
	lantern->entity.xr = 3;
	lantern->entity.yr = 2;
}
