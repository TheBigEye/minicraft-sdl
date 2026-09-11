/*
 * inputhandler.h - Keyboard state for the six game actions.
 *
 * Mirrors com.mojang.ld22.InputHandler: every Key counts presses since
 * the last game tick, and game code "absorbs" them one by one, so a
 * single tap is noticed exactly once even if it happens between ticks.
 */
#ifndef INPUTHANDLER_H_
#define INPUTHANDLER_H_

#ifdef USE_SDL1
	#include <SDL/SDL.h>
#else
	#include <SDL2/SDL.h>
#endif

typedef struct{
	int presses, absorbs; /* press count since last tick / presses consumed */
	char down, clicked;   /* held right now? / one unconsumed press pending? */
} Key;

/* The six tracked actions: movement, attacking and opening menus. */
extern Key up, down, left, right, attack, menu;

/* Records a press/release transition on a single Key. */
void key_toggle(Key* key, char pressed);
/* Turns pending presses into `clicked`, one per game tick. */
void key_tick(Key* key);

/* Maps an SDL keysym to an action and forwards the transition. */
#ifdef USE_SDL1
void input_toggle(SDLKey key, char pressed);
#else
void input_toggle(SDL_Keycode key, char pressed);
#endif

/* Ticks all six keys; called once per game tick. */
void input_tick();

#endif /* INPUTHANDLER_H_ */
