/*
 * itementity.c - The ItemEntity (Java: com.mojang.ld22.entity.ItemEntity).
 *
 * The physical form of a dropped item: it pops with a small ballistic arc,
 * rests on the ground, expires after about 10 seconds and can be collected
 * by a player walking over it.
 */
#include "itementity.h"

#include "../gfx/color.h"
#include "../sound/sound.h"


/* Java: ItemEntity.touchedBy(entity) { if (time > 30) entity.touchItem(this); } */
PUBLIC void itementity_touched_by(ItemEntity* this, Entity* entity) {
    if (this->time > 30) {
        entity->touch_item(entity, this);
    }
}


/* Java: ItemEntity.isBlockableBy(Mob mob) { return false; } */
PUBLIC boolean itementity_is_blockable_by(ItemEntity* this, struct Mob* mob) {
    (void) this;
    (void) mob;

    return false;
}


/*
 * Constructor: spawns the dropped item at (x, y) with a random pop
 * velocity and a lifetime of 10 seconds plus up to one extra.
 */
PUBLIC void itementity_create(ItemEntity* this, Item item, int x, int y) {
    entity_create(&this->entity);   /* super() */

    /* What ItemEntity overrides from Entity.
     * Java: class ItemEntity extends Entity */
    this->entity.tick            = (entity_tick_fn) itementity_tick;
    this->entity.render          = (entity_render_fn) itementity_render;
    this->entity.touched_by      = (entity_touched_by_fn) itementity_touched_by;
    this->entity.is_blockable_by = (entity_is_blockable_by_fn) itementity_is_blockable_by;

    this->entity.type = ITEMENTITY;

    this->walkDist = 0;
    this->dir = 0;
    this->hurtTime = 0;
    this->xKnockback = this->yKnockback = 0;
    this->time = 0;

    this->item = item;
    this->xx = this->entity.x = x;
    this->yy = this->entity.y = y;

    this->entity.xr = 3;
    this->entity.yr = 3;

    this->zz = 2;
    this->xa = this->entity.random.next_gaussian(&this->entity.random) * 0.3;
    this->ya = this->entity.random.next_gaussian(&this->entity.random) * 0.2;
    this->za = this->entity.random.next_float(&this->entity.random) * 0.7 + 1;

    this->lifeTime = 60 * 10 + this->entity.random.next_int(&this->entity.random, 60);
}


/*
 * Ages and moves the dropped item: a ballistic pop with friction, then it
 * rests; it removes itself once the lifetime expires.
 */
PUBLIC void itementity_tick(ItemEntity* this) {
    ++this->time;

    if (this->time >= this->lifeTime) {
        entity_remove(&this->entity);
        return;
    }

    this->xx += this->xa;
    this->yy += this->ya;
    this->zz += this->za;

    if (this->zz < 0) {
        this->zz = 0;
        this->za *= -0.5;
        this->xa *= 0.6;
        this->ya *= 0.6;
    }

    this->za -= 0.15;

    int ox = this->entity.x;
    int oy = this->entity.y;
    int nx = this->xx;
    int ny = this->yy;
    int exceptedx = nx - this->entity.x;
    int exceptedy = ny - this->entity.y;

    entity_move(&this->entity, nx - this->entity.x, ny - this->entity.y);

    int gotx = this->entity.x - ox;
    int goty = this->entity.y - oy;

    this->xx += gotx - exceptedx;
    this->yy += goty - exceptedy;

    if (this->hurtTime > 0) --this->hurtTime;
}


/* Draws the item's icon with a small vertical bob and a shadow. */
PUBLIC void itementity_render(ItemEntity* this, Screen* screen) {
    /* It blinks for the last two seconds of its life. */
    if (this->time >= this->lifeTime - 6 * 20) {
        if (this->time / 6 % 2 == 0) return;
    }

    int x = this->entity.x;
    int y = this->entity.y;

    screen->render(screen, x - 4, y - 4, item_get_sprite(&this->item), get_color4(-1, 0, 0, 0), 0);
    screen->render(screen, x - 4, y - 4 - this->zz, item_get_sprite(&this->item), item_get_color(&this->item), 0);
}


/*
 * Pickup (Java: ItemEntity.take): plays the pickup sound, awards a score
 * point, lets the item react through onTake and removes it.
 */
PUBLIC void itementity_take(ItemEntity* this, Player* player) {
    /* Sound.pickup.play() */
    sound_play(SND_PICKUP);

    ++player->score;
    item_on_take(&this->item, this);
    entity_remove(&this->entity);
}
