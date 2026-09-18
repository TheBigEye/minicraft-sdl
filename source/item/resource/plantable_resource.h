/*
 * plantable_resource.h - The extra data of a plantable resource
 *                        (Java: com.mojang.ld22.item.resource.PlantableResource).
 */
#ifndef ITEM_RESOURCE_PLANTABLE_RESOURCE_H_
#define ITEM_RESOURCE_PLANTABLE_RESOURCE_H_

#include "../../utils/javalang.h"
#include "../../level/tile/tile.h"

struct Resource;

/*
 * Extra per-resource state for plantables.
 *
 * Java keeps `private List<Tile> sourceTiles`; the port keeps a borrowed
 * array plus its length, because the tables are static and never freed.
 */
typedef struct res_plantable res_plantable;

struct res_plantable {
    /* Number of tiles this may be planted on. */
    int sourceTilesSize;
    /* Tiles this may be planted on. Java: `private List<Tile> sourceTiles` */
    TileID* sourceTiles;
    /* Tile the source becomes when planted. Java: `private Tile targetTile` */
    TileID targetTile;
};

/*
 * Constructor: fills the Resource fields, then the planting rules.
 * Java: PlantableResource(String, int, int, Tile targetTile, Tile... sourceTiles)
 */
PUBLIC void init_plantable_resource(struct Resource* this, const char* name, int sprite, int color, TileID target, TileID* sources, int size);

#endif /* ITEM_RESOURCE_PLANTABLE_RESOURCE_H_ */
