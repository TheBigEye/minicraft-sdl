/*
 * plantable_resource.c - The plantable resource initializer
 *                        (Java: com.mojang.ld22.item.resource.PlantableResource).
 */
#include "plantable_resource.h"

#include "resource.h"


/*
 * Constructor: fills the common fields, then the planting rules. The source
 * tile table is borrowed, not copied, so it is never freed here.
 * Java: PlantableResource(String, int, int, Tile, Tile...)
 */
PUBLIC void init_plantable_resource(struct Resource* this, const char* name, int sprite, int color, TileID target, TileID* sources, int size) {
    init_resource(this, name, sprite, color);   /* super(name, sprite, color) */

    this->add.plantable.sourceTilesSize = size;
    this->add.plantable.sourceTiles = sources;
    this->add.plantable.targetTile = target;
}
