/*
 * plantable_resource.h - Plantable resource extra data
 * (Java: PlantableResource).
 */
#ifndef ITEM_RESOURCE_PLANTABLE_RESOURCE_H_
#define ITEM_RESOURCE_PLANTABLE_RESOURCE_H_

#include "../../level/tile/tile.h"

struct _Resource;

/* Extra per-resource state for plantables. */
typedef struct{
	int sourceTilesSize;  /* Number of tiles this may be planted on. */
	TileID* sourceTiles;  /* Tiles this may be planted on. */
	TileID targetTile;    /* Tile the source becomes when planted. */
} res_plantable;



/* Initializes a resource as plantable with target/source tiles. */
void init_plantable_resource(struct _Resource* resource, char* name, int sprite, int color, TileID target, TileID* source, int size);

#endif /* ITEM_RESOURCE_PLANTABLE_RESOURCE_H_ */
