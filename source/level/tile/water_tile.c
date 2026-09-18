/*
 * water_tile.c - Behaviour of water (Java: tile.WaterTile).
 *
 * Crossable only by swimming. It is drawn animated: each quadrant's sprite
 * is picked with a Random seeded from the position and the tick, so the
 * surface moves without keeping any state.
 */
#include "tile.h"
#include "water_tile.h"

#include "../../gfx/color.h"
#include "../../utils/javarandom.h"

/*
 * The animation generator (Java: the `Random lRandom` local to render()).
 * It is created once and reseeded on every blit.
 */
STATIC Random wRandom;


/* Constructor: water connects to sand and to itself. */
PUBLIC void watertile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render   = watertile_render;
    this->tick     = watertile_tick;
    this->may_pass = watertile_may_pass;

    this->connects_to_sand  = true;
    this->connects_to_water = true;

    random_create(&wRandom);
}


/* Crossable only by swimming. Java: WaterTile.mayPass() { return e.canSwim(); } */
PUBLIC boolean watertile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;

    return e->can_swim(e);
}


/* Animated surface, with edges against sand or ground. */
PUBLIC void watertile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    wRandom.set_seed(&wRandom, (tile_tick_count + (x / 2 - y) * 4311) / 10 * 54687121 + x * 3271612 + y * 3412987161);

    int col = get_color4(5, 5, 115, 115);
    int transitionColor1 = get_color4(3, 5, level->dirtColor - 111, level->dirtColor);
    int transitionColor2 = get_color4(3, 5, level->sandColor - 111, level->sandColor);

    Tile* up    = level->get_tile(level, x, y - 1);
    Tile* down  = level->get_tile(level, x, y + 1);
    Tile* left  = level->get_tile(level, x - 1, y);
    Tile* right = level->get_tile(level, x + 1, y);

    boolean u = !up->connects_to_water;
    boolean d = !down->connects_to_water;
    boolean l = !left->connects_to_water;
    boolean r = !right->connects_to_water;

    boolean su = u && up->connects_to_sand;
    boolean sd = d && down->connects_to_sand;
    boolean sl = l && left->connects_to_sand;
    boolean sr = r && right->connects_to_sand;

    if (!u && !l) {
        screen->render(screen, x * 16 + 0, y * 16 + 0, wRandom.next_int(&wRandom, 4), col, wRandom.next_int(&wRandom, 4));
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 0, (l ? 14 : 15) + (u ? 0 : 1) * 32, (su || sl) ? transitionColor2 : transitionColor1, 0);
    }

    if (!u && !r) {
        screen->render(screen, x * 16 + 8, y * 16 + 0, wRandom.next_int(&wRandom, 4), col, wRandom.next_int(&wRandom, 4));
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 0, (r ? 16 : 15) + (u ? 0 : 1) * 32, (su || sr) ? transitionColor2 : transitionColor1, 0);
    }

    if (!d && !l) {
        screen->render(screen, x * 16 + 0, y * 16 + 8, wRandom.next_int(&wRandom, 4), col, wRandom.next_int(&wRandom, 4));
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 8, (l ? 14 : 15) + (d ? 2 : 1) * 32, (sd || sl) ? transitionColor2 : transitionColor1, 0);
    }

    if (!d && !r) {
        screen->render(screen, x * 16 + 8, y * 16 + 8, wRandom.next_int(&wRandom, 4), col, wRandom.next_int(&wRandom, 4));
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 8, (r ? 16 : 15) + (d ? 2 : 1) * 32, (sd || sr) ? transitionColor2 : transitionColor1, 0);
    }
}


/* Little by little the water spreads into a neighbouring hole. */
PUBLIC void watertile_tick(Tile* this, Level* level, int xt, int yt) {
    Random* random = &this->random;

    int xn = xt;
    int yn = yt;

    if (random->next_boolean(random)) {
        xn += random->next_int(random, 2) * 2 - 1;
    } else {
        yn += random->next_int(random, 2) * 2 - 1;
    }

    if (level->get_tile(level, xn, yn) == tiles[HOLE]) {
        level->set_tile(level, xn, yn, this, 0);
    }
}
