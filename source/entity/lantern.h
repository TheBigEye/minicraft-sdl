/*
 * lantern.h - Lantern furniture: emits light in dark areas.
 */
#ifndef LANTERN_H
#define LANTERN_H
#include "furniture.h"

typedef Furniture Lantern;
void lantern_create(Lantern* lantern);
int lantern_getLightRadius(Lantern* lantern);

#endif // LANTERN_H
