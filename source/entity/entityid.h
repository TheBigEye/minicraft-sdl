/*
 * entityid.h - Runtime class tags for every entity type: the C
 *              replacement for Java's instanceof checks.
 */
#ifndef ENTITYID_H
#define ENTITYID_H 1

typedef enum EntityId {
    AIRWIZARD,
    ANVIL,
    CHEST,
    FURNACE,
    ITEMENTITY,
    LANTERN,
    OVEN,
    PLAYER,
    SLIME,
    SPARK,
    WORKBENCH,
    ZOMBIE,
    SMASHPARTICLE,
    TEXTPARTICLE,

    /* Value the base constructor `entity_create()` sets before the subclass
     * overwrites it: it keeps the field from being left uninitialized. */
    NONE
} EntityId;

#endif /* ENTITYID_H */
