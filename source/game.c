/*
 * game.c - Game entry point and main loop (Java: Game).
 *
 * Owns the fixed 60Hz tick loop, SDL window/palette setup, the
 * incremental (dirty-rect) screen blitter, HUD rendering and the
 * level stack; main() wires everything together and runs until quit.
 */
#include "crafting/crafting.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gfx/spritesheet.h"
#include "gfx/font.h"

#ifdef USE_SDL1
	#include <SDL/SDL.h>
	/* SDL1 compatibility layer */
	typedef SDL_Surface* SDL_Window;
	#define SDL_WINDOWPOS_UNDEFINED 0
	#define SDL_GetError() SDL_GetError()
#else
	#include <SDL2/SDL.h>
#endif

#include "game.h"
#include "inputhandler.h"
#include "utils/arraylist.h"
#include "utils/utils.h"
#include "screen/menu.h"
#include "level/tile/tile.h"
#include "item/resource/resource.h"
#include "level/level.h"
#include "level/levelgen/levelgen.h"
#include "gfx/color.h"
#include "entity/player.h"
#include "item/item.h"
#include "extern/icons_data.h"
#include "sound/sound.h"

// Helper: print available SDL video drivers (useful for embedded / RISC-V / no-X11 systems)
static void print_sdl_video_drivers(void) {
#ifdef USE_SDL1
    char driver[32];
    if (SDL_VideoDriverName(driver, sizeof(driver))) {
        printf("SDL1 video driver: %s\n", driver);
    } else {
        printf("SDL1 video driver: (unknown)\n");
    }
#else
    int num = SDL_GetNumVideoDrivers();
    printf("Available SDL2 video drivers (%d):\n", num);
    if (num <= 0) {
        printf("  (none)\n");
        return;
    }
    for (int i = 0; i < num; ++i) {
        const char* drv = SDL_GetVideoDriver(i);
        printf("  %d: %s\n", i, drv ? drv : "(null)");
    }
    const char* current = SDL_GetCurrentVideoDriver();
    if (current) {
        printf("Current driver: %s\n", current);
    }
#endif
}

// Set video driver hint (called before SDL_Init)
static void set_video_driver_hint(void) {
    const char* driver = NULL;

#ifdef USE_FB
    #ifdef USE_SDL1
        driver = "fbcon";   // SDL1 framebuffer driver (needs /dev/fb0)
    #else
        driver = "KMSDRM";  // SDL2 modern framebuffer (needs DRM/KMS)
    #endif
#endif

    if (driver) {
#ifdef USE_SDL1
        // SDL1: use environment variable
        char env[128];
        snprintf(env, sizeof(env), "SDL_VIDEODRIVER=%s", driver);
        putenv(env);
#else
        SDL_SetHint(SDL_HINT_VIDEODRIVER, driver);
#endif
        printf("Forcing video driver: %s\n", driver);
    }

    // Allow user override via env var
    const char* env_driver = getenv("SDL_VIDEODRIVER");
    if (env_driver && !driver) {
#ifdef USE_SDL1
        char env[128];
        snprintf(env, sizeof(env), "SDL_VIDEODRIVER=%s", env_driver);
        putenv(env);
#else
        SDL_SetHint(SDL_HINT_VIDEODRIVER, env_driver);
#endif
        printf("Using SDL_VIDEODRIVER from environment: %s\n", env_driver);
    }
}

Screen game_screen;      /* Main framebuffer the HUD/world draw into. */
Screen game_lightScreen; /* Light map overlaid on underground levels. */

int g_ticks = 0; //perf measure
int g_frames = 0; //perf measure

unsigned long tickCount = 0;

SDL_Color sdl_colors[256]; /* 6x6x6 cube palette expanded to RGB. */

enum menu_id current_menu;         /* Active menu, 0 while playing. */
char game_hasfocus = 0;            /* Window has input focus. */
int game_pendingLevelChange = 0;   /* Depth change requested by stairs. */
char updatePerfctr = 0;
char running = 1;                  /* Main loop flag. */
char isingame = 0;                 /* Set once a game has started. */

int game_playerDeadTime = 0; /* Ticks since the player was removed. */
int game_wonTimer = 0;       /* Ticks left before the won menu shows. */
int game_gameTime = 0;       /* Ticks played this run. */
int game_currentLevel;       /* Index into game_levels. */
char game_hasWon = 0;

Level game_levels[5] = {0};  /* Sky, surface and three depths. */
Level* game_level = NULL;    /* Currently active level. */
Player* game_player = NULL;

const int MAX_FPS = -1;

// NOTE: this must be always an array, or C will treat this like read-only
char CLICK_TO_FOCUS[] = "Click to focus!";


/* Switches to a menu and runs its init hook; 0 resumes gameplay. */
void game_set_menu(enum menu_id menu) {
	current_menu = menu;
	init_menu(menu);
}


/* Detaches the player, moves to the level dir steps away and snaps
 * the position to the tile grid before re-adding it. */
void game_changeLevel(int dir) {
	level_removeEntity1(game_level, &game_player->mob.entity);

	game_currentLevel += dir;
	game_level = game_levels + game_currentLevel;
	game_player->mob.entity.x = (game_player->mob.entity.x >> 4) * 16 + 8;
	game_player->mob.entity.y = (game_player->mob.entity.y >> 4) * 16 + 8;

	level_addEntity(game_level, &game_player->mob.entity);
}


/* Starts the three-second win countdown. */
void game_won(){
	game_wonTimer = 60 * 3;
	game_hasWon = 1;
}


/* Frees old levels/player and, when in game, regenerates the whole
 * five-level stack (sky down to depth 3), respawns the player on
 * the surface and populates every level with mobs. */
void game_reset() {
	game_playerDeadTime = 0;
	game_wonTimer = 0;
	game_gameTime = 0;
	game_hasWon = 0;

	for (int i = 0; i < 5; ++i) {
		printf("Freeing level %d\n", i);
		level_free(game_levels + i);
	}

	if (!isingame) {
        return;
    }

	memset(game_levels, 0, sizeof(game_levels));

	game_currentLevel = 3;
	level_init(game_levels + 4, 128, 128, 1, 0);
	level_init(game_levels + 3, 128, 128, 0, game_levels + 4);
	level_init(game_levels + 2, 128, 128, -1, game_levels + 3);
	level_init(game_levels + 1, 128, 128, -2, game_levels + 2);
	level_init(game_levels + 0, 128, 128, -3, game_levels + 1);

	if (game_player) {
		game_player->mob.entity.vt->free(&game_player->mob.entity);
		free(game_player);
	}

	game_level = game_levels + game_currentLevel;
	game_player = (Player*) malloc(sizeof(Player));
	player_create(game_player);
	player_findStartPos(game_player, game_level);

	level_addEntity(game_level, &game_player->mob.entity);

	for (int i = 0; i < 5; ++i) {
        level_trySpawn(game_levels + i, 5000);
    }
}


/* One-time startup: subsystem inits, builds the 6x6x6 color cube
 * into the SDL palette, allocates the screens and opens the title. */
void game_init(){
	levelgen_preinit();
	font_pre_init();
	init_resources();
	init_tiles();
	init_menus();
	crafting_init();

	int pp = 0;
	for (int r = 0; r < 6; ++r) {
		for (int g = 0; g < 6; ++g) {
			for (int b = 0; b < 6; ++b) {
				int rr = r*255 / 5;
				int gg = g*255 / 5;
				int bb = b*255 / 5;
				int mid = (rr * 30 + gg * 59 + bb * 11) / 100;

				int r1 = ((rr + mid * 1) / 2) * 230 / 255 + 10;
				int g1 = ((gg + mid * 1) / 2) * 230 / 255 + 10;
				int b1 = ((bb + mid * 1) / 2) * 230 / 255 + 10;

				sdl_colors[pp].r = r1;
				sdl_colors[pp].g = g1;
				sdl_colors[pp].b = b1;
				++pp;
			}
		}
	}

	create_screen(&game_screen, WIDTH, HEIGHT, &icons_spritesheet);
	create_screen(&game_lightScreen, WIDTH, HEIGHT, &icons_spritesheet);

	game_reset();
	game_set_menu(mid_TITLE);
}


/* Writes one 32-bit pixel into an SDL surface (pitch-aware). */
void set_pixel(SDL_Surface* surface, int x, int y, int color){
	*(int*)(surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel) = color;
}


/* One simulation step: advances play time, polls input and either
 * ticks the active menu or the level itself; also drives the
 * death/won/level-transition state machines. */
void game_tick(){
	++tickCount;

	if (!game_hasfocus) {
		// TODO release all keys
	} else {
		if (isingame) {
            if (!game_player->mob.entity.removed && !game_hasWon) {
                ++game_gameTime;
            }
        }

		input_tick();

		if (current_menu) {
			tick_menu(current_menu);
		} else {
			if (game_player->mob.entity.removed) {
				++game_playerDeadTime;
				if (game_playerDeadTime > 60) {
					game_set_menu(mid_DEAD);
				}
			} else {
				if (game_pendingLevelChange != 0) {
					game_set_menu(mid_LEVEL_TRANSITION);
					game_pendingLevelChange = 0;
				}
			}

			if (game_wonTimer > 0) {
				if (--game_wonTimer == 0) {
					game_set_menu(mid_WON);
				}
			}

			level_tick(game_level);
			++tile_tickCount;
		}
	}
}


/* Draws the bottom HUD bar (health, stamina, active item), any
 * active menu, and the optional debug overlays. */
void game_renderGui() {

    // -DTEST_SHOWPORTALPOS
    #ifdef TEST_SHOWPORTALPOS
        if (isingame) {
            char hax[64];

            int x = game_player->mob.entity.x >> 4;
            int y = game_player->mob.entity.y >> 4;

            sprintf(hax, "P %d %d", x, y);
            font_draw(hax, strlen(hax), &game_screen, 2, 2, getColor4(000, 200, 500, 533));

            int Scnt = 10;

            for (x = 0; x < game_player->mob.entity.level->w; ++x) {
                for (y = 0; y < game_player->mob.entity.level->h; ++y) {
                    if (level_get_tile(game_player->mob.entity.level, x, y) == STAIRS_UP) {
                        sprintf(hax, "U %d %d", x, y);
                        font_draw(hax, strlen(hax), &game_screen, 2, Scnt, getColor4(000, 200, 500, 533));
                        Scnt += 8;
                    }

                    if (level_get_tile(game_player->mob.entity.level, x, y) == STAIRS_DOWN) {
                        sprintf(hax, "D %d %d", x, y);
                        font_draw(hax, strlen(hax), &game_screen, 2, Scnt, getColor4(000, 200, 500, 533));
                        Scnt += 8;
                    }
                }
            }

            if (game_player->mob.entity.level->depth == 1) {
                for (int i = 0; i < game_player->mob.entity.level->entities.size; ++i) {
                    Entity* e = game_player->mob.entity.level->entities.elements[i];
                    if (e->type == AIRWIZARD) {
                        sprintf(hax, "W %d %d", e->x>> 4, e->y>> 4);
                        font_draw(hax, strlen(hax), &game_screen, 2, Scnt, getColor4(000, 200, 500, 533));
                        Scnt += 8;
                        break;
                    }
                }
            }
        }
    #endif


    #ifdef FPS_AND_TICKS
        char fpsticks[64];
        sprintf(fpsticks, "%dfps %dticks", g_frames, g_ticks);
        font_draw(fpsticks, strlen(fpsticks), &game_screen, 2, 2, getColor4(000, 200, 500, 533));
    #endif

	for (int y = 0; y < 2; ++y) {
		for (int x = 0; x < 36; ++x) {
			render_screen(&game_screen, x * 8, game_screen.h - 16 + (y * 8), 0 + 12 * 32, getColor4(0, 0, 0, 0), 0);
		}
	}

	if (isingame){
        /// RENDER THE HUD
		for (int i = 0; i < 10; ++i) {

            // Player's health bar
			if (i < game_player->mob.health) {
				render_screen(&game_screen, i * 8, game_screen.h - 16, 0 + 12 * 32, getColor4(000, 200, 500, 533), 0);
			} else {
				render_screen(&game_screen, i * 8, game_screen.h - 16, 0 + 12 * 32, getColor4(000, 100, 000, 000), 0);
			}

            // Player's stamina bar
			if (game_player->staminaRechargeDelay > 0) {
				if (game_player->staminaRechargeDelay / 4 % 2 == 0) {
					render_screen(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, getColor4(000, 555, 000, 000), 0);
				} else {
					render_screen(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, getColor4(000, 110, 000, 000), 0);
				}
			} else {
				if (i < game_player->stamina) {
					render_screen(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, getColor4(000, 220, 550, 553), 0);
				} else {
					render_screen(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, getColor4(000, 110, 000, 000), 0);
				}
			}
		}

        // Player's current item
		if (game_player->activeItem) {
			item_renderInventory(game_player->activeItem, &game_screen, 10 * 8, game_screen.h - 16);
		}
	}

	if (current_menu) {
		render_menu(current_menu, &game_screen);
	}
}


/* Draws the framed, blinking "Click to focus!" prompt shown while
 * the window has no input focus. */
void game_renderFocusNagger() {
	//click_to_focus
	int c2fLen = strlen(CLICK_TO_FOCUS);
	int xx = (WIDTH - c2fLen * 8) / 2;
	int yy = (HEIGHT - 8) / 2;

	int w = c2fLen;
	int h = 1;

	render_screen(&game_screen, xx - 8, yy - 8, 0 + 13 * 32, getColor4(-1, 1, 5, 445), 0);
	render_screen(&game_screen, xx + w * 8, yy - 8, 0 + 13 * 32, getColor4(-1, 1, 5, 445), 1);
	render_screen(&game_screen, xx - 8, yy + 8, 0 + 13 * 32, getColor4(-1, 1, 5, 445), 2);
	render_screen(&game_screen, xx + w * 8, yy + 8, 0 + 13 * 32, getColor4(-1, 1, 5, 445), 3);

	for (int x = 0; x < w; ++x) {
		render_screen(&game_screen, xx + x * 8, yy - 8, 1 + 13 * 32, getColor4(-1, 1, 5, 445), 0);
		render_screen(&game_screen, xx + x * 8, yy + 8, 1 + 13 * 32, getColor4(-1, 1, 5, 445), 2);
	}

	for (int y = 0; y < h; ++y) {
		render_screen(&game_screen, xx - 8, yy + y * 8, 2 + 13 * 32, getColor4(-1, 1, 5, 445), 0);
		render_screen(&game_screen, xx + w * 8, yy + y * 8, 2 + 13 * 32, getColor4(-1, 1, 5, 445), 1);
	}

	if ((tickCount / 20) % 2 == 0) {
		font_draw(CLICK_TO_FOCUS, c2fLen, &game_screen, xx, yy, getColor4(5, 333, 333, 333));
	} else {
		font_draw(CLICK_TO_FOCUS, c2fLen, &game_screen, xx, yy, getColor4(5, 555, 555, 555));
	}
}


/* Renders one frame: clamps the camera around the player, paints
 * the sky background above ground, then background, sprites and the
 * light overlay, finishing with the GUI. */
void game_render() {
	if (isingame) {
		int xScroll = game_player->mob.entity.x - game_screen.w / 2;
		int yScroll = game_player->mob.entity.y - (game_screen.h - 8) / 2;

		if (xScroll < 16) xScroll = 16;
		if (yScroll < 16) yScroll = 16;
		if (xScroll > game_level->w * 16 - game_screen.w - 16) xScroll = game_level->w * 16 - game_screen.w - 16;
		if (yScroll > game_level->h * 16 - game_screen.h - 16) yScroll = game_level->h * 16 - game_screen.h - 16;

		if (game_currentLevel > 3) {
			int col = getColor4(20, 20, 121, 121);

			for (int y = 0; y < 28; ++y) {
				for (int x = 0; x < 38; ++x) {
					render_screen(&game_screen, x * 8 - ((xScroll / 4) & 7), y * 8 - ((yScroll / 4) & 7), 0, col, 0);
				}
			}
		}

		level_renderBackground(game_level, &game_screen, xScroll, yScroll);
		level_renderSprites(game_level, &game_screen, xScroll, yScroll);

		if(game_currentLevel < 3){
			clear_screen(&game_lightScreen, 0);
			renderLight(game_level, &game_lightScreen, xScroll, yScroll);
			screen_overlay(&game_screen, &game_lightScreen, xScroll, yScroll);
		}
	}

	game_renderGui();

	if (!game_hasfocus){
		game_renderFocusNagger();
	}
}


/* Entry point: SDL window/palette setup, then the fixed-timestep
 * loop (60 ticks per second) with event handling and an incremental
 * blit that only updates pixels changed since the last frame. */
int main(int argc, char** argv) {
	unsigned long long int lastTime = getTimeUS();
	unsigned long long int lastPrinted = lastTime;
	double unprocessed = 0;

	const double usPerTick = 1000000.0 / 60;

	unsigned long long int nextExceptedFrameRenderTime = 0;
	unsigned long long int now  = 0;
	int ticks = 0, frames = 0;

	unsigned char* prevBuf = 0;
	int ret = 0;
	int winHeight = HEIGHT * SCALE;
	int winWidth = WIDTH * SCALE;
	char needsFlip = 0;
	int flipXMin = 0, flipXMax = 0, flipYMin = 0, flipYMax = 0;

	// printf("Starting...\n");

#ifdef USE_SDL1
	SDL_Surface* window = NULL;   /* In SDL1 the "window" is the video surface */
	SDL_Surface* surface = NULL;
#else
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
#endif
	SDL_Event event;
#ifdef USE_SDL1
	SDL_KeyboardEvent* keyEvent = (SDL_KeyboardEvent*) &event;
#else
	SDL_KeyboardEvent* keyEvent = (SDL_KeyboardEvent*) &event;
#endif
	SDL_Rect pixel = {0, 0, SCALE, SCALE};

	game_init();

	// Set video driver hint (FB or SDL_VIDEODRIVER env) BEFORE SDL_Init
	set_video_driver_hint();

	// Initialize SDL and create the window
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		print_sdl_video_drivers();
		printf("\nHint: Try setting SDL_VIDEODRIVER=KMSDRM (SDL2) or fbcon (SDL1)\n");
		printf("      or export SDL_VIDEODRIVER=... before running.\n");
		printf("      On headless / embedded / RISC-V systems you may need DRM/KMS or /dev/fb0 in kernel.\n");
		ret = 1;
		goto QUIT;
	}

	// Initialize audio (embedded WAVs + software mixer).
	// Never fatal: if there's no audio device the game just runs silently.
	sound_init();

#ifdef USE_SDL1
	/* SDL 1.2: use SDL_SetVideoMode */
	window = SDL_SetVideoMode(winWidth, winHeight, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
	if (!window) {
		printf("Failed to set video mode (SDL1): %s\n", SDL_GetError());
		ret = 1;
		goto QUIT;
	}
	surface = window;   /* In SDL1 the returned surface IS the display surface */

	SDL_WM_SetCaption("Minicraft", NULL);
#else
	window = SDL_CreateWindow("Minicraft", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, 0);
	if (!window) {
		printf("Failed to create window: %s\n", SDL_GetError());
		ret = 1;
		goto QUIT;
	}

	SDL_SetWindowTitle(window, "Minicraft");

	// Get the window's surface
	surface = SDL_GetWindowSurface(window);

	if (surface == NULL){
		printf("Failed to get window surface: %s\n", SDL_GetError());
		ret = 1;
		goto QUIT;
	}
#endif

	// Set palette (SDL1 vs SDL2)
	if (surface->format->palette != NULL) {
#ifdef USE_SDL1
		SDL_SetColors(surface, (SDL_Color*)sdl_colors, 0, 256);
#else
		SDL_SetPaletteColors(surface->format->palette, sdl_colors, 0, 256);
#endif
	}

    // -DLEVELGENTEST (only supported on SDL2 for now)
    #if defined(LEVELGENTEST) && !defined(USE_SDL1)
	{
		#define set_px(x, y, color) {             \
			pixel.x = (x)*SCALE;                  \
			pixel.y = (y)*SCALE;                  \
			SDL_FillRect(surface, &pixel, color); \
		}

		int w = 128;
		int h = 128;

		unsigned char* map;
		unsigned char* data;

		pixel.w = SCALE;
		pixel.h = SCALE;

		createAndValidateTopMap(&map, &data, w, h);
		// printf("gen stopped\n");

		// Create a new window for the levelgen test
		SDL_Window* genWindow = SDL_CreateWindow("LevelGen Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w * SCALE, h * SCALE, 0);
		SDL_Surface* genSurface = SDL_GetWindowSurface(genWindow);

		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				int i = x + y * w;

				if (map[i] == WATER) set_px(x, y, 0x000080);
				if (map[i] == GRASS) set_px(x, y, 0x208020);
				if (map[i] == ROCK) set_px(x, y, 0xa0a0a0);
				if (map[i] == DIRT) set_px(x, y, 0x604040);
				if (map[i] == SAND) set_px(x, y, 0xa0a040);
				if (map[i] == TREE) set_px(x, y, 0x003000);
				if (map[i] == LAVA) set_px(x, y, 0xff2020);
				if (map[i] == CLOUD) set_px(x, y, 0xa0a0a0);
				if (map[i] == STAIRS_DOWN) set_px(x, y, 0xffffff);
				if (map[i] == STAIRS_UP) set_px(x, y, 0xffffff);
				if (map[i] == CLOUD_CACTUS) set_px(x, y, 0xff00ff);
			}
		}

		SDL_UpdateWindowSurface(genWindow);

		while (running) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT: running = 0; break;
				}
			}
		}

		free(map);
		free(data);
		goto QUIT;
	}
    #endif


	prevBuf = malloc(sizeof(int) * game_screen.h * game_screen.w);
    if (!prevBuf) {
        printf("Failed to allocate prevBuf memory!\n");
        ret = 1;
        goto QUIT;
    }

	for (int i = 0; i < game_screen.h * game_screen.w; ++i) {
        prevBuf[i] = 0x000000;
    }

	game_hasfocus = 1;

	while (running) {

		now = getTimeUS();
		unprocessed += (now - lastTime) / usPerTick;


		while (unprocessed >= 1) {
			++ticks;
			game_tick();
			--unprocessed;
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYUP:
#ifdef USE_SDL1
					input_toggle(event.key.keysym.sym, 0);
#else
					input_toggle(keyEvent->keysym.sym, 0);
#endif
					break;
				case SDL_KEYDOWN:
#ifdef USE_SDL1
					input_toggle(event.key.keysym.sym, 1);
#else
					input_toggle(keyEvent->keysym.sym, 1);
#endif
					break;
				case SDL_QUIT:
					running = 0;
					break;
#ifdef USE_SDL1
				case SDL_ACTIVEEVENT:
					if (event.active.state & SDL_APPACTIVE) {
						game_hasfocus = event.active.gain;
					}
					break;
#else
				case SDL_WINDOWEVENT:
					// To manage focus, the WINDOWEVENT_FOCUS_GAINED/LOST event is used
					if(event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
						game_hasfocus = 0;
					else if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
						game_hasfocus = 1;
					break;
#endif
			}
		}

		needsFlip = 0;
		flipXMin = winWidth;
		flipXMax = 0;
		flipYMin = winHeight;
		flipYMax = 0;

        if (MAX_FPS >= 30) {
            if (now < nextExceptedFrameRenderTime){
                goto SKIP_RENDER;
            } else{
                nextExceptedFrameRenderTime = now + 1000000 / MAX_FPS;
            }
        }

		++frames;
		game_render();

		for (int y = 0; y < game_screen.h; ++y) {
			pixel.y = y * SCALE;

			for (int x = 0; x < game_screen.w; ++x) {
				pixel.x = x * SCALE;

				int index = y * game_screen.w + x;
				int screen_px = game_screen.pixels[index];

				if(screen_px != prevBuf[index]){
					prevBuf[index] = screen_px;
					needsFlip = 1;

					int xmin = pixel.x;
					int xmax = xmin + SCALE;
					int ymin = pixel.y;
					int ymax = ymin + SCALE;

					if (xmin < flipXMin) flipXMin = xmin;
					if (xmax > flipXMax) flipXMax = xmax;
					if (ymin < flipYMin) flipYMin = ymin;
					if (ymax > flipYMax) flipYMax = ymax;

					// Convert index pallete to 32-bit color pallete.
					Uint32 mapped_color = SDL_MapRGB(surface->format, sdl_colors[screen_px].r, sdl_colors[screen_px].g, sdl_colors[screen_px].b);

                    #if SCALE == 1
                        ((Uint32*)surface->pixels)[ y * (surface->pitch / 4) + x] = mapped_color;

                    #elif SCALE == 2
                        for (int sub_y = 0; sub_y < SCALE; sub_y++){
                            for (int sub_x = 0; sub_x < SCALE; sub_x++){
                                int dest_x = x * SCALE + sub_x;
                                int dest_y = y * SCALE + sub_y;
                                ((Uint32*)surface->pixels)[ dest_y * (surface->pitch / 4) + dest_x] = mapped_color;
                            }
                        }

                    #else
                        SDL_FillRect(surface, &pixel, mapped_color);
                    #endif
				}
			}
		}

	SKIP_RENDER:
		// Update the window surface partially ...
		if (needsFlip) {
			// printf("RENDERING %d %d %d %d\n", flipXMin, flipXMax, flipYMin, flipYMax);
            SDL_Rect updateRect = { flipXMin, flipYMin, flipXMax - flipXMin, flipYMax - flipYMin };

#ifdef USE_SDL1
			/* SDL1: update whole surface or use dirty rects */
			SDL_Flip(surface);                 /* simple double buffer flip */
			/* Alternative for performance: SDL_UpdateRect(surface, flipXMin, flipYMin, ...); */
#else
			SDL_UpdateWindowSurfaceRects(window, &updateRect, 1);
#endif
		}

		if (now - lastPrinted > 1000000) {
			printf("%d ticks, %d fps\n", ticks, frames);
			g_ticks = ticks;
			g_frames = frames;
			ticks = 0;
			frames = 0;
			updatePerfctr = 1;
			lastPrinted = now;
		}

		lastTime = now;
	}

    QUIT:

	if (prevBuf) free(prevBuf);

	// Close SDL and free EVERYTHING
	sound_quit();
	SDL_Quit();
	crafting_free();
	delete_screen(&game_screen);
	delete_screen(&game_lightScreen);

	for (int i = 0; i < 5; ++i) {
		// printf("Freeing level %d\n", i);
		level_free(game_levels + i);
	}

	if (game_player) {
		game_player->mob.entity.vt->free(&game_player->mob.entity);
		free(game_player);
	}

	return ret;
}
