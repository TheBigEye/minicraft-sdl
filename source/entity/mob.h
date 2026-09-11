/*
 * mob.h - Base class of everything alive: player, slime, zombie and the
 *         Air Wizard (Java: com.mojang.ld22.entity.Mob).
 *
 * Adds health, facing direction, knockback, hurt cooldown and swimming
 * on top of Entity, plus the shared movement/hurt/die behavior that the
 * concrete mobs reuse through mob_vtable.
 */
#ifndef MOB_H
#define MOB_H

#include "entity.h"
#include "../level/tile/tileids.h"

struct _Level;

typedef struct _Mob{
	Entity entity;
	int walkDist;           /* steps taken; drives the walk animation */
	int dir;                /* facing: 0 down, 1 up, 2 left, 3 right */
	int hurtTime;           /* invulnerability ticks left after a hit */
	int xKnockback, yKnockback; /* pending knockback pixels per axis */
	int maxHealth;
	int health;
	int swimTimer;          /* ticks in water/lava; halves move rate */
	int tickTime;           /* total ticks alive */
} Mob;

/* Mob's vtable (the Java `Mob` class); subclass vtables inherit these entries. */
extern const EntityVTable mob_vtable;

/* The C equivalent of Java's `e instanceof Mob`. */
char entity_ismob(Entity* entity);

/* Initializes mob state: 10 hp, small collision box, facing down. */
void mob_create(Mob* mob);
/* Per-tick update: lava damage, death check, hurt cooldown. */
void mob_tick(Mob* mob);
/* Removes the mob from the level (Java: Mob.die()). */
void mob_die(Mob* mob);
/* Movement with knockback, swim pacing and facing update. */
uint8_t mob_move(Mob* mob, int xa, int ya);
/* True while standing in water or lava. */
uint8_t mob_isSwimming(Mob* mob);
/* A mob blocks entities that report isBlockableBy(mob). */
char mob_blocks(Mob* mob, Entity* entity);
/* Restores health (capped at maxHealth) with a green number popup. */
void mob_heal(Mob* mob, int heal);
/* Applies damage: red number popup, knockback and hurt cooldown. */
void mob_doHurt(Mob* mob, int damage, int attackDir);
/* Picks a random spawn tile far from the player and other mobs. */
char mob_findStartPos(Mob* mob, struct _Level* level);

/* Damage coming from a tile (lava, cactus...): forwards to doHurt. */
void mob_hurtTile(Mob* mob, TileID tile, int x, int y, int damage);
/* Damage coming from another mob: forwards to the virtual doHurt. */
void mob_hurt(Mob* mob, Mob* by, int damage, int attackDir);

#endif // MOB_H
