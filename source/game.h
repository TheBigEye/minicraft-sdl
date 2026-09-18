/*
 * game.h - The game globals
 *          (Java: com.mojang.ld22.Game): window geometry, the player, run
 *          state and the level-change plumbing shared by every screen.
 */
#ifndef GAME_H
#define GAME_H 1

#include "utils/javalang.h"

#include "entity/player.h"
#include "gfx/spritesheet.h"
#include "screen/menu.h"

#define HEIGHT 192  /* Internal screen height in pixels. */
#define WIDTH 288   /* Internal screen width in pixels. */
#define SCALE 3     /* Window scale factor per internal pixel. */

/* The single player entity. Java: `public Player player` */
extern Player* game_player;
/* Ticks played this run. Java: `public int gameTime` */
extern int game_gameTime;
/* Depth change requested by the stairs. Java: `private int pendingLevelChange` */
extern int game_pendingLevelChange;
/*
 * Set once "Start game" is chosen. C-only: the port keeps an empty world
 * ticking behind the title menu, and this says whether there is one.
 */
extern boolean isingame;

/* Main framebuffer the world and the HUD draw into.
 * Java: `private Screen screen` */
extern Screen game_screen;
/* Light map overlaid on the underground levels.
 * Java: `private Screen lightScreen` */
extern Screen game_lightScreen;

/* Frees and regenerates every level and the player. Java: Game.resetGame() */
PUBLIC void game_reset(void);

/*
 * Switches to a menu, the C counterpart of Java's Game.setMenu(Menu).
 *
 * Two overloads, dispatched on the argument type by _Generic (see the
 * "Overloading" note in javalang.h for why the branches hold the function name):
 *
 *   game_set_menu(mid_TITLE);      by id, the form used all over the port
 *   game_set_menu(&titlemenu);     by object, as Java passes a reference;
 *                                  Menu is the first member of every
 *                                  screen, so any of them works
 *   game_set_menu(null);           closes the menus, back to gameplay
 *
 * Either way the screen's init() hook runs, which is what setMenu() does in
 * Java.
 */
PUBLIC void game_set_menu_id(menu_id menu);
PUBLIC void game_set_menu_obj(void* menu);

/*
 * The selection carries an `int` branch as well as the `menu_id` one, so
 * that a plain integer still lands on game_set_menu_id(), the way Java
 * would accept any expression of the enum's underlying type.
 *
 * Neither GCC nor Clang complains (both are silent with -Wall -Wextra),
 * but the front-end behind Visual Studio Code's C/C++ extension treats
 * the enum and int as the same type and reports a bogus
 *     duplicate association type ("int") in _Generic selection
 * on every translation unit that includes this header. That diagnostic is
 * a false positive, and there is no per-message filter to switch off: the
 * extension only offers the blunt "C_Cpp.errorSquiggles": "Disabled",
 * which throws away every other squiggle too.
 *
 * The way out is to hide the redundant branch from that front-end only.
 * It defines __INTELLISENSE__, so real builds keep the int association
 * exactly as before, and IntelliSense sees a selection with no duplicate
 * types and stays quiet. Call sites are unaffected either way: every one
 * of them passes a menu_id constant, a menu object or null.
 */
#ifdef __INTELLISENSE__
    #define game_set_menu(m)  _Generic((m),         \
            menu_id: game_set_menu_id,              \
            default: game_set_menu_obj)(m)
#else
    #define game_set_menu(m)  _Generic((m),         \
            menu_id: game_set_menu_id,              \
            int:     game_set_menu_id,              \
            default: game_set_menu_obj)(m)
#endif /* __INTELLISENSE__ */

/* Starts the win countdown after the air wizard dies. Java: Game.won() */
PUBLIC void game_won(void);

/* Moves the player between levels in the given direction.
 * Java: Game.changeLevel(int) */
PUBLIC void game_change_level(int dir);


#endif /* GAME_H */
