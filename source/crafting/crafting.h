#ifndef CRAFTING_H
#define CRAFTING_H

#include "../utils/arraylist.h"

extern ArrayList anvilRecipes;
extern ArrayList ovenRecipes;
extern ArrayList furnaceRecipes;
extern ArrayList workbenchRecipes;

void crafting_init();
void crafting_free();

#endif
