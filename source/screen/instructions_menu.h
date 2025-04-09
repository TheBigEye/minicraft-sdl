#ifndef SCREEN_INSTRUCTIONS_MENU_H_
#define SCREEN_INSTRUCTIONS_MENU_H_
#include "menu.h"

extern const menu_vt instructionsmenu_vt;

void instructionsmenu_tick();
void instructionsmenu_init();
void instructionsmenu_render(Screen*);

#endif /* SCREEN_INSTRUCTIONS_MENU_H_ */
