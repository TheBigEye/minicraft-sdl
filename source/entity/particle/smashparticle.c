#include "smashparticle.h"
#include "../_entity_caller.h"
#include "../../gfx/color.h"


void smashparticle_create(SmashParticle* particle, int x, int y){
	entity_create(&particle->entity);

	particle->entity.type = SMASHPARTICLE;
	particle->time = 0;
	particle->entity.x = x;
	particle->entity.y = y;
	// TODO Sound.mosterHurt.play();
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
