/*
 * spark.c - The Spark projectile (Java: com.mojang.ld22.entity.Spark).
 *
 * A short-lived projectile the Air Wizard fires: it hurts any mob it
 * touches except the wizard itself, and then expires.
 */
#include "spark.h"

#include "../gfx/color.h"
#include "../gfx/screen.h"
#include "../level/level.h"
#include "../utils/arraylist.h"
#include "airwizard.h"


/* Java: Spark.isBlockableBy(Mob mob) { return false; } */
PUBLIC boolean spark_is_blockable_by(Spark* this, Mob* mob) {
    (void) this;
    (void) mob;

    return false;
}


/*
 * Constructor: spawns the spark at the owner's position with the given
 * velocity and a lifetime of about 10 seconds plus a random extra.
 * Java: Spark(AirWizard owner, double xa, double ya)
 */
PUBLIC void spark_create(Spark* this, AirWizard* owner, double xa, double ya) {
    entity_create(&this->entity);   /* super() */

    /* What Spark overrides from Entity. Java: class Spark extends Entity */
    this->entity.tick            = (entity_tick_fn) spark_tick;
    this->entity.render          = (entity_render_fn) spark_render;
    this->entity.is_blockable_by = (entity_is_blockable_by_fn) spark_is_blockable_by;

    this->entity.type = SPARK;

    this->owner = owner;
    this->xx = this->entity.x = owner->mob.entity.x;
    this->yy = this->entity.y = owner->mob.entity.y;

    this->entity.xr = 0;
    this->entity.yr = 0;

    this->xa = xa;
    this->ya = ya;
    this->time = 0;
    this->lifeTime = 60 * 10 + this->entity.random.next_int(&this->entity.random, 30);
}


/*
 * Ages and moves the spark, hurting the mobs it overlaps, the Air Wizard
 * excluded, for 1 damage; it removes itself at the end of its life.
 */
PUBLIC void spark_tick(Spark* this) {
    ++this->time;

    if (this->time >= this->lifeTime) {
        entity_remove(&this->entity);
        return;
    }

    this->xx += this->xa;
    this->yy += this->ya;

    this->entity.x = (int) this->xx;
    this->entity.y = (int) this->yy;

    ArrayList toHit;

    arraylist_create(&toHit);

    this->entity.level->get_entities(this->entity.level, &toHit, this->entity.x, this->entity.y, this->entity.x, this->entity.y);

    for (int i = 0; i < toHit.size; ++i) {
        Entity* e = toHit.elements[i];

        if (entity_is_mob(e) && e->type != AIRWIZARD) {
            e->hurt(e, &this->owner->mob, 1, ((Mob*) e)->dir ^ 1);
        }
    }

    toHit.free(&toHit);
}


/* Draws the spark as two flickering tiles; it blinks during the last
 * seconds before expiring. */
PUBLIC void spark_render(Spark* this, Screen* screen) {
    if (this->time >= this->lifeTime - 6 * 20) {
        if (this->time / 6 % 2 == 0) return;
    }

    int xt = 8;
    int yt = 13;
    int x = this->entity.x;
    int y = this->entity.y;

    screen->render(screen, x - 4, y - 4 - 2, xt + yt * 32, get_color4(-1, 555, 555, 555), this->entity.random.next_int(&this->entity.random, 4));
    screen->render(screen, x - 4, y - 4 + 2, xt + yt * 32, get_color4(-1, 000, 000, 000), this->entity.random.next_int(&this->entity.random, 4));
}
