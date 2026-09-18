/*
 * lantern.c - The Lantern furniture
 *             (Java: com.mojang.ld22.entity.Lantern).
 *
 * A placeable light source: its only addition over plain Furniture is a
 * light radius of 8, lighting up caves and the night around it.
 */
#include "lantern.h"

#include <stdlib.h>
#include <string.h>

#include "../gfx/color.h"


/* Java: Lantern.getLightRadius() { return 8; } */
PUBLIC int lantern_get_light_radius(Lantern* this) {
    (void) this;

    return 8;
}


/*
 * Constructor: spawns a lantern, that is, furniture named "Lantern",
 * sprite 5, in a bright yellow palette and with the light radius coming
 * from the override above.
 */
PUBLIC void lantern_create(Lantern* this) {
    /* XXX ew: the name has to outlive this call, so it is heap-allocated. */
    String name = new_array(char, strlen("Lantern") + 1);

    strcpy(name, "Lantern");

    furniture_create(this, name);

    /* Java: class Lantern extends Furniture */
    this->entity.get_light_radius = (entity_get_light_radius_fn) lantern_get_light_radius;

    this->entity.type = LANTERN;
    this->col = get_color4(-1, 000, 111, 555);
    this->sprite = 5;
    this->entity.xr = 3;
    this->entity.yr = 2;
}
