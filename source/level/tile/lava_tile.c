/*
 * lava_tile.c - Behaviour of lava (Java: tile.LavaTile).
 *
 * It glows (radius 6), is crossable only by swimming, and is drawn animated
 * with a Random seeded from the position and the tick, just like water.
 */
#include "tile.h"
#include "lava_tile.h"

#include "../../gfx/color.h"
#include "../../utils/javarandom.h"

/* The animation generator (Java: the local Random of render()). */
STATIC Random wRandom;


/* Constructor: a liquid that connects to sand and to itself. */
PUBLIC void lavatile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render           = lavatile_render;
    this->tick             = lavatile_tick;
    this->may_pass         = lavatile_may_pass;
    this->get_light_radius = lavatile_get_light_radius;

    this->connects_to_sand = true;
    this->connects_to_lava = true;

    random_create(&wRandom);
}


/* Crossable only by swimming. Java: LavaTile.mayPass() { return e.canSwim(); } */
PUBLIC boolean lavatile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;

    return e->can_swim(e);
}


/* Lava glows: it is the only light source in the game. */
PUBLIC int lavatile_get_light_radius(Tile* this, Level* level, int x, int y) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;

    return 6;
}


/* Animated surface, with edges against sand or ground. */
PUBLIC void lavatile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    wRandom.set_seed(&wRandom, (tile_tick_count + (x / 2 - y) * 4311) / 10 * 54687121 + x * 3271612 + y * 3412987161);

    int col = get_color4(500, 500, 520, 550);
    int transitionColor1 = get_color4(3, 500, level->dirtColor - 111, level->dirtColor);
    int transitionColor2 = get_color4(3, 500, level->sandColor - 110, level->sandColor);

    Tile* up    = level->get_tile(level, x, y - 1);
    Tile* down  = level->get_tile(level, x, y + 1);
    Tile* left  = level->get_tile(level, x - 1, y);
    Tile* right = level->get_tile(level, x + 1, y);

    boolean u = !up->connects_to_lava;
    boolean d = !down->connects_to_lava;
    boolean l = !left->connects_to_lava;
    boolean r = !right->connects_to_lava;

    boolean su = u && up->connects_to_sand;
    boolean sd = d && down->connects_to_sand;
    boolean sl = l && left->connects_to_sand;
    boolean sr = r && right->connects_to_sand;

    if (!u && !l) screen->render(screen, x * 16 + 0, y * 16 + 0, wRandom.next_int(&wRandom, 4), col, wRandom.next_int(&wRandom, 4));
    else screen->render(screen, x * 16 + 0, y * 16 + 0, (l ? 14 : 15) + (u ? 0 : 1) * 32, (su || sl) ? transitionColor2 : transitionColor1, 0);

    if (!u && !r) screen->render(screen, x * 16 + 8, y * 16 + 0, wRandom.next_int(&wRandom, 4), col, wRandom.next_int(&wRandom, 4));
    else screen->render(screen, x * 16 + 8, y * 16 + 0, (r ? 16 : 15) + (u ? 0 : 1) * 32, (su || sr) ? transitionColor2 : transitionColor1, 0);

    if (!d && !l) screen->render(screen, x * 16 + 0, y * 16 + 8, wRandom.next_int(&wRandom, 4), col, wRandom.next_int(&wRandom, 4));
    else screen->render(screen, x * 16 + 0, y * 16 + 8, (l ? 14 : 15) + (d ? 2 : 1) * 32, (sd || sl) ? transitionColor2 : transitionColor1, 0);

    if (!d && !r) screen->render(screen, x * 16 + 8, y * 16 + 8, wRandom.next_int(&wRandom, 4), col, wRandom.next_int(&wRandom, 4));
    else screen->render(screen, x * 16 + 8, y * 16 + 8, (r ? 16 : 15) + (d ? 2 : 1) * 32, (sd || sr) ? transitionColor2 : transitionColor1, 0);
}


/* Little by little the lava spreads into a neighbouring hole. */
PUBLIC void lavatile_tick(Tile* this, Level* level, int xt, int yt) {
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
