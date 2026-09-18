/*
 * player.h - The player character
 *            (Java: com.mojang.ld22.entity.Player).
 *
 * A Mob plus inventory, stamina, the selected and the swung items, and the
 * interaction boxes used to attack, use and interact with the world.
 */
#ifndef PLAYER_H
#define PLAYER_H 1

#include "inventory.h"
#include "mob.h"

#include "../utils/javalang.h"
#include "../level/level.h"

struct Item;
struct ItemEntity;

typedef struct Player Player;

struct Player {
    /* Inheritance: Mob, always the first member. Java: extends Mob */
    Mob mob;

    /* Swing animation ticks and the direction it points at.
     * Java: `private int attackTime, attackDir` */
    int attackTime, attackDir;
    /* Java: `public Inventory inventory = new Inventory()` */
    Inventory inventory;
    /* Item swung and item held. Java: `public Item attackItem, activeItem` */
    struct Item* attackItem, *activeItem;
    /* Java: `public int stamina, staminaRecharge, staminaRechargeDelay` */
    int stamina, staminaRecharge, staminaRechargeDelay;
    /* Java: `public int score` */
    int score;
    /* Java: `public int maxStamina = 10` */
    int maxStamina;
    /* Cooldown between stair level transitions.
     * Java: `private int onStairDelay` */
    int onStairDelay;
    /* Post-hit and post-win invulnerability.
     * Java: `public int invulnerableTime = 0` */
    int invulnerableTime;
};

/*
 * Two fields of Java's Player have no counterpart here: `private
 * InputHandler input` and `public Game game`. The port keeps both as
 * globals (inputhandler.h and game.h), so they are not stored per player.
 */

/* Constructor: 10 hp, 10 stamina, workbench plus power glove.
 * Java: Player(Game game, InputHandler input) */
PUBLIC void player_create(Player* this);

/* Java: Player.tick() */
PUBLIC void player_tick(Player* this);

/* Java: Player.render(Screen) */
PUBLIC void player_render(Player* this, Screen* screen);

/* Java: Player.touchItem(ItemEntity) */
PUBLIC void player_touch_item(Player* this, struct ItemEntity* itemEntity);

/* Players always can. Java: Player.canSwim() { return true; } */
PUBLIC boolean player_can_swim(Player* this);

/* Java: Player.findStartPos(Level) */
PUBLIC boolean player_find_start_pos(Player* this, Level* level);

/* Java: Player.payStamina(int) */
PUBLIC boolean player_pay_stamina(Player* this, int cost);

/* 2, or the carried furniture's radius. Java: Player.getLightRadius() */
PUBLIC int player_get_light_radius(Player* this);

/* Java: Player.die() */
PUBLIC void player_die(Player* this);

/* Java: Player.touchedBy(Entity) */
PUBLIC void player_touched_by(Player* this, Entity* entity);

/* Java: Mob.doHurt(int, int), overridden by Player. */
PUBLIC void player_do_hurt(Player* this, int damage, int attackDir);

/* Marks the game as won. Java: Player.gameWon() */
PUBLIC void player_game_won(Player* this);

/* C-only: releases everything the player owns. */
PUBLIC void player_free(Player* this);

/*
 * The helpers Java marks private are deliberately absent from this header:
 * attack(), use(), use(int, int, int, int), interact(int, int, int, int),
 * hurt(int, int, int, int) and getAttackDamage(Entity). They are PRIVATE in
 * player.c, since nothing outside it calls them.
 *
 * use() and use(int, int, int, int) are Java overloads of one name, so
 * player.c defines a single name for them as well; see javalang.h.
 */

#endif /* PLAYER_H */
