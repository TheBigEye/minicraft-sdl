/*
 * airwizard.c - The Air Wizard boss (Java: com.mojang.ld22.entity.AirWizard).
 *
 * The game's final challenge: it wanders the sky island and fires spark
 * projectiles at the player in attack cycles. Killing it ends the game
 * with a win.
 */
#include "airwizard.h"

#include <math.h>
#include <stdlib.h>

#include "../game.h"
#include "../gfx/color.h"
#include "../gfx/screen.h"
#include "../level/level.h"
#include "../sound/sound.h"
#include "spark.h"


/* Constructor: spawns the wizard at a random spot with the boss health pool. */
PUBLIC void airwizard_create(AirWizard* this) {
    mob_create(&this->mob);   /* super(): Mob already installs its methods */

    /* What AirWizard overrides from Mob.
     * Java: class AirWizard extends Mob */
    this->mob.entity.tick       = (entity_tick_fn) airwizard_tick;
    this->mob.entity.render     = (entity_render_fn) airwizard_render;
    this->mob.entity.touched_by = (entity_touched_by_fn) airwizard_touched_by;
    this->mob.entity.die        = (entity_die_fn) airwizard_die;
    this->mob.entity.do_hurt    = (entity_do_hurt_fn) airwizard_do_hurt;

    this->mob.entity.type = AIRWIZARD;
    this->mob.entity.x = this->mob.entity.random.next_int(&this->mob.entity.random, 64 * 16);
    this->mob.entity.y = this->mob.entity.random.next_int(&this->mob.entity.random, 64 * 16);
    this->mob.health = this->mob.maxHealth = 2000;

    this->xa = this->ya = 0;
    this->randomWalkTime = 0;
    this->attackDelay = 0;
    this->attackTime = 0;
    this->attackType = 0;
}


/*
 * Boss AI (Java: AirWizard.tick): random walking interleaved with attack
 * cycles that spawn sparks aimed around the player; the shape of the
 * cycle depends on the current attackType.
 */
PUBLIC void airwizard_tick(AirWizard* this) {
    mob_tick(&this->mob);   /* Java: super.tick() */

    if (this->attackDelay > 0) {
        this->mob.dir = (this->attackDelay - 45) / 4 % 4;
        this->mob.dir = (this->mob.dir * 2 % 4) + (this->mob.dir / 2);

        if (this->attackDelay < 45) this->mob.dir = 0;

        --this->attackDelay;

        if (this->attackDelay == 0) {
            this->attackType = 0;

            if (this->mob.health < 1000) this->attackType = 1;
            if (this->mob.health < 200) this->attackType = 2;

            this->attackTime = 60 * 2;
        }

        return;
    }

    if (this->attackTime > 0) {
        --this->attackTime;

        double dir = this->attackTime * 0.25 * (this->attackTime % 2 * 2 - 1);
        double speed = 0.7 + this->attackType * 0.2;

        Spark* spark = new(Spark);

        spark_create(spark, this, cos(dir) * speed, sin(dir) * speed);
        this->mob.entity.level->add(this->mob.entity.level, &spark->entity);
        return;
    }

    int speed = (this->mob.tickTime % 4) == 0 ? 0 : 1;

    if (!mob_move(&this->mob, this->xa * speed, this->ya * speed)
            || this->mob.entity.random.next_int(&this->mob.entity.random, 100) == 0) {
        this->randomWalkTime = 30;
        this->xa = this->mob.entity.random.next_int(&this->mob.entity.random, 3) - 1;
        this->ya = this->mob.entity.random.next_int(&this->mob.entity.random, 3) - 1;
    }

    if (this->randomWalkTime > 0) {
        --this->randomWalkTime;

        if (game_player->mob.entity.level == this->mob.entity.level && this->randomWalkTime == 0) {
            int xd = game_player->mob.entity.x - this->mob.entity.x;
            int yd = game_player->mob.entity.y - this->mob.entity.y;

            if (this->mob.entity.random.next_int(&this->mob.entity.random, 4) == 0 && xd * xd + yd * yd < 50 * 50) {
                if (this->attackDelay == 0 && this->attackTime == 0) {
                    this->attackDelay = 60 * 2;
                }
            }
        }
    }
}


/* A hit always starts an attack cycle. */
PUBLIC void airwizard_do_hurt(AirWizard* this, int damage, int attackDir) {
    mob_do_hurt(&this->mob, damage, attackDir);   /* Java: super.doHurt() */

    if (this->attackDelay == 0 && this->attackTime == 0) {
        this->attackDelay = 60 * 2;
    }
}


/* Draws the wizard sprite with its walk animation and facing. */
PUBLIC void airwizard_render(AirWizard* this, Screen* screen) {
    int xt = 8;
    int yt = 14;

    int flip1 = (this->mob.walkDist >> 3) & 1;
    int flip2 = (this->mob.walkDist >> 3) & 1;

    if (this->mob.dir == 1) xt += 2;

    if (this->mob.dir > 1) {
        flip1 = 0;
        flip2 = (this->mob.walkDist >> 4) & 1;

        if (this->mob.dir == 2) flip1 = 1;

        xt += 4 + ((this->mob.walkDist >> 3) & 1) * 2;
    }

    int xo = this->mob.entity.x - 8;
    int yo = this->mob.entity.y - 11;

    int col1 = get_color4(-1, 100, 500, 555);
    int col2 = get_color4(-1, 100, 500, 532);

    /* It flashes red as it gets closer to dying. */
    if (this->mob.health < 200) {
        if (this->mob.tickTime / 3 % 2 == 0) {
            col1 = get_color4(-1, 500, 100, 555);
            col2 = get_color4(-1, 500, 100, 532);
        }
    } else if (this->mob.health < 1000) {
        if (this->mob.tickTime / 5 % 4 == 0) {
            col1 = get_color4(-1, 500, 100, 555);
            col2 = get_color4(-1, 500, 100, 532);
        }
    }

    if (this->mob.hurtTime > 0) {
        col1 = get_color4(-1, 555, 555, 555);
        col2 = get_color4(-1, 555, 555, 555);
    }

    screen->render(screen, xo + 8 * flip1, yo + 0, xt + yt * 32, col1, flip1);
    screen->render(screen, xo + 8 - 8 * flip1, yo + 0, xt + 1 + yt * 32, col1, flip1);
    screen->render(screen, xo + 8 * flip2, yo + 8, xt + (yt + 1) * 32, col2, flip2);
    screen->render(screen, xo + 8 - 8 * flip2, yo + 8, xt + 1 + (yt + 1) * 32, col2, flip2);
}


/* Hurts the player on contact, for 3 damage. */
PUBLIC void airwizard_touched_by(AirWizard* this, Entity* entity) {
    if (entity->type == PLAYER) {
        entity->hurt(entity, &this->mob, 3, this->mob.dir);
    }
}


/*
 * Boss death (Java: AirWizard.die): it awards 1000 score, starts the win
 * sequence through player_game_won() and plays bossdeath.
 */
PUBLIC void airwizard_die(AirWizard* this) {
    mob_die(&this->mob);   /* Java: super.die() */

    if (game_player->mob.entity.level == this->mob.entity.level) {
        game_player->score += 1000;
        player_game_won(game_player);
    }

    /* Sound.bossdeath.play() */
    sound_play(SND_BOSSDEATH);
}
