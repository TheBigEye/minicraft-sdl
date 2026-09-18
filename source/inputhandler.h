/*
 * inputhandler.h - Keyboard state for the six game actions
 *                  (Java: com.mojang.ld22.InputHandler).
 *
 * Every Key counts the presses since the last game tick, and the game code
 * "absorbs" them one by one, so that a single tap is noticed exactly once
 * even if it happens between ticks.
 */
#ifndef INPUTHANDLER_H_
#define INPUTHANDLER_H_ 1

#include "utils/javalang.h"

#include "sdlcompat.h"

typedef struct Key Key;

struct Key {
    /* Press count since the last tick, and how many of them were consumed.
     * Java: `private int presses, absorbs` */
    int presses, absorbs;
    /* Held right now. Java: `private boolean down` */
    boolean down;
    /* One unconsumed press pending. Java: `private boolean clicked` */
    boolean clicked;
};

/* The six tracked actions: movement, attacking and opening menus.
 * Java: `public Key up, down, left, right, attack, menu` */
extern Key up, down, left, right, attack, menu;

/* Registers a press (true) or a release (false); counts the press for this
 * tick. Java: Key.toggle(boolean pressed) */
PUBLIC void key_toggle(Key* this, boolean pressed);

/* Turns the pending presses into `clicked`, at most one per game tick.
 * Java: Key.tick() */
PUBLIC void key_tick(Key* this);

/* Maps an SDL keysym to an action and forwards the transition. */
#ifdef USE_SDL1
PUBLIC void input_toggle(SDLKey key, boolean pressed);
#else
PUBLIC void input_toggle(SDL_Keycode key, boolean pressed);
#endif /* USE_SDL1 */

/* Ticks all six keys; called once per game tick. Java: InputHandler.tick() */
PUBLIC void input_tick(void);

#endif /* INPUTHANDLER_H_ */
