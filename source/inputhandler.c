#include "inputhandler.h"
#include <SDL2/SDL.h>

Key up;
Key down;
Key left;
Key right;
Key attack;
Key menu;


void key_toggle(Key* key, char pressed){
    if (pressed != key->down) {
        key->down = pressed;
    }

    if (pressed) {
        ++key->presses;
    }
}


void key_tick(Key* key){
    if (key->absorbs < key->presses) {
        ++key->absorbs;
        key->clicked = 1;
    } else {
        key->clicked = 0;
    }
}


void input_tick(){
    key_tick(&up);
    key_tick(&down);
    key_tick(&left);
    key_tick(&right);
    key_tick(&attack);
    key_tick(&menu);
}


void input_toggle(SDL_Keycode key, char pressed) {
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
