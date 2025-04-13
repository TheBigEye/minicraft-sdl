#include "mob.h"
#include "../level/tile/tileids.h"
#include "../level/tile/tile.h"
#include "../inputhandler.h"
#include "_entity_caller.h"
#include "inventory.h"
#include "player.h"
#include "../item/powergloveitem.h"
#include "../screen/menu.h"
#include "particle/textparticle.h"
#include "workbench.h"
#include "../item/furniture_item.h"
#include "../item/tooltype.h"
#include "../item/tool_item.h"
#include "../item/resourceitem.h"
#include "../game.h"
#include "lantern.h"
#include "anvil.h"
#include "furnace.h"
#include "chest.h"
#include "../gfx/color.h"

void player_create(Player* player) {
	mob_create(&player->mob);

	player->mob.entity.type = PLAYER;
	player->attackTime = player->attackDir = 0;
	player->stamina = player->staminaRecharge = player->staminaRechargeDelay = 0;
	player->score = 0;
	player->onStairDelay = 0;
	player->invulnerableTime = 0;

	player->mob.entity.x = 24;
	player->mob.entity.y = 24;
	player->stamina = player->maxStamina = 10;
	inventory_create(&player->inventory);

	Item iWork;
	Workbench* workbench = malloc(sizeof(Workbench));
	workbench_create(workbench);
	furnitureitem_create(&iWork, workbench);
	inventory_addItem(&player->inventory, &iWork);

	Item glove;
	powergloveitem_create(&glove);
	inventory_addItem(&player->inventory, &glove);

    // -DTEST_INVENTORY
    #ifdef TEST_INVENTORY
        Item tool;

        Chest* chest = malloc(sizeof(Chest));
        chest_create(chest);
        furnitureitem_create(&tool, (Furniture *) chest);
        inventory_addItem(&player->inventory, &tool);

        Anvil* anvil = malloc(sizeof(Anvil));
        anvil_create(anvil);
        furnitureitem_create(&tool, (Furniture *) anvil);
        inventory_addItem(&player->inventory, &tool);

        Furnace* furnace = malloc(sizeof(Furnace));
        furnace_create(furnace);
        furnitureitem_create(&tool, (Furniture *) furnace);
        inventory_addItem(&player->inventory, &tool);

        resourceitem_create_cnt(&tool, &ironOre, 99);
        inventory_addItem(&player->inventory, &tool);

        resourceitem_create_cnt(&tool, &coal, 99);
        inventory_addItem(&player->inventory, &tool);

        toolitem_create(&tool, PICKAXE, 1);
        inventory_addItem(&player->inventory, &tool);

        toolitem_create(&tool, PICKAXE, 2);
        inventory_addItem(&player->inventory, &tool);

        toolitem_create(&tool, PICKAXE, 3);
        inventory_addItem(&player->inventory, &tool);

        toolitem_create(&tool, PICKAXE, 4);
        inventory_addItem(&player->inventory, &tool);


        toolitem_create(&tool, SWORD, 4);
        inventory_addItem(&player->inventory, &tool);

        toolitem_create(&tool, AXE, 1);
        inventory_addItem(&player->inventory, &tool);

        toolitem_create(&tool, SHOVEL, 1);
        inventory_addItem(&player->inventory, &tool);

        toolitem_create(&tool, HOE, 1);
        inventory_addItem(&player->inventory, &tool);

        Lantern* lantern = malloc(sizeof(Lantern));
        lantern_create(lantern);
        furnitureitem_create(&tool, (Furniture *) lantern);
        inventory_addItem(&player->inventory, &tool);
    #endif

	player->activeItem = 0;
	player->attackItem = 0;
}


int player_getAttackDamage(Player* player, Entity* e) {
	int dmg = random_next_int(&player->mob.entity.random, 3) + 1;
	if (player->attackItem) {
		dmg += item_getAttackDamageBonus(player->attackItem, e);
	}
	return dmg;
}


void player_hurt(Player* player, int x0, int y0, int x1, int y1) {
	ArrayList entities;
	create_arraylist(&entities);
	level_getEntities(player->mob.entity.level, &entities, x0, y0, x1, y1);

	for (int i = 0; i < entities.size; ++i) {
		Entity* entity = entities.elements[i];
		if (entity != player) {
            call_entity_hurt(entity, &player->mob, player_getAttackDamage(player, entity), player->attackDir);
        }
	}

	arraylist_remove(&entities);
}


char player_interact(Player* player, int x0, int y0, int x1, int y1) {
	ArrayList entities;
	create_arraylist(&entities);
	level_getEntities(player->mob.entity.level, &entities, x0, y0, x1, y1);

	for (int i = 0; i < entities.size; ++i) {
		Entity* entity = entities.elements[i];
		if (entity != player) {
			if (entity_interact(entity, player, player->activeItem, player->attackDir)) {
				arraylist_remove(&entities);
				return 1;
			}
		}
	}

	arraylist_remove(&entities);
	return 0;
}


void player_attack(Player* player){
	player->mob.walkDist += 8;
	player->attackDir = player->mob.dir;
	player->attackItem = player->activeItem;

	uint8_t done = 0;

	if (player->activeItem) {
		player->attackTime = 10;
		int yo = -2;
		int range = 12;
		int dir = player->mob.dir;
		int x = player->mob.entity.x;
		int y = player->mob.entity.y;

		if (dir == 0 && player_interact(player, x - 8, y + 4 + yo, x + 8, y + range + yo)) done = 1;
		if (dir == 1 && player_interact(player, x - 8, y - range + yo, x + 8, y - 4 + yo)) done = 1;
		if (dir == 3 && player_interact(player, x + 4, y - 8 + yo, x + range, y + 8 + yo)) done = 1;
		if (dir == 2 && player_interact(player, x - range, y - 8 + yo, x - 4, y + 8 + yo)) done = 1;

        if (done) {
            return;
        }

		int xt = x >> 4;
		int yt = (y + yo) >> 4;
		int r = 12;

        if (player->attackDir == 0) yt = (y + r + yo) >> 4;
		if (player->attackDir == 1) yt = (y - r + yo) >> 4;
		if (player->attackDir == 2) xt = (x - r) >> 4;
		if (player->attackDir == 3) xt = (x + r) >> 4;

		if (xt >= 0 && yt >= 0 && xt < player->mob.entity.level->w && yt < player->mob.entity.level->h) {
			TileID id = level_get_tile(player->mob.entity.level, xt, yt);
			if (item_interactOn(player->activeItem, id, player->mob.entity.level, xt, yt, player, player->attackDir)) {
				done = 1;
			} else {
				if (tile_interact(id, player->mob.entity.level, xt, yt, player, player->activeItem, player->attackDir)) {
					done = 1;
				}
			}

			if (item_isDepleted(player->activeItem)) {
				item_free(player->activeItem);
				free(player->activeItem);
				if (player->attackItem == player->activeItem) {
                    player->attackItem = 0;
                }
				player->activeItem = 0;
			}
		}
	}

	if (done) return;

	if (!player->activeItem || item_canAttack(player->activeItem)) {
		player->attackTime = 5;
		int yo = -2;
		int range = 20;
		int x = player->mob.entity.x;
		int y = player->mob.entity.y;

		if (player->mob.dir == 0) player_hurt(player, x - 8, y + 4 + yo, x + 8, y + range + yo);
		if (player->mob.dir == 1) player_hurt(player, x - 8, y - range + yo, x + 8, y - 4 + yo);
		if (player->mob.dir == 3) player_hurt(player, x + 4, y - 8 + yo, x + range, y + 8 + yo);
		if (player->mob.dir == 2) player_hurt(player, x - range, y - 8 + yo, x - 4, y + 8 + yo);

		int xt = player->mob.entity.x >> 4;
		int yt = (player->mob.entity.y + yo) >> 4;
		int r = 12;

		if (player->attackDir == 0) yt = (y + r + yo) >> 4;
		if (player->attackDir == 1) yt = (y - r + yo) >> 4;
		if (player->attackDir == 2) xt = (x - r) >> 4;
		if (player->attackDir == 3) xt = (x + r) >> 4;

		if (xt >= 0 && yt >= 0 && xt < player->mob.entity.level->w && yt < player->mob.entity.level->h) {
			TileID tile = level_get_tile(player->mob.entity.level, xt, yt);
			tile_hurt(tile, player->mob.entity.level, xt, yt, (Mob *) player, random_next_int(&player->mob.entity.random, 3) + 1, player->attackDir);
		}
	}
}


void player_doHurt(Player* player, int damage, int attackDir){
	if (player->mob.hurtTime > 0 || player->invulnerableTime > 0) {
        return;
    }

	// TODO: Sound.playerHurt.play();

    TextParticle* text_particle = malloc(sizeof(TextParticle));
	char* text = malloc(16);
	sprintf(text, "%d", damage);
	textparticle_create(text_particle, text, player->mob.entity.x, player->mob.entity.y, getColor4(-1, 504, 504, 504));
	level_addEntity(player->mob.entity.level, &text_particle->entity);

	player->mob.health -= damage;

	printf("%d\n", attackDir);
	if (attackDir == 0) player->mob.yKnockback = 6;
	if (attackDir == 1) player->mob.yKnockback = -6;
	if (attackDir == 2) player->mob.xKnockback = -6;
	if (attackDir == 3) player->mob.xKnockback = 6;

	player->mob.hurtTime = 10;
	player->invulnerableTime = 30;
}


void player_die(Player* player){
	mob_die(&player->mob);
	// TODO Sound.playerDeath.play();
}


char player_usexy(Player* player, int x0, int y0, int x1, int y1) {
	ArrayList entities;
	create_arraylist(&entities);
	level_getEntities(player->mob.entity.level, &entities, x0, y0, x1, y1);

	for (int i = 0; i < entities.size; ++i) {
		Entity* entity = entities.elements[i];
		if (entity != player) {
			if (call_entity_use(entity, player, player->attackDir)) {
				arraylist_remove(&entities);
				return 1;
			}
		}
	}

	arraylist_remove(&entities);
	return 0;
}


char player_use_(Player* player) {
	int yo = -2;
	int x = player->mob.entity.x;
	int y = player->mob.entity.y;
	int dir = player->mob.dir;

	if (dir == 0 && player_usexy(player, (x - 8), (y + 4) + yo, (x + 8), (y + 12) + yo)) return 1;
	if (dir == 1 && player_usexy(player, (x - 8), (y - 12) + yo, (x + 8), (y - 4) + yo)) return 1;
	if (dir == 3 && player_usexy(player, (x + 4), (y - 8) + yo, (x + 12), (y + 8) + yo)) return 1;
	if (dir == 2 && player_usexy(player, (x - 12), (y - 8) + yo, (x - 4), (y + 8) + yo)) return 1;

	int xt = x >> 4;
	int yt = (y + yo) >> 4;
	int r = 12;

	if (player->attackDir == 0) yt = (y + r + yo) >> 4;
	if (player->attackDir == 1) yt = (y - r + yo) >> 4;
	if (player->attackDir == 2) xt = (x - r) >> 4;
	if (player->attackDir == 3) xt = (x + r) >> 4;

	if (xt >= 0 && yt >= 0 && xt < player->mob.entity.level->w && yt < player->mob.entity.level->h) {
		TileID tile = level_get_tile(player->mob.entity.level, xt, yt);
		char c = tile_use(tile, player->mob.entity.level, xt, yt, player, player->attackDir);
		if (c) {
            return 1;
        }
	}
	return 0;
}


void player_gameWon(Player* player){
	player->invulnerableTime = 60*5;
	game_won();
}


void player_tick(Player* player){
	mob_tick(&player->mob);

    // -DGODMODE
    #ifdef GODMODE
        player->mob.health = player->mob.maxHealth;
        player->stamina = player->maxStamina;
    #endif

	if (player->invulnerableTime > 0) --player->invulnerableTime;

	int x = player->mob.entity.x;
	int y = player->mob.entity.y;

	unsigned char tile = level_get_tile(player->mob.entity.level, x >> 4, y >> 4);

	if (tile == STAIRS_DOWN || tile == STAIRS_UP) {
		if (player->onStairDelay == 0) {
			game_pendingLevelChange = tile == STAIRS_UP ? 1 : -1;
			player->onStairDelay = 10;
			return;
		}

		player->onStairDelay = 10;

	} else {
		if (player->onStairDelay > 0) {
            --player->onStairDelay;
        }
	}

	if (player->stamina <= 0 && player->staminaRechargeDelay == 0 && player->staminaRecharge == 0) {
		player->staminaRechargeDelay = 40;
	}

	if (player->staminaRechargeDelay > 0) {
		--player->staminaRechargeDelay;
	}

	if (player->staminaRechargeDelay == 0) {
		++player->staminaRecharge;
		if (call_entity_isSwimming(&player->mob.entity)) {
			player->staminaRecharge = 0;
		}

		while (player->staminaRecharge > 10) {
			player->staminaRecharge -= 10;
			if (player->stamina < player->maxStamina) {
                ++player->stamina;
            }
		}
	}

	int xa = 0;
	int ya = 0;

	if (up.down) --ya;
	if (down.down) ++ya;
	if (left.down) --xa;
	if (right.down) ++xa;

	if (call_entity_isSwimming(&player->mob.entity) && player->mob.tickTime % 60 == 0) {
		if (player->stamina > 0) {
            --player->stamina;
        } else {
            mob_hurt(&player->mob, &player->mob, 1, player->mob.dir ^ 1);
        }
	}

	if (player->staminaRechargeDelay % 2 == 0) {
		mob_move(&player->mob, xa, ya);
	}

	if (attack.clicked) {
		if (player->stamina == 0) {
            // Nothing :D
        } else{
			--player->stamina;
			player->staminaRecharge = 0;
			player_attack(player);
		}
	}

	if (menu.clicked) {
		if (!player_use_(player)) {
			game_set_menu(mid_INVENTORY);
		}
	}

	if (player->attackTime > 0) {
        --player->attackTime;
    }
}


char player_payStamina(Player* player, int cost){
	if (cost > player->stamina) {
        return 0;
    }

	player->stamina -= cost;
	return 1;
}


void player_render(Player* player, Screen* screen){
	int xt = 0;
	int yt = 14;

	int flip1 = (player->mob.walkDist >> 3) & 1;
	int flip2 = (player->mob.walkDist >> 3) & 1;

	if (player->mob.dir == 1) {
		xt += 2;
	}

	if (player->mob.dir > 1) {
		flip1 = 0;
		flip2 = ((player->mob.walkDist >> 4) & 1);

		if (player->mob.dir == 2) {
            flip1 = 1;
        }

		xt += 4 + ((player->mob.walkDist >> 3) & 1) * 2;
	}

	int xo = player->mob.entity.x - 8;
	int yo = player->mob.entity.y - 11;

	if (call_entity_isSwimming(&player->mob.entity)) {
		yo += 4;

		int waterColor = getColor4(-1, -1, 115, 335);
		if (player->mob.tickTime / 8 % 2 == 0) {
			waterColor = getColor4(-1, 335, 5, 115);
		}

		render_screen(screen, xo + 0, yo + 3, 5 + 13 * 32, waterColor, 0);
		render_screen(screen, xo + 8, yo + 3, 5 + 13 * 32, waterColor, 1);
	}

	if (player->attackTime > 0 && player->attackDir == 1) {
		render_screen(screen, xo + 0, yo - 4, 6 + 13 * 32, getColor4(-1, 555, 555, 555), 0);
		render_screen(screen, xo + 8, yo - 4, 6 + 13 * 32, getColor4(-1, 555, 555, 555), 1);

		if (player->attackItem) {
            item_renderIcon(player->attackItem, screen, xo + 4, yo - 4);
        }
	}

	int col = getColor4(-1, 100, 220, 532);
	if (player->mob.hurtTime > 0) {
        col = getColor4(-1, 555, 555, 555);
    }

	if (player->activeItem && player->activeItem->id == FURNITURE) {
        yt += 2;
    }

	render_screen(screen, xo + 8 * flip1, yo + 0, xt + yt*32, col, flip1);
	render_screen(screen, xo + 8 - 8 * flip1, yo + 0, xt + 1 + yt*32, col, flip1);

	if (!call_entity_isSwimming(&player->mob.entity)) {
		render_screen(screen, xo + 8 * flip2, yo + 8, xt + (yt + 1) * 32, col, flip2);
		render_screen(screen, xo + 8 - 8 * flip2, yo + 8, xt + 1 + (yt + 1) * 32, col, flip2);
	}

	if (player->attackTime > 0 && player->attackDir == 2) {
		render_screen(screen, xo - 4, yo, 7 + 13*32, getColor4(-1, 555, 555, 555), 1);
		render_screen(screen, xo - 4, yo + 8, 7 + 13*32, getColor4(-1, 555, 555, 555), 3);

		if (player->attackItem) {
            item_renderIcon(player->attackItem, screen, xo - 4, yo + 4);
        }
	}

	if (player->attackTime > 0 && player->attackDir == 3) {
		render_screen(screen, xo + 8 + 4, yo, 7 + 13 * 32, getColor4(-1, 555, 555, 555), 0);
		render_screen(screen, xo + 8 + 4, yo + 8, 7 + 13 * 32, getColor4(-1, 555, 555, 555), 2);

		if (player->attackItem) {
            item_renderIcon(player->attackItem, screen, xo + 8 + 4, yo + 4);
        }
	}

	if (player->attackTime > 0 && player->attackDir == 0) {
		render_screen(screen, xo + 0, yo + 8 + 4, 6 + 13 * 32, getColor4(-1, 555, 555, 555), 2);
		render_screen(screen, xo + 8, yo + 8 + 4, 6 + 13 * 32, getColor4(-1, 555, 555, 555), 3);

		if (player->attackItem) {
            item_renderIcon(player->attackItem, screen, xo + 4, yo + 8 + 4);
        }
	}

	if (player->activeItem && player->activeItem->id == FURNITURE) {
		player->activeItem->add.furniture.furniture->entity.x = player->mob.entity.x;
		player->activeItem->add.furniture.furniture->entity.y = yo;
		call_entity_render((Entity *) player->activeItem->add.furniture.furniture, screen);
	}
}


char player_findStartPos(Player* player, Level* level){
	Random* random = &player->mob.entity.random;

	while (1) {
		int x = random_next_int(random, level->w);
		int y = random_next_int(random, level->h);

		if (level_get_tile(level, x, y) == GRASS) {
			player->mob.entity.x = x * 16 + 8;
			player->mob.entity.y = y * 16 + 8;
			return 1;
		}
	}
}


void player_free(Player* player){
	char freeAttack = player->attackItem != 0;
    char freeActive = player->activeItem != 0;

	for (int i = 0; i < player->inventory.items.size; ++i) {
		Item* item = player->inventory.items.elements[i];

		if (item == player->activeItem) freeActive = 0;
		if (item == player->attackItem) freeAttack = 0;

		item_free(item);
		free(item);
	}

	free(player->inventory.items.elements);

	if (freeAttack && freeActive) {
		if (player->activeItem == player->attackItem) {
			item_free(player->activeItem);
			free(player->activeItem);

		} else {
			item_free(player->activeItem);
			free(player->activeItem);
			item_free(player->attackItem);
			free(player->attackItem);
		}

	} else if(freeAttack) {
		item_free(player->attackItem);
		free(player->attackItem);

	} else if(freeActive) {
		item_free(player->activeItem);
		free(player->activeItem);
	}
}
