/*
 * crafting.h - Recipe tables (Java: Crafting): the four station
 *              recipe lists filled at startup.
 */
#ifndef CRAFTING_H
#define CRAFTING_H

#include "../utils/arraylist.h"

extern ArrayList anvilRecipes;      /* Tools of iron tier and above. */
extern ArrayList ovenRecipes;       /* Food. */
extern ArrayList furnaceRecipes;    /* Smelting. */
extern ArrayList workbenchRecipes;  /* Furniture and low-tier tools. */

/* Builds all four recipe lists. */
void crafting_init();
/* Frees every recipe and the lists themselves. */
void crafting_free();

#endif
