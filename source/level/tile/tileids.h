/*
 * tileids.h - The id of every tile type.
 *
 * Java identified each tile by its static field on Tile (`Tile.grass`,
 * `Tile.rock`, ...); here they are the values of this enum, which is also
 * the byte the level's map stores for each square.
 */
#ifndef LEVEL_TILE_TILEIDS_H_
#define LEVEL_TILE_TILEIDS_H_ 1

typedef enum tileid_ {
    GRASS,
    ROCK,
    WATER,
    FLOWER,
    TREE,
    DIRT,
    SAND,
    CACTUS,
    HOLE,
    TREE_SAPLING,
    CACTUS_SAPLING,
    FARMLAND,
    WHEAT,
    LAVA,
    STAIRS_DOWN,
    STAIRS_UP,
    INFINITE_FALL,
    CLOUD,
    HARD_ROCK,
    IRON_ORE,
    GOLD_ORE,
    GEM_ORE,
    CLOUD_CACTUS
} TileID;

#endif /* LEVEL_TILE_TILEIDS_H_ */
