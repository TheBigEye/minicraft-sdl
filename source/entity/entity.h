#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include "entityid.h"
#include "../level/tile/tileids.h"

#include "../gfx/screen.h"
#include "../utils/javarandom.h"
#include "../utils/utils.h"

struct _Mob;
struct _Player;
struct _Item;
struct _Level;
struct _ItemEntity;

typedef struct _Entity Entity;

/*
 * Virtual method table — the C equivalent of Java's dynamic dispatch.
 *
 * Every Entity subclass (Mob, Player, Slime, Furniture, ItemEntity, ...)
 * defines a `static const EntityVTable <class>_vtable` in its own .c file,
 * exactly mirroring the methods of the matching class in the original
 * Minicraft (com.mojang.ld22.entity.*), and assigns it in its create().
 *
 * Inheritance works by struct embedding (Entity is always the FIRST member:
 *   Entity <- Mob <- Player/Slime/Zombie/AirWizard
 *   Entity <- Furniture <- Anvil/Chest/Furnace/Lantern/Oven/Workbench
 *   Entity <- ItemEntity / Spark / SmashParticle / TextParticle
 * so a subclass pointer is also a valid Entity pointer) plus copying the
 * parent's function pointers into the child's vtable initializer.
 *
 * Methods that are never overridden in the original Java (move, move2,
 * remove, init, intersects, interact) stay plain functions.
 */

typedef void (*vt_tick_fn)(Entity* self);
typedef void (*vt_render_fn)(Entity* self, Screen* screen);
typedef char (*vt_blocks_fn)(Entity* self, Entity* other);
typedef void (*vt_hurt_fn)(Entity* self, struct _Mob* source, int dmg, int attackDir);
typedef void (*vt_hurtTile_fn)(Entity* self, TileID tile, int x, int y, int dmg);
typedef void (*vt_touchedBy_fn)(Entity* self, struct _Entity* other);
typedef char (*vt_isBlockableBy_fn)(Entity* self, struct _Mob* mob);
typedef void (*vt_touchItem_fn)(Entity* self, struct _ItemEntity* item);
typedef char (*vt_canSwim_fn)(Entity* self);
typedef char (*vt_use_fn)(Entity* self, struct _Player* player, int attackDir);
typedef int  (*vt_getLightRadius_fn)(Entity* self);
typedef void (*vt_die_fn)(Entity* self);
typedef void (*vt_doHurt_fn)(Entity* self, int damage, int attackDir);
typedef char (*vt_isSwimming_fn)(Entity* self);
typedef void (*vt_free_fn)(Entity* self);

typedef struct _EntityVTable {
	/* --- com.mojang.ld22.entity.Entity methods --- */
	vt_tick_fn           tick;            /* Entity.tick()            */
	vt_render_fn         render;          /* Entity.render(Screen)    */
	vt_blocks_fn         blocks;          /* Entity.blocks(Entity)    */
	vt_hurt_fn           hurt;            /* Entity.hurt(Mob,int,int) */
	vt_hurtTile_fn       hurtTile;        /* Entity.hurt(Tile,x,y,int)*/
	vt_touchedBy_fn      touchedBy;       /* Entity.touchedBy(Entity) */
	vt_isBlockableBy_fn  isBlockableBy;   /* Entity.isBlockableBy(Mob)*/
	vt_touchItem_fn      touchItem;       /* Entity.touchItem(ItemEntity) */
	vt_canSwim_fn        canSwim;         /* Entity.canSwim()         */
	vt_use_fn            use;             /* Entity.use(Player,int)   */
	vt_getLightRadius_fn getLightRadius;  /* Entity.getLightRadius()  */
	/* --- com.mojang.ld22.entity.Mob methods --- */
	vt_die_fn            die;             /* Mob.die()                */
	vt_doHurt_fn         doHurt;          /* Mob.doHurt(int,int)      */
	vt_isSwimming_fn     isSwimming;      /* Mob.isSwimming()         */
	/* --- C-specific lifecycle --- */
	vt_free_fn           free;            /* frees owned resources (NOT the struct itself) */
} EntityVTable;

typedef struct _Entity{
	const EntityVTable* vt;   /* virtual methods — set by every *_create() */
	EntityId type;            /* class tag (used where Java used instanceof) */
	Random random;
	int x, y;
	int xr, yr;
	struct _Level* level;
	uint8_t removed;
} Entity;

/* Base vtable with the default (Java Entity) implementations. */
extern const EntityVTable entity_vtable;

void entity_create(Entity* entity);
void entity_remove(Entity* entity);
void entity_init(Entity* entity, struct _Level* level);
uint8_t entity_intersects(Entity* entity, int x0, int y0, int x1, int y1);
uint8_t entity_move(Entity* entity, int xa, int ya);
uint8_t entity_move2(Entity* entity, int xa, int ya);
uint8_t entity_interact(Entity* entity, struct _Player* player, struct _Item* item, int attackDir);

/*
 * Default virtual-method implementations (= the Java Entity base class).
 * Declared here so subclass vtables can inherit them by name, e.g.:
 *     static const EntityVTable slime_vtable = {
 *         .canSwim = (vt_canSwim_fn) entity_canSwim, ...
 *     };
 */
void entity_tick(Entity* entity);
void entity_render(Entity* entity, Screen* screen);
char entity_blocks(Entity* entity, Entity* other);
void entity_hurt(Entity* entity, struct _Mob* source, int dmg, int attackDir);
void entity_hurtTile(Entity* entity, TileID tile, int x, int y, int dmg);
void entity_touchedBy(Entity* entity, Entity* other);
char entity_isBlockableBy(Entity* entity, struct _Mob* mob);
void entity_touchItem(Entity* entity, struct _ItemEntity* item);
char entity_canSwim(Entity* entity);
char entity_use(Entity* entity, struct _Player* player, int attackDir);
int  entity_getLightRadius(Entity* entity);
void entity_die(Entity* entity);
void entity_doHurt(Entity* entity, int damage, int attackDir);
char entity_isSwimming(Entity* entity);
void entity_free(Entity* entity);

#endif // ENTITY_H
