/*
 * game.c - Entry point and main loop
 *          (Java: com.mojang.ld22.Game).
 *
 * Owns the fixed 60 Hz tick loop, the SDL window and palette setup, the
 * incremental (dirty-rect) screen blitter, the HUD rendering and the level
 * stack; main() wires everything together and runs until quit.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crafting/crafting.h"
#include "entity/player.h"
#include "extern/icons_data.h"
#include "game.h"
#include "gfx/color.h"
#include "gfx/font.h"
#include "gfx/spritesheet.h"
#include "inputhandler.h"
#include "item/item.h"
#include "item/resource/resource.h"
#include "utils/javalang.h"
#include "level/level.h"
#include "level/levelgen/levelgen.h"
#include "level/tile/tile.h"
#include "log.h"
#include "screen/menu.h"
#include "sound/sound.h"
#include "utils/arraylist.h"
#include "utils/utils.h"
#include "version.h"

/* The SDL 1.2 / SDL2 / SDL3 differences are absorbed in one place. */
#include "sdlcompat.h"


/* Prints the available SDL video drivers, which is what you want to see on
 * embedded, RISC-V or otherwise headless systems. */
PRIVATE void print_sdl_video_drivers(void) {
#ifdef USE_SDL1
    char driver[32];

    if (SDL_VideoDriverName(driver, sizeof(driver))) {
        LOG_INFO("SDL1 video driver: %s", driver);
    } else {
        LOG_INFO("SDL1 video driver: (unknown)");
    }
#else
    int num = SDL_GetNumVideoDrivers();

    LOG_INFO("available SDL video drivers (%d):", num);

    if (num <= 0) {
        LOG_INFO("  (none)");
        return;
    }

    for (int i = 0; i < num; ++i) {
        const char* drv = SDL_GetVideoDriver(i);

        LOG_INFO("  %d: %s", i, drv ? drv : "(null)");
    }

    const char* current = SDL_GetCurrentVideoDriver();

    if (current) {
        LOG_INFO("current driver: %s", current);
    }
#endif
}


/* Sets the video driver hint; it has to run before SDL_Init. */
PRIVATE void set_video_driver_hint(void) {
    const char* driver = null;

#ifdef USE_FB
    #ifdef USE_SDL1
        driver = "fbcon";   /* SDL1 framebuffer driver, needs /dev/fb0. */
    #else
        driver = "KMSDRM";  /* SDL2 modern framebuffer, needs DRM/KMS. */
    #endif
#endif

    if (driver) {
#ifdef USE_SDL1
        /* SDL1: set it through the environment. */
        char env[128];

        snprintf(env, sizeof(env), "SDL_VIDEODRIVER=%s", driver);
        putenv(env);
#else
        SDL_SET_HINT(SDL_HINT_VIDEODRIVER_NAME, driver);
#endif
        LOG_INFO("forcing video driver: %s", driver);
    }

    /* The environment overrides the compiled-in choice. */
    const char* env_driver = getenv("SDL_VIDEODRIVER");

    if (env_driver && !driver) {
#ifdef USE_SDL1
        char env[128];

        snprintf(env, sizeof(env), "SDL_VIDEODRIVER=%s", env_driver);
        putenv(env);
#else
        SDL_SET_HINT(SDL_HINT_VIDEODRIVER_NAME, env_driver);
#endif
        LOG_INFO("using SDL_VIDEODRIVER from environment: %s", env_driver);
    }
}


/* --- state --- */

/* Main framebuffer the world and the HUD draw into. */
Screen game_screen;
/* Light map overlaid on the underground levels. */
Screen game_lightScreen;

/* Frames and ticks of the last second, for the FPS counter. */
PRIVATE int g_ticks = 0;
PRIVATE int g_frames = 0;

PRIVATE unsigned long tickCount = 0;

/* The 6x6x6 colour cube expanded to RGB. */
PRIVATE SDL_Color sdl_colors[256];

/* Active menu, null while playing. Java: `public Menu menu` */
PRIVATE Menu* current_menu = null;
/* Window has input focus. */
PRIVATE boolean game_hasfocus = false;
/* Depth change requested by the stairs. */
int game_pendingLevelChange = 0;
/* Main loop flag. */
PRIVATE boolean running = true;
/* Set once a game has started. */
boolean isingame = false;

/* Ticks since the player was removed. Java: `private int playerDeadTime` */
PRIVATE int game_playerDeadTime = 0;
/* Ticks left before the won menu shows. Java: `private int wonTimer` */
PRIVATE int game_wonTimer = 0;
/* Ticks played this run. */
int game_gameTime = 0;
/* Index into game_levels. */
PRIVATE int game_currentLevel;
/* Set once the air wizard is down. Java: `private boolean hasWon` */
PRIVATE boolean game_hasWon = false;

/* Sky, surface and three depths. Java: `private Level[] levels` */
PRIVATE Level game_levels[5] = {0};
/* Currently active level. Java: `private Level level` */
PRIVATE Level* game_level = null;
/* The single player. */
Player* game_player = null;

/* -1 means uncapped; anything at or above 30 caps the frame rate. */
PRIVATE const int MAX_FPS = -1;

/* NOTE: this has to stay an array, or C will place it in read-only memory. */
PRIVATE char CLICK_TO_FOCUS[] = "Click to focus!";


/*
 * Java: Game.setMenu(Menu). Runs the incoming screen's init hook.
 *
 * The two overloads differ only in how they name the screen: by id, or by
 * the object itself. `game_set_menu()` in game.h picks one from the
 * argument's type.
 */
PUBLIC void game_set_menu_id(menu_id menu) {
    current_menu = get_menu(menu);

    if (current_menu) {
        current_menu->init(current_menu);
    }
}


/*
 * `menu` is a Menu*, or a pointer to any screen, since Menu is the first
 * member of all of them. null closes the menus and resumes gameplay.
 */
PUBLIC void game_set_menu_obj(void* menu) {
    current_menu = (Menu*) menu;

    if (current_menu) {
        current_menu->init(current_menu);
    }
}


/*
 * Java: Game.changeLevel(int).
 *
 * Detaches the player, moves to the level `dir` steps away and snaps the
 * position to the tile grid before adding it back.
 */
PUBLIC void game_change_level(int dir) {
    game_level->remove(game_level, &game_player->mob.entity);

    game_currentLevel += dir;
    game_level = game_levels + game_currentLevel;

    game_player->mob.entity.x = (game_player->mob.entity.x >> 4) * 16 + 8;
    game_player->mob.entity.y = (game_player->mob.entity.y >> 4) * 16 + 8;

    game_level->add(game_level, &game_player->mob.entity);
}


/* Java: Game.won(). Starts the three-second win countdown. */
PUBLIC void game_won(void) {
    game_wonTimer = 60 * 3;
    game_hasWon = true;
}


/*
 * Java: Game.resetGame().
 *
 * Frees the old levels and player and, when in game, regenerates the whole
 * five-level stack (sky down to depth 3), respawns the player on the surface
 * and populates every level with mobs.
 */
PUBLIC void game_reset(void) {
    game_playerDeadTime = 0;
    game_wonTimer = 0;
    game_gameTime = 0;
    game_hasWon = false;

    /*
     * The first time around the levels have never been built (the array is
     * all zeroes), so each one is freed only if it had been created before.
     */
    for (int i = 0; i < 5; ++i) {
        if (game_levels[i].free) {
            LOG_TRACE("freeing level %d", i);
            game_levels[i].free(game_levels + i);
        }
    }

    if (!isingame) {
        return;
    }

    memset(game_levels, 0, sizeof(game_levels));

    game_currentLevel = 3;
    level_create(game_levels + 4, 128, 128, 1, 0);
    level_create(game_levels + 3, 128, 128, 0, game_levels + 4);
    level_create(game_levels + 2, 128, 128, -1, game_levels + 3);
    level_create(game_levels + 1, 128, 128, -2, game_levels + 2);
    level_create(game_levels + 0, 128, 128, -3, game_levels + 1);

    if (game_player) {
        game_player->mob.entity.free(&game_player->mob.entity);
        delete(game_player);
    }

    game_level = game_levels + game_currentLevel;
    game_player = new(Player);
    player_create(game_player);
    player_find_start_pos(game_player, game_level);

    game_level->add(game_level, &game_player->mob.entity);

    for (int i = 0; i < 5; ++i) {
        game_levels[i].try_spawn(game_levels + i, 5000);
    }
}


/*
 * One-time startup: the subsystem inits, the 6x6x6 colour cube built into
 * the SDL palette, the screens allocated and the title menu opened.
 */
PRIVATE void game_init(void) {
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
                int rr = r * 255 / 5;
                int gg = g * 255 / 5;
                int bb = b * 255 / 5;
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

    screen_create(&game_screen, WIDTH, HEIGHT, &icons_spritesheet);
    screen_create(&game_lightScreen, WIDTH, HEIGHT, &icons_spritesheet);

    game_reset();
    game_set_menu(mid_TITLE);
}


/*
 * Java: Game.tick().
 *
 * One simulation step: advances the play time, polls input and either ticks
 * the active menu or the level itself; it also drives the death, win and
 * level-transition state machines.
 */
PRIVATE void game_tick(void) {
    ++tickCount;

    if (!game_hasfocus) {
        /* TODO release all keys */
    } else {
        if (isingame) {
            if (!game_player->mob.entity.removed && !game_hasWon) {
                ++game_gameTime;
            }
        }

        input_tick();

        if (current_menu) {
            current_menu->tick(current_menu);
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

            game_level->tick(game_level);
            ++tile_tick_count;
        }
    }
}


/*
 * Java: Game.renderGui().
 *
 * Draws the bottom HUD bar (health, stamina, active item), any active menu
 * and the optional debug overlays.
 */
PRIVATE void game_render_gui(void) {
    /* -DTEST_SHOWPORTALPOS */
    #ifdef TEST_SHOWPORTALPOS
        if (isingame) {
            char hax[64];

            int x = game_player->mob.entity.x >> 4;
            int y = game_player->mob.entity.y >> 4;

            sprintf(hax, "P %d %d", x, y);
            font_draw(hax, strlen(hax), &game_screen, 2, 2, get_color4(000, 200, 500, 533));

            int Scnt = 10;

            for (x = 0; x < game_player->mob.entity.level->w; ++x) {
                for (y = 0; y < game_player->mob.entity.level->h; ++y) {
                    if (game_player->mob.entity.level->get_tile(game_player->mob.entity.level, x, y) == tiles[STAIRS_UP]) {
                        sprintf(hax, "U %d %d", x, y);
                        font_draw(hax, strlen(hax), &game_screen, 2, Scnt, get_color4(000, 200, 500, 533));
                        Scnt += 8;
                    }

                    if (game_player->mob.entity.level->get_tile(game_player->mob.entity.level, x, y) == tiles[STAIRS_DOWN]) {
                        sprintf(hax, "D %d %d", x, y);
                        font_draw(hax, strlen(hax), &game_screen, 2, Scnt, get_color4(000, 200, 500, 533));
                        Scnt += 8;
                    }
                }
            }

            if (game_player->mob.entity.level->depth == 1) {
                for (int i = 0; i < game_player->mob.entity.level->entities.size; ++i) {
                    Entity* e = game_player->mob.entity.level->entities.elements[i];

                    if (e->type == AIRWIZARD) {
                        sprintf(hax, "W %d %d", e->x >> 4, e->y >> 4);
                        font_draw(hax, strlen(hax), &game_screen, 2, Scnt, get_color4(000, 200, 500, 533));
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
        font_draw(fpsticks, strlen(fpsticks), &game_screen, 2, 2, get_color4(000, 200, 500, 533));
    #endif

    for (int y = 0; y < 2; ++y) {
        for (int x = 0; x < 36; ++x) {
            game_screen.render(&game_screen, x * 8, game_screen.h - 16 + (y * 8), 0 + 12 * 32, get_color4(0, 0, 0, 0), 0);
        }
    }

    if (isingame) {
        /* The HUD: health on top, stamina below it. */
        for (int i = 0; i < 10; ++i) {
            if (i < game_player->mob.health) {
                game_screen.render(&game_screen, i * 8, game_screen.h - 16, 0 + 12 * 32, get_color4(000, 200, 500, 533), 0);
            } else {
                game_screen.render(&game_screen, i * 8, game_screen.h - 16, 0 + 12 * 32, get_color4(000, 100, 000, 000), 0);
            }

            if (game_player->staminaRechargeDelay > 0) {
                if (game_player->staminaRechargeDelay / 4 % 2 == 0) {
                    game_screen.render(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, get_color4(000, 555, 000, 000), 0);
                } else {
                    game_screen.render(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, get_color4(000, 110, 000, 000), 0);
                }
            } else {
                if (i < game_player->stamina) {
                    game_screen.render(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, get_color4(000, 220, 550, 553), 0);
                } else {
                    game_screen.render(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, get_color4(000, 110, 000, 000), 0);
                }
            }
        }

        /* The item the player is holding. */
        if (game_player->activeItem) {
            item_render_inventory(game_player->activeItem, &game_screen, 10 * 8, game_screen.h - 16);
        }
    }

    if (current_menu) {
        current_menu->render(current_menu, &game_screen);
    }
}


/*
 * Java: Game.renderFocusNagger().
 *
 * Draws the framed, blinking "Click to focus!" prompt shown while the window
 * has no input focus.
 */
PRIVATE void game_render_focus_nagger(void) {
    int c2fLen = strlen(CLICK_TO_FOCUS);
    int xx = (WIDTH - c2fLen * 8) / 2;
    int yy = (HEIGHT - 8) / 2;

    int w = c2fLen;
    int h = 1;

    game_screen.render(&game_screen, xx - 8, yy - 8, 0 + 13 * 32, get_color4(-1, 1, 5, 445), 0);
    game_screen.render(&game_screen, xx + w * 8, yy - 8, 0 + 13 * 32, get_color4(-1, 1, 5, 445), 1);
    game_screen.render(&game_screen, xx - 8, yy + 8, 0 + 13 * 32, get_color4(-1, 1, 5, 445), 2);
    game_screen.render(&game_screen, xx + w * 8, yy + 8, 0 + 13 * 32, get_color4(-1, 1, 5, 445), 3);

    for (int x = 0; x < w; ++x) {
        game_screen.render(&game_screen, xx + x * 8, yy - 8, 1 + 13 * 32, get_color4(-1, 1, 5, 445), 0);
        game_screen.render(&game_screen, xx + x * 8, yy + 8, 1 + 13 * 32, get_color4(-1, 1, 5, 445), 2);
    }

    for (int y = 0; y < h; ++y) {
        game_screen.render(&game_screen, xx - 8, yy + y * 8, 2 + 13 * 32, get_color4(-1, 1, 5, 445), 0);
        game_screen.render(&game_screen, xx + w * 8, yy + y * 8, 2 + 13 * 32, get_color4(-1, 1, 5, 445), 1);
    }

    if ((tickCount / 20) % 2 == 0) {
        font_draw(CLICK_TO_FOCUS, c2fLen, &game_screen, xx, yy, get_color4(5, 333, 333, 333));
    } else {
        font_draw(CLICK_TO_FOCUS, c2fLen, &game_screen, xx, yy, get_color4(5, 555, 555, 555));
    }
}


/*
 * Java: Game.render().
 *
 * Renders one frame: clamps the camera around the player, paints the sky
 * background above ground, then the background, the sprites and the light
 * overlay, and finishes with the GUI.
 */
PRIVATE void game_render(void) {
    if (isingame) {
        int xScroll = game_player->mob.entity.x - game_screen.w / 2;
        int yScroll = game_player->mob.entity.y - (game_screen.h - 8) / 2;

        if (xScroll < 16) xScroll = 16;
        if (yScroll < 16) yScroll = 16;
        if (xScroll > game_level->w * 16 - game_screen.w - 16) xScroll = game_level->w * 16 - game_screen.w - 16;
        if (yScroll > game_level->h * 16 - game_screen.h - 16) yScroll = game_level->h * 16 - game_screen.h - 16;

        if (game_currentLevel > 3) {
            int col = get_color4(20, 20, 121, 121);

            for (int y = 0; y < 28; ++y) {
                for (int x = 0; x < 38; ++x) {
                    game_screen.render(&game_screen, x * 8 - ((xScroll / 4) & 7), y * 8 - ((yScroll / 4) & 7), 0, col, 0);
                }
            }
        }

        game_level->render_background(game_level, &game_screen, xScroll, yScroll);
        game_level->render_sprites(game_level, &game_screen, xScroll, yScroll);

        if (game_currentLevel < 3) {
            game_lightScreen.clear(&game_lightScreen, 0);
            game_level->render_light(game_level, &game_lightScreen, xScroll, yScroll);
            screen_overlay(&game_screen, &game_lightScreen, xScroll, yScroll);
        }
    }

    game_render_gui();

    if (!game_hasfocus) {
        game_render_focus_nagger();
    }
}


/*
 * Entry point: SDL window and palette setup, then the fixed-timestep loop
 * (60 ticks per second) with event handling and an incremental blit that
 * only updates the pixels changed since the last frame.
 */
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    unsigned long long int lastTime = get_time_us();
    unsigned long long int lastPrinted = lastTime;
    double unprocessed = 0;

    const double usPerTick = 1000000.0 / 60;

    unsigned long long int nextExceptedFrameRenderTime = 0;
    unsigned long long int now = 0;
    int ticks = 0, frames = 0;

    int* prevBuf = null;
    int ret = 0;
    int winHeight = HEIGHT * SCALE;
    int winWidth = WIDTH * SCALE;
    boolean needsFlip = false;
    int flipXMin = 0, flipXMax = 0, flipYMin = 0, flipYMax = 0;

#ifdef USE_SDL1
    /* In SDL1 the "window" is the video surface itself. */
    SDL_Surface* window = null;
    SDL_Surface* surface = null;
#else
    SDL_Window* window = null;
    SDL_Surface* surface = null;
#endif
    SDL_Event event;
    SDL_KeyboardEvent* keyEvent = (SDL_KeyboardEvent*) &event;
    SDL_Rect pixel = {0, 0, SCALE, SCALE};

    game_init();

    /* Set the video driver hint (compiled in, or SDL_VIDEODRIVER) before
     * SDL_Init. */
    set_video_driver_hint();

    /* Initialize SDL and create the window. */
    if (!SDL_INIT_SUCCEEDED(SDL_Init(SDL_INIT_VIDEO))) {
        LOG_ERROR("SDL_Init failed: %s", SDL_GetError());
        print_sdl_video_drivers();
        LOG_ERROR("hint: try SDL_VIDEODRIVER=KMSDRM (SDL2) or fbcon (SDL1)");
        LOG_ERROR("      or export SDL_VIDEODRIVER=... before running.");
        LOG_ERROR("      on headless, embedded or RISC-V systems you may need DRM/KMS or /dev/fb0 in the kernel.");
        ret = 1;
        goto QUIT;
    }

    /* Initialize audio (embedded WAVs plus a software mixer). Never fatal:
     * with no audio device the game simply runs silently. */
    sound_init();

#ifdef USE_SDL1
    /* SDL 1.2: SDL_SetVideoMode. */
    window = SDL_SetVideoMode(winWidth, winHeight, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
    if (!window) {
        LOG_ERROR("failed to set video mode (SDL1): %s", SDL_GetError());
        ret = 1;
        goto QUIT;
    }
    surface = window;   /* In SDL1 the returned surface IS the display surface. */

    SDL_WM_SetCaption("Minicraft " VERSION, null);
#else
    window = SDL_CREATE_WINDOW("Minicraft " VERSION, winWidth, winHeight, 0);
    if (!window) {
        LOG_ERROR("failed to create window: %s", SDL_GetError());
        ret = 1;
        goto QUIT;
    }

    SDL_SetWindowTitle(window, "Minicraft " VERSION);

    /* The window's surface. */
    surface = SDL_GetWindowSurface(window);
    if (surface == null) {
        LOG_ERROR("failed to get window surface: %s", SDL_GetError());
        ret = 1;
        goto QUIT;
    }
#endif

    /*
     * Set the palette, if the window surface has one. The game itself
     * indexes into sdl_colors and maps to RGB per pixel (see the blit
     * below), so this only matters for the SDL1 code path that can ask the
     * surface to do the translation.
     */
    if (SDL_SURFACE_PALETTE(surface) != null) {
        SDL_SET_PALETTE_COLORS(surface, sdl_colors, 0, 256);
    }

    /* -DLEVELGENTEST, only supported on SDL2 for now. */
    #if defined(LEVELGENTEST) && !defined(USE_SDL1)
    {
        #define set_px(x, y, color) {             \
            pixel.x = (x)*SCALE;                  \
            pixel.y = (y)*SCALE;                  \
            SDL_FILL_RECT(surface, &pixel, color); \
        }

        int w = 128;
        int h = 128;

        unsigned char* map;
        unsigned char* data;

        pixel.w = SCALE;
        pixel.h = SCALE;

        create_and_validate_top_map(&map, &data, w, h);

        /* A second window shows the generated map. */
        SDL_Window* genWindow = SDL_CREATE_WINDOW("LevelGen Test", w * SCALE, h * SCALE, 0);
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
                    case GAME_EV_QUIT: running = false; break;
                    default: break;
                }
            }
        }

        free(map);
        free(data);
        goto QUIT;
    }
    #endif

    prevBuf = new_array(int, game_screen.h * game_screen.w);
    if (!prevBuf) {
        LOG_ERROR("failed to allocate prevBuf memory");
        ret = 1;
        goto QUIT;
    }

    for (int i = 0; i < game_screen.h * game_screen.w; ++i) {
        prevBuf[i] = 0x000000;
    }

    game_hasfocus = true;

    while (running) {
        now = get_time_us();
        unprocessed += (now - lastTime) / usPerTick;

        while (unprocessed >= 1) {
            ++ticks;
            game_tick();
            --unprocessed;
        }

        while (SDL_PollEvent(&event)) {
            /*
             * Focus first, because the three libraries report it in three
             * unrelated ways - SDL1 with an SDL_ACTIVEEVENT and a state
             * mask, SDL2 with a window event plus a sub-type, SDL3 with two
             * events of its own - and only the last of those fits a switch
             * on event.type. sdlcompat.h folds them into these two tests.
             */
            if (GAME_IS_FOCUS_LOST(&event)) {
                game_hasfocus = false;
                continue;
            }

            if (GAME_IS_FOCUS_GAINED(&event)) {
                game_hasfocus = true;
                continue;
            }

            switch (event.type) {
                case GAME_EV_KEY_UP:
                    input_toggle(GAME_KEYCODE(keyEvent), 0);
                    break;
                case GAME_EV_KEY_DOWN:
                    input_toggle(GAME_KEYCODE(keyEvent), 1);
                    break;
                case GAME_EV_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }

        needsFlip = false;
        flipXMin = winWidth;
        flipXMax = 0;
        flipYMin = winHeight;
        flipYMax = 0;

        if (MAX_FPS >= 30) {
            if (now < nextExceptedFrameRenderTime) {
                goto SKIP_RENDER;
            } else {
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

                if (screen_px != prevBuf[index]) {
                    prevBuf[index] = screen_px;
                    needsFlip = true;

                    int xmin = pixel.x;
                    int xmax = xmin + SCALE;
                    int ymin = pixel.y;
                    int ymax = ymin + SCALE;

                    if (xmin < flipXMin) flipXMin = xmin;
                    if (xmax > flipXMax) flipXMax = xmax;
                    if (ymin < flipYMin) flipYMin = ymin;
                    if (ymax > flipYMax) flipYMax = ymax;

                    /* Convert the palette index into a 32-bit colour. */
                    Uint32 mapped_color = SDL_MAP_RGB(surface, sdl_colors[screen_px].r, sdl_colors[screen_px].g, sdl_colors[screen_px].b);

                    #if SCALE == 1
                        ((Uint32*)surface->pixels)[ y * (surface->pitch / 4) + x] = mapped_color;
                    #elif SCALE == 2
                        for (int sub_y = 0; sub_y < SCALE; sub_y++) {
                            for (int sub_x = 0; sub_x < SCALE; sub_x++) {
                                int dest_x = x * SCALE + sub_x;
                                int dest_y = y * SCALE + sub_y;

                                ((Uint32*)surface->pixels)[ dest_y * (surface->pitch / 4) + dest_x] = mapped_color;
                            }
                        }
                    #else
                        SDL_FILL_RECT(surface, &pixel, mapped_color);
                    #endif
                }
            }
        }

    SKIP_RENDER:
        /* Update the window surface, but only the rectangle that changed. */
        if (needsFlip) {
#ifdef USE_SDL1
            /* SDL1: a plain double buffer flip of the whole display. */
            SDL_Flip(surface);
#else
            /* Only the rectangle that changed. */
            SDL_Rect updateRect = { flipXMin, flipYMin, flipXMax - flipXMin, flipYMax - flipYMin };

            SDL_UpdateWindowSurfaceRects(window, &updateRect, 1);
#endif
        }

        if (now - lastPrinted > 1000000) {
            LOG_INFO("%d ticks, %d fps", ticks, frames);

            g_ticks = ticks;
            g_frames = frames;
            ticks = 0;
            frames = 0;
            lastPrinted = now;
        }

        lastTime = now;
    }

QUIT:
    delete(prevBuf);

    /* Shut SDL down and release everything. */
    sound_quit();
    SDL_Quit();
    crafting_free();
    game_screen.free(&game_screen);
    game_lightScreen.free(&game_lightScreen);

    /*
     * The levels only exist once the game has been entered: while the title
     * menu is up, game_reset() returns before creating them and their free
     * pointer is still null, hence the check.
     */
    for (int i = 0; i < 5; ++i) {
        if (game_levels[i].free) {
            game_levels[i].free(game_levels + i);
        }
    }

    if (game_player) {
        game_player->mob.entity.free(&game_player->mob.entity);
        delete(game_player);
    }

    return ret;
}
