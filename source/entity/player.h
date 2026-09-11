/*
 * player.h - The player character (Java: com.mojang.ld22.entity.Player).
 *
 * A Mob plus inventory, stamina, selected/attacking item and the
 * interaction boxes used to attack, use and interact with the world.
 */
#ifndef PLAYER_H
#define PLAYER_H

#include "inventory.h"
#include "mob.h"

#include "../level/level.h"

struct _Item;
struct _ItemEntity;

typedef struct _Player{
	Mob mob;
	int attackTime, attackDir;  /* swing animation ticks and its direction */
	Inventory inventory;
	struct _Item* attackItem, *activeItem; /* item swung / item held */
	int stamina, staminaRecharge, staminaRechargeDelay;
	int score;
	int maxStamina;
	int onStairDelay;      /* cooldown between stair level transitions */
	int invulnerableTime;  /* post-hit and post-win invulnerability */
} Player;

/* Initializes the player: 10 hp, 10 stamina, workbench + power glove. */
void player_create(Player* player);
/* Per-tick update: input, stamina, stairs, swimming, attack/menu keys. */
void player_tick(Player* player);

/* Swings the held item: interact on entities/tiles, else hurt area. */
void player_attack(Player* player);
/* Calls use() on entities inside the box; first success wins. */
char player_usexy(Player* player, int x0, int y0, int x1, int y1);
/* Calls interact() with the active item inside the box. */
char player_interact(Player* player, int x0, int y0, int x1, int y1);
/* Hurts every entity inside the box with the player's attack damage. */
void player_hurt(Player* player, int x0, int y0, int x1, int y1);
/* 1-3 base damage plus the held item's bonus. */
int player_getAttackDamage(Player* player, Entity* entity);
/* Draws the player, swim overlay, swing arc and carried furniture. */
void player_render(Player* player, Screen* screen);
/* Finds a grass tile to spawn on (loops until one appears). */
char player_findStartPos(Player* player, Level* level);
/* Subtracts `cost` stamina if available; returns success. */
char player_payStamina(Player* player, int cost);
/* Player death: removes the mob and plays the death sound. */
void player_die(Player *player);

/* Damage received: invulnerability windows, sound, popup, knockback. */
void player_doHurt(Player* player, int damage, int attackDir);
void player_die(Player* mob);
/* Marks the game as won: long invulnerability + win sequence. */
void player_gameWon(Player* player);
/* Releases inventory and held items without double frees. */
void player_free(Player* player);

/* Virtual overrides (declared for the vtable; mirror Player.java) */
char player_canSwim(Player* player);        /* players always can */
void player_touchItem(Player* player, struct _ItemEntity* item); /* pick up */
void player_touchedBy(Player* player, Entity* entity); /* forward to non-players */
int  player_getLightRadius(Player* player); /* 2, or carried furniture's */

#endif // PLAYER_H
