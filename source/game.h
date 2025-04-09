#include "gfx/spritesheet.h"
#include "entity/player.h"
#include "screen/menu.h"

#define HEIGHT 192
#define WIDTH 288
#define SCALE 3

extern Player* game_player;
extern int game_gameTime;
extern int game_pendingLevelChange;
extern char isingame;

void game_reset();
void game_set_menu(enum menu_id menu);
void game_won();

void game_changeLevel(int dir);
