/*
 * game.h - Game globals (Java: Game): window geometry, the player,
 *          run state and level-change plumbing shared by all screens.
 */
#include "gfx/spritesheet.h"
#include "entity/player.h"
#include "screen/menu.h"

#define HEIGHT 192  /* Internal screen height in pixels. */
#define WIDTH 288   /* Internal screen width in pixels. */
#define SCALE 3     /* Window scale factor per internal pixel. */

extern Player* game_player;            /* The single player entity. */
extern int game_gameTime;              /* Ticks played this run. */
extern int game_pendingLevelChange;    /* Nonzero requests a depth change. */
extern char isingame;                  /* Set once "Start game" is chosen. */

/* Frees and regenerates all levels and the player. */
void game_reset();
/* Switches to a menu (0 closes menus and returns to gameplay). */
void game_set_menu(enum menu_id menu);
/* Starts the win countdown after the air wizard dies. */
void game_won();

/* Moves the player between levels in the given direction. */
void game_changeLevel(int dir);
