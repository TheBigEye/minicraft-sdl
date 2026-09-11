/*
 * recipeids.h - Recipe kind tags for the Recipe union.
 */
#ifndef RECIPEIDS_H
#define RECIPEIDS_H

/* Discriminator selecting the active add.* payload in Recipe. */
typedef enum _RecipeID{
	rip_FURNITURE = 1,
	rip_RESOURCE,
	rip_TOOL
} RecipeID;

#endif // RECIPEIDS_H
