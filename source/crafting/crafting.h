/*
 * crafting.h - The recipe tables
 *              (Java: com.mojang.ld22.crafting.Crafting): the four station
 *              recipe lists, filled at startup.
 */
#ifndef CRAFTING_H
#define CRAFTING_H 1

#include "../utils/javalang.h"
#include "../utils/arraylist.h"

/* Tools of iron tier and above. Java: `public static List<Recipe> anvilRecipes` */
extern ArrayList anvilRecipes;
/* Food. Java: `public static List<Recipe> ovenRecipes` */
extern ArrayList ovenRecipes;
/* Smelting. Java: `public static List<Recipe> furnaceRecipes` */
extern ArrayList furnaceRecipes;
/* Furniture and low-tier tools. Java: `public static List<Recipe> workbenchRecipes` */
extern ArrayList workbenchRecipes;

/* Builds all four recipe lists. */
PUBLIC void crafting_init(void);

/* Frees every recipe and the lists themselves. */
PUBLIC void crafting_free(void);

#endif /* CRAFTING_H */
