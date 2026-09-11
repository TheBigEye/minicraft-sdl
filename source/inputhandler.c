/*
 * inputhandler.c - Translates SDL key events into the six game actions.
 *
 * The keymap keeps the original's quirks (e.g. the "6" key for moving
 * right) and offers several aliases per action, as the Java original
 * did for arrows/letters.
 */
#include "inputhandler.h"

Key up;
Key down;
Key left;
Key right;
Key attack;
Key menu;


/* Registers a press (1) or release (0); counts the press for this tick. */
void key_toggle(Key* key, char pressed){
    if (pressed != key->down) {
        key->down = pressed;
    }

    if (pressed) {
        ++key->presses;
    }
}


/*
 * Consumes at most one pending press per tick: `clicked` stays true for
 * exactly one tick per press, which is what menus and item use check.
 */
void key_tick(Key* key){
    if (key->absorbs < key->presses) {
        ++key->absorbs;
        key->clicked = 1;
    } else {
        key->clicked = 0;
    }
}


/* Advances the press/consume state of all six actions. */
void input_tick(){
    key_tick(&up);
    key_tick(&down);
    key_tick(&left);
    key_tick(&right);
    key_tick(&attack);
    key_tick(&menu);
}


/*
 * Routes one SDL keysym to its action. Aliases per action:
 *   move up    : W / up arrow
 *   move down  : S / down arrow
 *   move left  : A / left arrow
 *   move right : 6 / right arrow (original keymap quirk)
 *   menu       : Tab / Alt / Enter / X
 *   attack     : Space / Left Ctrl / Insert / C
 */
#ifdef USE_SDL1
void input_toggle(SDLKey key, char pressed) {
#else
void input_toggle(SDL_Keycode key, char pressed) {
#endif

    switch (key) {
        case SDLK_w:
        case SDLK_UP:
            key_toggle(&up, pressed);
            break;
        case SDLK_s:
        case SDLK_DOWN:
            key_toggle(&down, pressed);
            break;
        case SDLK_a:
        case SDLK_LEFT:
            key_toggle(&left, pressed);
            break;
        case SDLK_6:
        case SDLK_RIGHT:
            key_toggle(&right, pressed);
            break;
        case SDLK_TAB:
        case SDLK_LALT:
        case SDLK_RALT:
        case SDLK_RETURN:
        case SDLK_x:
            key_toggle(&menu, pressed);
            break;
        case SDLK_SPACE:
        case SDLK_LCTRL:
        case SDLK_INSERT:
        case SDLK_c:
            key_toggle(&attack, pressed);
            break;
        default:
            break;
    }
}
