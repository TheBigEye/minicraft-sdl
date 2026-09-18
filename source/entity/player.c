/*
 * player.c - The player character
 *            (Java: com.mojang.ld22.entity.Player).
 *
 * Input-driven movement, stamina economy, the three interaction boxes
 * (attack / use / interact), swimming, stair transitions and the sprite
 * composition (walk cycle, swim overlay, swing arcs, carried furniture).
 */
#include <stdio.h>
#include <stdlib.h>

#include "../game.h"
#include "../gfx/color.h"
#include "../inputhandler.h"
#include "../item/furniture_item.h"
#include "../item/powergloveitem.h"
#include "../item/resourceitem.h"
#include "../item/tool_item.h"
#include "../item/tooltype.h"
#include "../level/tile/tile.h"
#include "../level/tile/tileids.h"
#include "../log.h"
#include "../screen/menu.h"
#include "../sound/sound.h"

#include "anvil.h"
#include "chest.h"
#include "furnace.h"
#include "inventory.h"
#include "itementity.h"
#include "lantern.h"
#include "mob.h"
#include "particle/textparticle.h"
#include "player.h"
#include "workbench.h"


/*
 * The helpers Java marks private. They are declared here because
 * player_create() installs none of them into a method slot and nothing
 * outside this file calls them.
 */

/* Java: private int Player.getAttackDamage(Entity e) */
PRIVATE int player_get_attack_damage(Player* this, Entity* e);
/* Java: private void Player.attack() */
PRIVATE void player_attack(Player* this);
/* Java: private boolean Player.interact(int, int, int, int) */
PRIVATE boolean player_interact(Player* this, int x0, int y0, int x1, int y1);
/* Java: private void Player.hurt(int, int, int, int) */
PRIVATE void player_hurt(Player* this, int x0, int y0, int x1, int y1);
/* Java: private boolean Player.use(int, int, int, int) */
PRIVATE boolean player_use_5(Player* this, int x0, int y0, int x1, int y1);
/* Java: private boolean Player.use() */
PRIVATE boolean player_use_1(Player* this);

/*
 * Java: use() is one name overloaded on its argument list. Callers write
 * player_use(this) or player_use(this, x0, y0, x1, y1); the macro picks
 * between the two implementations above. See javalang.h.
 */
#define player_use(...)   LANG_OVERLOAD(player_use, __VA_ARGS__)


/*
 * Constructor (Java: Player(Game game, InputHandler input)).
 *
 * Base mob init, 10 stamina, empty inventory plus the two starting items of
 * the original game (a workbench furniture item and the power glove).
 * -DTEST_INVENTORY fills the inventory with every item type for debugging.
 */
PUBLIC void player_create(Player* this) {
    mob_create(&this->mob);   /* super() */

    /* What the class Player overrides. Java: class Player extends Mob */
    this->mob.entity.tick             = (entity_tick_fn) player_tick;
    this->mob.entity.render           = (entity_render_fn) player_render;
    this->mob.entity.touched_by       = (entity_touched_by_fn) player_touched_by;
    this->mob.entity.touch_item       = (entity_touch_item_fn) player_touch_item;
    this->mob.entity.can_swim         = (entity_can_swim_fn) player_can_swim;
    this->mob.entity.get_light_radius = (entity_get_light_radius_fn) player_get_light_radius;
    this->mob.entity.die              = (entity_die_fn) player_die;
    this->mob.entity.do_hurt          = (entity_do_hurt_fn) player_do_hurt;
    this->mob.entity.free             = (entity_free_fn) player_free;

    this->mob.entity.type = PLAYER;
    this->mob.entity.x = 24;
    this->mob.entity.y = 24;

    this->attackTime = this->attackDir = 0;
    this->stamina = this->staminaRecharge = this->staminaRechargeDelay = 0;
    this->score = 0;
    this->onStairDelay = 0;
    this->invulnerableTime = 0;
    this->stamina = this->maxStamina = 10;

    inventory_create(&this->inventory);

    Item iWork;
    Workbench* workbench = new(Workbench);

    workbench_create(workbench);
    furnitureitem_create(&iWork, workbench);
    inventory_add(&this->inventory, &iWork);

    Item glove;

    powergloveitem_create(&glove);
    inventory_add(&this->inventory, &glove);

    /* -DTEST_INVENTORY */
    #ifdef TEST_INVENTORY
        Item tool;

        Chest* chest = new(Chest);

        chest_create(chest);
        furnitureitem_create(&tool, (Furniture *) chest);
        inventory_add(&this->inventory, &tool);

        Anvil* anvil = new(Anvil);

        anvil_create(anvil);
        furnitureitem_create(&tool, (Furniture *) anvil);
        inventory_add(&this->inventory, &tool);

        Furnace* furnace = new(Furnace);

        furnace_create(furnace);
        furnitureitem_create(&tool, (Furniture *) furnace);
        inventory_add(&this->inventory, &tool);

        resourceitem_create_cnt(&tool, &ironOre, 99);
        inventory_add(&this->inventory, &tool);

        resourceitem_create_cnt(&tool, &coal, 99);
        inventory_add(&this->inventory, &tool);

        toolitem_create(&tool, PICKAXE, 1);
        inventory_add(&this->inventory, &tool);

        toolitem_create(&tool, PICKAXE, 2);
        inventory_add(&this->inventory, &tool);

        toolitem_create(&tool, PICKAXE, 3);
        inventory_add(&this->inventory, &tool);

        toolitem_create(&tool, PICKAXE, 4);
        inventory_add(&this->inventory, &tool);

        toolitem_create(&tool, SWORD, 4);
        inventory_add(&this->inventory, &tool);

        toolitem_create(&tool, AXE, 1);
        inventory_add(&this->inventory, &tool);

        toolitem_create(&tool, SHOVEL, 1);
        inventory_add(&this->inventory, &tool);

        toolitem_create(&tool, HOE, 1);
        inventory_add(&this->inventory, &tool);

        Lantern* lantern = new(Lantern);

        lantern_create(lantern);
        furnitureitem_create(&tool, (Furniture *) lantern);
        inventory_add(&this->inventory, &tool);
    #endif

    this->activeItem = null;
    this->attackItem = null;
}


/*
 * Per-tick player update
 * (Java: Player.tick()).
 *
 * Base mob tick, stair detection (standing on stairs queues a level change
 * after a short delay), stamina recharge with its delay and the swim pause,
 * input driven movement (halved while the recharge delay runs), swimming
 * stamina drain with drowning damage, and the attack/menu keys.
 */
PUBLIC void player_tick(Player* this) {
    mob_tick(&this->mob);   /* super.tick() */

    /* -DGODMODE */
    #ifdef GODMODE
        this->mob.health = this->mob.maxHealth;
        this->stamina = this->maxStamina;
    #endif

    if (this->invulnerableTime > 0) --this->invulnerableTime;

    int x = this->mob.entity.x;
    int y = this->mob.entity.y;

    Tile* onTile = this->mob.entity.level->get_tile(this->mob.entity.level, x >> 4, y >> 4);

    if (onTile == tiles[STAIRS_DOWN] || onTile == tiles[STAIRS_UP]) {
        if (this->onStairDelay == 0) {
            /*
             * Java: changeLevel((onTile == Tile.stairsUp) ? 1 : -1);
             * The port queues the change instead of running it here, so
             * that the level is not swapped in the middle of a tick.
             */
            game_pendingLevelChange = onTile == tiles[STAIRS_UP] ? 1 : -1;
            this->onStairDelay = 10;
            return;
        }

        this->onStairDelay = 10;
    } else {
        if (this->onStairDelay > 0) --this->onStairDelay;
    }

    /* Stamina floor reached: wait a bit before recharging at all. */
    if (this->stamina <= 0 && this->staminaRechargeDelay == 0 && this->staminaRecharge == 0) {
        this->staminaRechargeDelay = 40;
    }

    if (this->staminaRechargeDelay > 0) --this->staminaRechargeDelay;

    if (this->staminaRechargeDelay == 0) {
        ++this->staminaRecharge;

        if (this->mob.entity.is_swimming(&this->mob.entity)) {
            this->staminaRecharge = 0;
        }

        while (this->staminaRecharge > 10) {
            this->staminaRecharge -= 10;

            if (this->stamina < this->maxStamina) ++this->stamina;
        }
    }

    int xa = 0;
    int ya = 0;

    if (up.down) --ya;
    if (down.down) ++ya;
    if (left.down) --xa;
    if (right.down) ++xa;

    /* Swimming costs one stamina per second; empty stamina drowns. */
    if (this->mob.entity.is_swimming(&this->mob.entity) && this->mob.tickTime % 60 == 0) {
        if (this->stamina > 0) {
            --this->stamina;
        } else {
            mob_hurt(&this->mob, &this->mob, 1, this->mob.dir ^ 1);
        }
    }

    if (this->staminaRechargeDelay % 2 == 0) {
        mob_move(&this->mob, xa, ya);
    }

    if (attack.clicked) {
        if (this->stamina == 0) {
            /* Nothing :D */
        } else {
            --this->stamina;
            this->staminaRecharge = 0;
            player_attack(this);
        }
    }

    if (menu.clicked) {
        if (!player_use(this)) {
            game_set_menu(mid_INVENTORY);
        }
    }

    if (this->attackTime > 0) --this->attackTime;
}


/*
 * The attack action
 * (Java: private void Player.attack()).
 *
 * With an item held, first tries interact() on entities in the facing box,
 * then the item's and the tile's own interact hooks in front of the player;
 * depleted items (broken tools) are freed. If nothing accepted the swing, it
 * becomes a plain attack: entities in a wider box take damage and the tile
 * in front is hurt (mining/hitting).
 */
PRIVATE void player_attack(Player* this) {
    this->mob.walkDist += 8;
    this->attackDir = this->mob.dir;
    this->attackItem = this->activeItem;

    boolean done = false;

    if (this->activeItem) {
        this->attackTime = 10;

        int yo = -2;
        int range = 12;

        if (this->mob.dir == 0 && player_interact(this, this->mob.entity.x - 8, this->mob.entity.y + 4 + yo, this->mob.entity.x + 8, this->mob.entity.y + range + yo)) done = true;
        if (this->mob.dir == 1 && player_interact(this, this->mob.entity.x - 8, this->mob.entity.y - range + yo, this->mob.entity.x + 8, this->mob.entity.y - 4 + yo)) done = true;
        if (this->mob.dir == 3 && player_interact(this, this->mob.entity.x + 4, this->mob.entity.y - 8 + yo, this->mob.entity.x + range, this->mob.entity.y + 8 + yo)) done = true;
        if (this->mob.dir == 2 && player_interact(this, this->mob.entity.x - range, this->mob.entity.y - 8 + yo, this->mob.entity.x - 4, this->mob.entity.y + 8 + yo)) done = true;

        if (done) return;

        int xt = this->mob.entity.x >> 4;
        int yt = (this->mob.entity.y + yo) >> 4;
        int r = 12;

        if (this->attackDir == 0) yt = (this->mob.entity.y + r + yo) >> 4;
        if (this->attackDir == 1) yt = (this->mob.entity.y - r + yo) >> 4;
        if (this->attackDir == 2) xt = (this->mob.entity.x - r) >> 4;
        if (this->attackDir == 3) xt = (this->mob.entity.x + r) >> 4;

        if (xt >= 0 && yt >= 0 && xt < this->mob.entity.level->w && yt < this->mob.entity.level->h) {
            Tile* tile = this->mob.entity.level->get_tile(this->mob.entity.level, xt, yt);

            if (item_interact_on(this->activeItem, tile->id, this->mob.entity.level, xt, yt, this, this->attackDir)) {
                done = true;
            } else {
                if (tile->interact(tile, this->mob.entity.level, xt, yt, this, this->activeItem, this->attackDir)) {
                    done = true;
                }
            }

            if (item_is_depleted(this->activeItem)) {
                /*
                 * C-only: Java only does `activeItem = null` here and lets
                 * the collector deal with the rest. Here the item is owned,
                 * so it has to be freed, and every reference to it has to go
                 * first: attackItem was pointed at it at the top of this
                 * function, and player_render() keeps drawing it while
                 * attackTime is still counting down.
                 *
                 * The references are dropped BEFORE the free, never after:
                 * `delete()` clears its own argument, so an
                 * `if (attackItem == activeItem)` written after the delete
                 * would be comparing against null and would leave
                 * attackItem dangling.
                 */
                Item* spent = this->activeItem;

                if (this->attackItem == spent) this->attackItem = null;

                this->activeItem = null;

                item_free(spent);
                delete(spent);
            }
        }
    }

    if (done) return;

    if (!this->activeItem || item_can_attack(this->activeItem)) {
        this->attackTime = 5;

        int yo = -2;
        int range = 20;

        if (this->mob.dir == 0) player_hurt(this, this->mob.entity.x - 8, this->mob.entity.y + 4 + yo, this->mob.entity.x + 8, this->mob.entity.y + range + yo);
        if (this->mob.dir == 1) player_hurt(this, this->mob.entity.x - 8, this->mob.entity.y - range + yo, this->mob.entity.x + 8, this->mob.entity.y - 4 + yo);
        if (this->mob.dir == 3) player_hurt(this, this->mob.entity.x + 4, this->mob.entity.y - 8 + yo, this->mob.entity.x + range, this->mob.entity.y + 8 + yo);
        if (this->mob.dir == 2) player_hurt(this, this->mob.entity.x - range, this->mob.entity.y - 8 + yo, this->mob.entity.x - 4, this->mob.entity.y + 8 + yo);

        int xt = this->mob.entity.x >> 4;
        int yt = (this->mob.entity.y + yo) >> 4;
        int r = 12;

        if (this->attackDir == 0) yt = (this->mob.entity.y + r + yo) >> 4;
        if (this->attackDir == 1) yt = (this->mob.entity.y - r + yo) >> 4;
        if (this->attackDir == 2) xt = (this->mob.entity.x - r) >> 4;
        if (this->attackDir == 3) xt = (this->mob.entity.x + r) >> 4;

        if (xt >= 0 && yt >= 0 && xt < this->mob.entity.level->w && yt < this->mob.entity.level->h) {
            Tile* tile = this->mob.entity.level->get_tile(this->mob.entity.level, xt, yt);

            tile->hurt(tile, this->mob.entity.level, xt, yt, (Mob *) this, this->mob.entity.random.next_int(&this->mob.entity.random, 3) + 1, this->attackDir);
        }
    }
}


/*
 * The menu/use key action
 * (Java: private boolean Player.use()).
 *
 * Tries use() on entities in the facing box, then use() on the tile in
 * front (doors, stairs, ...). Returns whether anything happened; the caller
 * opens the inventory otherwise.
 */
PRIVATE boolean player_use_1(Player* this) {
    int yo = -2;

    if (this->mob.dir == 0 && player_use(this, this->mob.entity.x - 8, this->mob.entity.y + 4 + yo, this->mob.entity.x + 8, this->mob.entity.y + 12 + yo)) return true;
    if (this->mob.dir == 1 && player_use(this, this->mob.entity.x - 8, this->mob.entity.y - 12 + yo, this->mob.entity.x + 8, this->mob.entity.y - 4 + yo)) return true;
    if (this->mob.dir == 3 && player_use(this, this->mob.entity.x + 4, this->mob.entity.y - 8 + yo, this->mob.entity.x + 12, this->mob.entity.y + 8 + yo)) return true;
    if (this->mob.dir == 2 && player_use(this, this->mob.entity.x - 12, this->mob.entity.y - 8 + yo, this->mob.entity.x - 4, this->mob.entity.y + 8 + yo)) return true;

    int xt = this->mob.entity.x >> 4;
    int yt = (this->mob.entity.y + yo) >> 4;
    int r = 12;

    if (this->attackDir == 0) yt = (this->mob.entity.y + r + yo) >> 4;
    if (this->attackDir == 1) yt = (this->mob.entity.y - r + yo) >> 4;
    if (this->attackDir == 2) xt = (this->mob.entity.x - r) >> 4;
    if (this->attackDir == 3) xt = (this->mob.entity.x + r) >> 4;

    if (xt >= 0 && yt >= 0 && xt < this->mob.entity.level->w && yt < this->mob.entity.level->h) {
        Tile* tile = this->mob.entity.level->get_tile(this->mob.entity.level, xt, yt);

        if (tile->use(tile, this->mob.entity.level, xt, yt, this, this->attackDir)) return true;
    }

    return false;
}


/*
 * Offers use() to every entity in the box
 * (Java: private boolean Player.use(int x0, int y0, int x1, int y1));
 * the first entity that accepts wins.
 */
PRIVATE boolean player_use_5(Player* this, int x0, int y0, int x1, int y1) {
    ArrayList entities;

    arraylist_create(&entities);
    this->mob.entity.level->get_entities(this->mob.entity.level, &entities, x0, y0, x1, y1);

    for (int i = 0; i < entities.size; ++i) {
        Entity* e = entities.elements[i];

        if (e != (Entity*) this) {
            if (e->use(e, this, this->attackDir)) {
                entities.free(&entities);
                return true;
            }
        }
    }

    entities.free(&entities);   /* C-only: Java's list is garbage collected. */

    return false;
}


/*
 * Offers the active item to every entity in the box via interact()
 * (Java: private boolean Player.interact(int x0, int y0, int x1, int y1));
 * stops at the first entity that accepts it.
 */
PRIVATE boolean player_interact(Player* this, int x0, int y0, int x1, int y1) {
    ArrayList entities;

    arraylist_create(&entities);
    this->mob.entity.level->get_entities(this->mob.entity.level, &entities, x0, y0, x1, y1);

    for (int i = 0; i < entities.size; ++i) {
        Entity* e = entities.elements[i];

        if (e != (Entity*) this) {
            if (entity_interact(e, this, this->activeItem, this->attackDir)) {
                entities.free(&entities);
                return true;
            }
        }
    }

    entities.free(&entities);   /* C-only: Java's list is garbage collected. */

    return false;
}


/*
 * Hurts every entity (except the player) inside the attack box
 * (Java: private void Player.hurt(int x0, int y0, int x1, int y1)), using
 * the player's current attack damage and swing direction.
 */
PRIVATE void player_hurt(Player* this, int x0, int y0, int x1, int y1) {
    ArrayList entities;

    arraylist_create(&entities);
    this->mob.entity.level->get_entities(this->mob.entity.level, &entities, x0, y0, x1, y1);

    for (int i = 0; i < entities.size; ++i) {
        Entity* e = entities.elements[i];

        if (e != (Entity*) this) {
            e->hurt(e, &this->mob, player_get_attack_damage(this, e), this->attackDir);
        }
    }

    entities.free(&entities);   /* C-only: Java's list is garbage collected. */
}


/*
 * Attack damage dealt to `e`
 * (Java: private int Player.getAttackDamage(Entity e)):
 * 1-3 random plus the held item's bonus.
 */
PRIVATE int player_get_attack_damage(Player* this, Entity* e) {
    int dmg = this->mob.entity.random.next_int(&this->mob.entity.random, 3) + 1;

    if (this->attackItem) {
        dmg += item_get_attack_damage_bonus(this->attackItem, e);
    }

    return dmg;
}


/*
 * Draws the player
 * (Java: Player.render(Screen)).
 *
 * Walk-cycle frame selection from walkDist, direction-based sprite column,
 * swim overlay (legs hidden under animated water), white flash while hurt,
 * the four directional swing arcs with the swung item's icon, and the
 * carried furniture rendered on top when holding one.
 */
PUBLIC void player_render(Player* this, Screen* screen) {
    int xt = 0;
    int yt = 14;

    int flip1 = (this->mob.walkDist >> 3) & 1;
    int flip2 = (this->mob.walkDist >> 3) & 1;

    if (this->mob.dir == 1) {
        xt += 2;
    }

    if (this->mob.dir > 1) {
        flip1 = 0;
        flip2 = ((this->mob.walkDist >> 4) & 1);

        if (this->mob.dir == 2) {
            flip1 = 1;
        }

        xt += 4 + ((this->mob.walkDist >> 3) & 1) * 2;
    }

    int xo = this->mob.entity.x - 8;
    int yo = this->mob.entity.y - 11;

    if (this->mob.entity.is_swimming(&this->mob.entity)) {
        yo += 4;

        int waterColor = get_color4(-1, -1, 115, 335);

        if (this->mob.tickTime / 8 % 2 == 0) {
            waterColor = get_color4(-1, 335, 5, 115);
        }

        screen->render(screen, xo + 0, yo + 3, 5 + 13 * 32, waterColor, 0);
        screen->render(screen, xo + 8, yo + 3, 5 + 13 * 32, waterColor, 1);
    }

    if (this->attackTime > 0 && this->attackDir == 1) {
        screen->render(screen, xo + 0, yo - 4, 6 + 13 * 32, get_color4(-1, 555, 555, 555), 0);
        screen->render(screen, xo + 8, yo - 4, 6 + 13 * 32, get_color4(-1, 555, 555, 555), 1);

        if (this->attackItem) {
            item_render_icon(this->attackItem, screen, xo + 4, yo - 4);
        }
    }

    int col = get_color4(-1, 100, 220, 532);

    if (this->mob.hurtTime > 0) {
        col = get_color4(-1, 555, 555, 555);
    }

    if (this->activeItem && this->activeItem->id == FURNITURE) {
        yt += 2;
    }

    screen->render(screen, xo + 8 * flip1, yo + 0, xt + yt * 32, col, flip1);
    screen->render(screen, xo + 8 - 8 * flip1, yo + 0, xt + 1 + yt * 32, col, flip1);

    if (!this->mob.entity.is_swimming(&this->mob.entity)) {
        screen->render(screen, xo + 8 * flip2, yo + 8, xt + (yt + 1) * 32, col, flip2);
        screen->render(screen, xo + 8 - 8 * flip2, yo + 8, xt + 1 + (yt + 1) * 32, col, flip2);
    }

    if (this->attackTime > 0 && this->attackDir == 2) {
        screen->render(screen, xo - 4, yo, 7 + 13 * 32, get_color4(-1, 555, 555, 555), 1);
        screen->render(screen, xo - 4, yo + 8, 7 + 13 * 32, get_color4(-1, 555, 555, 555), 3);

        if (this->attackItem) {
            item_render_icon(this->attackItem, screen, xo - 4, yo + 4);
        }
    }

    if (this->attackTime > 0 && this->attackDir == 3) {
        screen->render(screen, xo + 8 + 4, yo, 7 + 13 * 32, get_color4(-1, 555, 555, 555), 0);
        screen->render(screen, xo + 8 + 4, yo + 8, 7 + 13 * 32, get_color4(-1, 555, 555, 555), 2);

        if (this->attackItem) {
            item_render_icon(this->attackItem, screen, xo + 8 + 4, yo + 4);
        }
    }

    if (this->attackTime > 0 && this->attackDir == 0) {
        screen->render(screen, xo + 0, yo + 8 + 4, 6 + 13 * 32, get_color4(-1, 555, 555, 555), 2);
        screen->render(screen, xo + 8, yo + 8 + 4, 6 + 13 * 32, get_color4(-1, 555, 555, 555), 3);

        if (this->attackItem) {
            item_render_icon(this->attackItem, screen, xo + 4, yo + 8 + 4);
        }
    }

    if (this->activeItem && this->activeItem->id == FURNITURE) {
        Furniture* furniture = this->activeItem->add.furniture.furniture;
        Entity* furn = &furniture->entity;

        furniture->entity.x = this->mob.entity.x;
        furniture->entity.y = yo;
        furn->render(furn, screen);
    }
}


/*
 * Walking over an item entity takes it and stores it in the inventory
 * (Java: Player.touchItem(ItemEntity itemEntity)).
 */
PUBLIC void player_touch_item(Player* this, ItemEntity* itemEntity) {
    itementity_take(itemEntity, this);
    inventory_add(&this->inventory, &itemEntity->item);
}


/* Players can always swim (Java: Player.canSwim() { return true; }). */
PUBLIC boolean player_can_swim(Player* this) {
    (void) this;

    return true;
}


/*
 * Spawns the player on a random grass tile
 * (Java: Player.findStartPos(Level)); it retries until one is found, and
 * the surface level always has plenty, so this terminates fast.
 */
PUBLIC boolean player_find_start_pos(Player* this, Level* level) {
    Random* random = &this->mob.entity.random;

    while (true) {
        int x = random->next_int(random, level->w);
        int y = random->next_int(random, level->h);

        if (level->get_tile(level, x, y) == tiles[GRASS]) {
            this->mob.entity.x = x * 16 + 8;
            this->mob.entity.y = y * 16 + 8;

            return true;
        }
    }
}


/* Pays `cost` stamina points for an action (Java: Player.payStamina(int)). */
PUBLIC boolean player_pay_stamina(Player* this, int cost) {
    if (cost > this->stamina) return false;

    this->stamina -= cost;

    return true;
}


/*
 * Light emitted by the player
 * (Java: Player.getLightRadius()): 2 by itself, or the carried furniture's
 * radius when holding something that shines (a lantern, say).
 */
PUBLIC int player_get_light_radius(Player* this) {
    int r = 2;

    if (this->activeItem && this->activeItem->id == FURNITURE) {
        Furniture* furniture = this->activeItem->add.furniture.furniture;
        int rr = furniture->entity.get_light_radius(&furniture->entity);

        if (rr > r) r = rr;
    }

    return r;
}


/* Player death (Java: Player.die()): base mob removal plus the death sound. */
PUBLIC void player_die(Player* this) {
    mob_die(&this->mob);   /* super.die() */
    sound_play(SND_PLAYERDEATH);   /* Java: Sound.playerDeath.play() */
}


/*
 * When a non-player entity touches the player, the notification is bounced
 * back to that entity
 * (Java: protected void Player.touchedBy(Entity entity)).
 */
PUBLIC void player_touched_by(Player* this, Entity* entity) {
    if (entity->type != PLAYER) {
        entity->touched_by(entity, (Entity*) this);
    }
}


/*
 * Damage received by the player
 * (Java: protected void Player.doHurt(int damage, int attackDir)).
 *
 * Ignored while hurt or invulnerable; otherwise plays the hurt sound, shows
 * a damage number, subtracts health, applies knockback and opens both the
 * short hurt window (10 ticks) and the longer invulnerability one (30).
 */
PUBLIC void player_do_hurt(Player* this, int damage, int attackDir) {
    if (this->mob.hurtTime > 0 || this->invulnerableTime > 0) return;

    sound_play(SND_PLAYERHURT);   /* Java: Sound.playerHurt.play() */

    TextParticle* text_particle = new(TextParticle);
    String text = new_array(char, 16);

    sprintf(text, "%d", damage);
    textparticle_create(text_particle, text, this->mob.entity.x, this->mob.entity.y, get_color4(-1, 504, 504, 504));
    this->mob.entity.level->add(this->mob.entity.level, &text_particle->entity);

    this->mob.health -= damage;

    LOG_TRACE("player hurt: %d damage, attackDir %d", damage, attackDir);

    if (attackDir == 0) this->mob.yKnockback = +6;
    if (attackDir == 1) this->mob.yKnockback = -6;
    if (attackDir == 2) this->mob.xKnockback = -6;
    if (attackDir == 3) this->mob.xKnockback = +6;

    this->mob.hurtTime = 10;
    this->invulnerableTime = 30;
}


/* Called when the Air Wizard dies (Java: Player.gameWon()): makes the
 * player invulnerable for the win jingle and starts the win sequence. */
PUBLIC void player_game_won(Player* this) {
    this->invulnerableTime = 60 * 5;
    game_won();
}


/*
 * Releases everything the player owns: every inventory item, then the
 * held/attack items only if they were not part of the inventory (they
 * usually are), avoiding double frees. C-only.
 */
PUBLIC void player_free(Player* this) {
    for (int i = 0; i < this->inventory.items.size; ++i) {
        Item* item = this->inventory.items.elements[i];

        if (item == this->activeItem) this->activeItem = null;
        if (item == this->attackItem) this->attackItem = null;

        item_free(item);
        delete(item);
    }

    delete(this->inventory.items.elements);

    /*
     * Whatever is still referenced was held outside the inventory. The two
     * may be the same object, in which case it must only be freed once.
     */
    if (this->activeItem == this->attackItem) this->attackItem = null;

    if (this->activeItem) {
        item_free(this->activeItem);
        delete(this->activeItem);
    }

    if (this->attackItem) {
        item_free(this->attackItem);
        delete(this->attackItem);
    }
}
