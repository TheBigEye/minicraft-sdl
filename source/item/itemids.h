/*
 * itemids.h - Item type tags for the Item union.
 */
#ifndef ITEMIDS_H
#define ITEMIDS_H 1

/* Discriminator selecting the active add.* payload in Item. */
typedef enum ItemID {
    FURNITURE,
    ITEM,
    POWERGLOVE,
    RESOURCE,
    TOOL
} ItemID;

#endif /* ITEMIDS_H */
