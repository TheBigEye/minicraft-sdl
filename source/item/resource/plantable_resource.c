#include "resource.h"

void init_plantable_resource(Resource* resource, char* name, int sprite, int color, TileID target, TileID* sources, int size) {
	init_resource(resource, name, sprite, color);

	resource->add.plantable.sourceTilesSize = size;
	resource->add.plantable.sourceTiles = sources;
	resource->add.plantable.targetTile = target;
}
