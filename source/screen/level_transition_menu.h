/*
 * level_transition_menu.h - Level transition (Java:
 * LevelTransitionMenu): the wipe animation played between depths.
 */
#ifndef LEVEL_TRANSITION_MENU_H
#define LEVEL_TRANSITION_MENU_H

extern const menu_vt leveltransitionmenu_vt;

void leveltransitionmenu_init();
void leveltransitionmenu_render(struct _Screen*);
void leveltransitionmenu_tick();

#endif // LEVEL_TRANSITION_MENU_H
