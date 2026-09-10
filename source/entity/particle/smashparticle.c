#include "smashparticle.h"
#include "../../gfx/color.h"
#include "../../sound/sound.h"

/* The SmashParticle vtable (= the Java `SmashParticle` class). */
static const EntityVTable smashparticle_vtable = {
	.tick           = (vt_tick_fn) smashparticle_tick,
	.render         = (vt_render_fn) smashparticle_render,
	.blocks         = entity_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = entity_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = entity_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = entity_free,
};


void smashparticle_create(SmashParticle* particle, int x, int y){
	entity_create(&particle->entity);
	particle->entity.vt = &smashparticle_vtable;

	particle->entity.type = SMASHPARTICLE;
	particle->time = 0;
	particle->entity.x = x;
	particle->entity.y = y;
	sound_play(SND_MONSTERHURT); // Sound.monsterHurt.play()
}


void smashparticle_tick(SmashParticle* particle){
	++particle->time;
	if (particle->time > 10) {
        entity_remove(&particle->entity);
    }
}


void smashparticle_render(SmashParticle* particle, Screen* screen){
	int col = getColor4(-1, 555, 555, 555);
	int x = particle->entity.x;
	int y = particle->entity.y;

	render_screen(screen, x - 8, y - 8, 5 + 12 * 32, col, 2);
	render_screen(screen, x - 0, y - 8, 5 + 12 * 32, col, 3);
	render_screen(screen, x - 8, y - 0, 5 + 12 * 32, col, 0);
	render_screen(screen, x - 0, y - 0, 5 + 12 * 32, col, 1);
}
