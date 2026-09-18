/*
 * entity.c - The behaviour of the base Entity: the C counterpart of the
 *            body of Java's com.mojang.ld22.entity.Entity.
 *
 * It holds the default implementations every subclass inherits, plus the
 * methods the original Java never overrode: init, intersects, move, move2,
 * interact and remove.
 */
#include "entity.h"

#include "player.h"

#include "../item/item.h"
#include "../level/tile/tile.h"
#include "../level/tile/tileids.h"
#include "../log.h"


/*
 * Constructor: installs the base class methods, that is, Java's Entity
 * ones, and sets the common fields. Subclasses call this first, their
 * `super()`, and then override the methods they need. The default
 * collision radius is 6 pixels, as in the original.
 */
PUBLIC void entity_create(Entity* this) {
    this->tick             = entity_tick;
    this->render           = entity_render;
    this->blocks           = entity_blocks;
    this->hurt             = entity_hurt;
    this->hurt_tile        = entity_hurt_tile;
    this->touched_by       = entity_touched_by;
    this->is_blockable_by  = entity_is_blockable_by;
    this->touch_item       = entity_touch_item;
    this->can_swim         = entity_can_swim;
    this->use              = entity_use;
    this->get_light_radius = entity_get_light_radius;
    this->die              = entity_die;
    this->do_hurt          = entity_do_hurt;
    this->is_swimming      = entity_is_swimming;
    this->free             = entity_free;

    this->x = this->y = 0;
    this->xr = this->yr = 6;
    this->removed = false;
    this->level = null;
    this->type = NONE;

    random_create(&this->random);
    this->random.set_seed(&this->random, get_time_ms());
}


/*
 * Applies an item's interact behaviour to this entity, for instance
 * hitting a mob with a tool. It delegates to the item's own logic.
 */
PUBLIC boolean entity_interact(Entity* this, Player* player, Item* item, int attackDir) {
    return item_interact(item, player, this, attackDir);
}


/* Marks the entity for removal; the level drops it on the next pass. */
PUBLIC void entity_remove(Entity* this) {
    this->removed = true;
}


/* ------------------------------------------------------------------ */
/* Base class implementations: the body of Java's Entity.            */
/* Each one is the behaviour a subclass inherits when it does not     */
/* install an override of its own.                                    */
/* ------------------------------------------------------------------ */

/* Base tick(): does nothing; subclasses drive their own updates. */
PUBLIC void entity_tick(Entity* this) {
    /* public void tick() {} */
    (void) this;
}


/* Base render(): invisible by default; subclasses draw themselves. */
PUBLIC void entity_render(Entity* this, Screen* screen) {
    /* public void render(Screen screen) {} */
    (void) this;
    (void) screen;
}


/* Base blocks(): entities never block others; Mob and Furniture override. */
PUBLIC boolean entity_blocks(Entity* this, Entity* other) {
    /* public boolean blocks(Entity e) { return false; } */
    (void) this;
    (void) other;

    return false;
}


/* Base hurt(): immune to mob attacks unless a subclass says otherwise. */
PUBLIC void entity_hurt(Entity* this, Mob* source, int dmg, int attackDir) {
    /* public void hurt(Mob mob, int dmg, int attackDir) {} */
    (void) this;
    (void) source;
    (void) dmg;
    (void) attackDir;
}


/* Base hurt_tile(): immune to tile damage, mining or hoeing, by default. */
PUBLIC void entity_hurt_tile(Entity* this, TileID tile, int x, int y, int dmg) {
    /* public void hurt(Tile tile, int x, int y, int dmg) {} */
    (void) this;
    (void) tile;
    (void) x;
    (void) y;
    (void) dmg;
}


/* Base touched_by(): no reaction to being touched by another entity. */
PUBLIC void entity_touched_by(Entity* this, Entity* other) {
    /* protected void touchedBy(Entity entity) {} */
    (void) this;
    (void) other;
}


/* Base is_blockable_by(): any mob's movement is blocked by this entity. */
PUBLIC boolean entity_is_blockable_by(Entity* this, Mob* mob) {
    /* public boolean isBlockableBy(Mob mob) { return true; } */
    (void) this;
    (void) mob;

    return true;
}


/* Base touch_item(): ignores item pickups; Player overrides to collect. */
PUBLIC void entity_touch_item(Entity* this, struct ItemEntity* item) {
    /* public void touchItem(ItemEntity itemEntity) {} */
    (void) this;
    (void) item;
}


/* Base canSwim(): cannot enter water; Player overrides. */
PUBLIC boolean entity_can_swim(Entity* this) {
    /* public boolean canSwim() { return false; } */
    (void) this;

    return false;
}


/* Base use(): cannot be used by the player; Furniture overrides. */
PUBLIC boolean entity_use(Entity* this, Player* player, int attackDir) {
    /* public boolean use(Player player, int attackDir) { return false; } */
    (void) this;
    (void) player;
    (void) attackDir;

    return false;
}


/* Base get_light_radius(): emits no light; Lantern overrides with 8. */
PUBLIC int entity_get_light_radius(Entity* this) {
    /* public int getLightRadius() { return 0; } */
    (void) this;

    return 0;
}


/* Base die(): unreachable in practice, as only Mobs die; diagnostic only. */
PUBLIC void entity_die(Entity* this) {
    /* Only Mobs can die in the original; keep the old diagnostic. */
    LOG_WARN("Tried dying undyable entity (wat)! %d", (int) this->type);
}


/* Base do_hurt(): same as die(), a diagnostic for logic bugs. */
PUBLIC void entity_do_hurt(Entity* this, int damage, int attackDir) {
    /* Only Mobs can be hurt in the original; keep the old diagnostic. */
    (void) damage;
    (void) attackDir;

    LOG_WARN("Tried hurting unhurtable entity! %d", (int) this->type);
}


/* Base is_swimming(): a Mob-only query; non-mobs are never swimming. */
PUBLIC boolean entity_is_swimming(Entity* this) {
    /* Mob-only method; non-mobs never swim. */
    (void) this;

    return false;
}


/* Base free(): the plain Entity owns no heap resources. */
PUBLIC void entity_free(Entity* this) {
    /* C-specific: default entities own no heap resources. */
    (void) this;
}


/* Attaches the entity to a level; called by level->add(). */
PUBLIC void entity_init(Entity* this, Level* level) {
    this->level = level;
}


/*
 * True if the entity's bounding box, the centre plus or minus xr and yr,
 * intersects the given tile-space rectangle: Java's Entity.intersects().
 */
PUBLIC boolean entity_intersects(Entity* this, int x0, int y0, int x1, int y1) {
    return !(this->x + this->xr < x0 || this->y + this->yr < y0 || this->x - this->xr > x1 || this->y - this->yr > y1);
}


/*
 * Single-axis movement with collision: Java's Entity.move2().
 *
 * Coordinates: entities live in pixel space, 2048x2048, while tiles live
 * in tile space, 128x128; each tile is 16x16 pixels, so ">> 4" turns
 * pixels into tiles and the range of tiles touched by the move is swept
 * for blocking ones. The tiles entered are notified through bumpedInto(),
 * then the entities overlapping the destination are asked, through
 * touchedBy() and blocks(), whether they allow the move. On success the
 * position is updated.
 */
PUBLIC boolean entity_move2(Entity* this, int xa, int ya) {
    if (xa && ya) {
        LOG_WARN("Entity(%d) called move2 with xa and ya != 0!", (int) this->type);
    }

    /* Tile box currently occupied... */
    int xto0 = (this->x - this->xr) >> 4;
    int yto0 = (this->y - this->yr) >> 4;
    int xto1 = (this->x + this->xr) >> 4;
    int yto1 = (this->y + this->yr) >> 4;

    /* ...and the tile box it would occupy after the move. */
    int xt0 = (this->x + xa - this->xr) >> 4;
    int yt0 = (this->y + ya - this->yr) >> 4;
    int xt1 = (this->x + xa + this->xr) >> 4;
    int yt1 = (this->y + ya + this->yr) >> 4;

    boolean blocked = false;

    /* Sweep the destination box; tiles already occupied are skipped. */
    for (int yt = yt0; yt <= yt1; ++yt) {
        for (int xt = xt0; xt <= xt1; ++xt) {

            if (xt >= xto0 && xt <= xto1 && yt >= yto0 && yt <= yto1) {
                continue;
            }

            Tile* tile = this->level->get_tile(this->level, xt, yt);

            tile->bumped_into(tile, this->level, xt, yt, this);

            /* Re-get the tile, in case bumpedInto changed it. */
            tile = this->level->get_tile(this->level, xt, yt);

            if (!tile->may_pass(tile, this->level, xt, yt, this)) {
                blocked = true;
                return false;
            }
        }
    }

    if (blocked) return false;

    ArrayList wasInside, isInside;

    arraylist_create(&wasInside);
    arraylist_create(&isInside);

    int x = this->x;
    int xr = this->xr;
    int y = this->y;
    int yr = this->yr;

    /* The entities overlapped before and after the move. */
    this->level->get_entities(this->level, &wasInside, x - xr, y - yr, x + xr, y + yr);
    this->level->get_entities(this->level, &isInside, x + xa - xr, y + ya - yr, x + xa + xr, y + ya + yr);

    /* Notify the newly touched entities: item pickup, the Air Wizard... */
    for (int i = 0; i < isInside.size; ++i) {
        Entity* e = isInside.elements[i];

        if (e == this) continue;

        /* Java: e.touchedBy(this) */
        e->touched_by(e, this);
    }

    /* An entity already overlapping before the move cannot block it. */
    for (int j = 0; j < wasInside.size; ++j) {
        for (int i = 0; i < isInside.size; ++i) {
            Entity* a = isInside.elements[i];
            Entity* b = wasInside.elements[j];

            if (a == b) {
                isInside.remove_at(&isInside, i--);
            }
        }
    }

    wasInside.free(&wasInside);

    /* Any remaining overlapping entity that blocks us stops the move. */
    for (int i = 0; i < isInside.size; ++i) {
        Entity* e = isInside.elements[i];

        if (e == this) continue;

        /* Java: e.blocks(this) */
        if (e->blocks(e, this)) {
            isInside.free(&isInside);
            return false;
        }
    }

    isInside.free(&isInside);

    this->x += xa;
    this->y += ya;

    return true;
}


/*
 * A full movement step: Java's Entity.move(). Diagonal movement is split
 * into two single-axis move2() passes so that collisions stay
 * axis-aligned. When the entity ends up on a new tile, that tile is
 * notified through steppedOn(), which is how saplings, flowers and the
 * stairs react. Returns whether it moved.
 */
PUBLIC boolean entity_move(Entity* this, int xa, int ya) {
    if (xa || ya) {
        boolean stopped = true;

        if (xa != 0 && entity_move2(this, xa, 0)) stopped = false;
        if (ya != 0 && entity_move2(this, 0, ya)) stopped = false;

        if (!stopped) {
            int xt = this->x >> 4;
            int yt = this->y >> 4;

            Tile* tile = this->level->get_tile(this->level, xt, yt);

            tile->stepped_on(tile, this->level, xt, yt, this);
        }

        return !stopped;
    }

    return true;
}
