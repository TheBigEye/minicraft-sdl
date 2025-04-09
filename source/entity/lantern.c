#include "lantern.h"
#include "../gfx/color.h"

#include <string.h>

void lantern_create(Lantern* lantern){
	char* name = malloc(strlen("Lantern")+1); //XXX ew
	strcpy(name, "Lantern");

	furniture_create((Furniture *) lantern, name);

	lantern->entity.type = LANTERN;
	lantern->col = getColor4(-1, 000, 111, 555);
	lantern->sprite = 5;
	lantern->entity.xr = 3;
	lantern->entity.yr = 2;
}
