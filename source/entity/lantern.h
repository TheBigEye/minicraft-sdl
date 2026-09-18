/*
 * lantern.h - The Lantern: furniture that lights up dark areas
 *             (Java: com.mojang.ld22.entity.Lantern).
 */
#ifndef LANTERN_H
#define LANTERN_H 1

#include "furniture.h"

#include "../utils/javalang.h"

/*
 * A lantern adds nothing to Furniture but its light, so it is a plain
 * alias instead of a struct of its own.
 */
typedef Furniture Lantern;

/* Constructor. Java: Lantern() */
PUBLIC void lantern_create(Lantern* this);

/* Java: Lantern.getLightRadius() { return 8; } */
PUBLIC int lantern_get_light_radius(Lantern* this);

#endif /* LANTERN_H */
