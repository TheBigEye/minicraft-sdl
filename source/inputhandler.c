/*
 * inputhandler.c - Translates SDL key events into the six game actions
 *                  (Java: com.mojang.ld22.InputHandler).
 *
 * The keymap keeps the original's quirks (the "6" key for moving right, for
 * instance) and offers several aliases per action, as the Java original did
 * for arrows and letters.
 */
#include "inputhandler.h"

#include "log.h"


Key up;
Key down;
Key left;
Key right;
Key attack;
Key menu;


/* Java: Key.toggle(boolean pressed). Registers a press or a release, and
 * counts the press for this tick. */
PUBLIC void key_toggle(Key* this, boolean pressed) {
    if (pressed != this->down) {
        this->down = pressed;
    }

    if (pressed) {
        ++this->presses;
    }
}


/*
 * Java: Key.tick().
 *
 * Consumes at most one pending press per tick: `clicked` stays true for
 * exactly one tick per press, which is what menus and item use check.
 */
PUBLIC void key_tick(Key* this) {
    if (this->absorbs < this->presses) {
        ++this->absorbs;
        this->clicked = true;
    } else {
        this->clicked = false;
    }
}


/* Java: InputHandler.tick(). Advances the press and consume state of all
 * six actions. */
PUBLIC void input_tick(void) {
    key_tick(&up);
    key_tick(&down);
    key_tick(&left);
    key_tick(&right);
    key_tick(&attack);
    key_tick(&menu);
}


/*
 * Routes one SDL keysym to its action. Aliases per action:
 *
 *   move up    : W / up arrow
 *   move down  : S / down arrow
 *   move left  : A / left arrow
 *   move right : 6 / right arrow (original keymap quirk)
 *   menu       : Tab / Alt / Enter / X
 *   attack     : Space / Left Ctrl / Insert / C
 */
#ifdef USE_SDL1
PUBLIC void input_toggle(SDLKey key, boolean pressed) {
#else
PUBLIC void input_toggle(SDL_Keycode key, boolean pressed) {
#endif
    /*
     * SDL3 upper-cased the keycodes of the printable keys: SDLK_w became
     * SDLK_W and so on, because they now hold the Unicode code point rather
     * than the Latin-1 byte. The named keys (arrows, Tab, Insert...) kept
     * their spelling, so only the five letters below need the pair.
     */
    switch (key) {
#ifdef USE_SDL3
        case SDLK_W:
#else
        case SDLK_w:
#endif
        case SDLK_UP:
            key_toggle(&up, pressed);
            break;
#ifdef USE_SDL3
        case SDLK_S:
#else
        case SDLK_s:
#endif
        case SDLK_DOWN:
            key_toggle(&down, pressed);
            break;
#ifdef USE_SDL3
        case SDLK_A:
#else
        case SDLK_a:
#endif
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
#ifdef USE_SDL3
        case SDLK_X:
#else
        case SDLK_x:
#endif
            key_toggle(&menu, pressed);
            break;
        case SDLK_SPACE:
        case SDLK_LCTRL:
        case SDLK_INSERT:
#ifdef USE_SDL3
        case SDLK_C:
#else
        case SDLK_c:
#endif
            key_toggle(&attack, pressed);
            break;
        default:
            break;
    }
}
