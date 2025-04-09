#ifndef ITEM_RESOURCE_PLANTABLE_RESOURCE_H_
#define ITEM_RESOURCE_PLANTABLE_RESOURCE_H_

#include "../../level/tile/tile.h"

struct _Resource;

typedef struct{
	int sourceTilesSize;
	TileID* sourceTiles;
	TileID targetTile;
} res_plantable;



void init_plantable_resource(struct _Resource* resource, char* name, int sprite, int color, TileID target, TileID* source, int size);

#endif /* ITEM_RESOURCE_PLANTABLE_RESOURCE_H_ */
