#include "crafting/crafting.h"
#include "gfx/spritesheet.h"
#include "gfx/font.h"
#include <SDL2/SDL.h>
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
#include "entity/_entity_caller.h"
#include "item/item.h"
#include "icons.h"

Screen game_screen;
Screen game_lightScreen;

int g_ticks = 0; //perf measure
int g_frames = 0; //perf measure

unsigned long tickCount = 0;

SDL_Color sdl_colors[256];

enum menu_id current_menu;
char game_hasfocus = 0;
int game_pendingLevelChange = 0;
char updatePerfctr = 0;
char running = 1;
char isingame = 0;

const int MAX_FPS = -1;

void game_set_menu(enum menu_id menu){
	current_menu = menu;
	init_menu(menu);
}

int game_playerDeadTime = 0;
int game_wonTimer = 0;
int game_gameTime = 0;
char game_hasWon = 0;
Level game_levels[5] = {0};
int game_currentLevel;
Level* game_level;


Player* game_player = 0;

void game_changeLevel(int dir) {
	level_removeEntity1(game_level, (Entity *) game_player);
	game_currentLevel += dir;
	game_level = game_levels + game_currentLevel;
	game_player->mob.entity.x = (game_player->mob.entity.x >> 4) * 16 + 8;
	game_player->mob.entity.y = (game_player->mob.entity.y >> 4) * 16 + 8;
	level_addEntity(game_level, (Entity *) game_player);
}


void game_won(){
	game_wonTimer = 60 * 3;
	game_hasWon = 1;
}


void game_reset() {
	game_playerDeadTime = 0;
	game_wonTimer = 0;
	game_gameTime = 0;
	game_hasWon = 0;

	for (int i = 0; i < 5; ++i) {
		printf("Freeing level %d\n", i);
		level_free(game_levels + i);
	}

	if(!isingame) return;

	memset(game_levels, 0, sizeof(game_levels));

	game_currentLevel = 3;
	level_init(game_levels + 4, 128, 128, 1, 0);
	level_init(game_levels + 3, 128, 128, 0, game_levels + 4);
	level_init(game_levels + 2, 128, 128, -1, game_levels + 3);
	level_init(game_levels + 1, 128, 128, -2, game_levels + 2);
	level_init(game_levels + 0, 128, 128, -3, game_levels + 1);

	if (game_player) {
		call_entity_free((Entity *) game_player);
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

void game_init(){
	levelgen_preinit();
	font_pre_init();
	init_resources();
	init_tiles();
	init_menus();
	crafting_init();

	int pp = 0;
	for(int r = 0; r < 6; ++r){
		for(int g = 0; g < 6; ++g){
			for(int b = 0; b < 6; ++b){
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

void set_pixel(SDL_Surface* surface, int x, int y, int color){
	*(int*)(surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel) = color;
}
char teststr[] = "test uwu";

void game_tick(){
	++tickCount;
	if(!game_hasfocus){
		//TODO release all keys
	}else{
		if(isingame) if(!game_player->mob.entity.removed && !game_hasWon) ++game_gameTime;
		input_tick();

		if(current_menu){
			tick_menu(current_menu);
		}else{
			if(game_player->mob.entity.removed){
				++game_playerDeadTime;
				if(game_playerDeadTime > 60){
					game_set_menu(mid_DEAD);
				}
			}else{
				if(game_pendingLevelChange != 0){
					game_set_menu(mid_LEVEL_TRANSITION);
					game_pendingLevelChange = 0;
				}
			}

			if(game_wonTimer > 0){
				if(--game_wonTimer == 0){
					game_set_menu(mid_WON);
				}
			}

			level_tick(game_level);
			++tile_tickCount;
		}
	}
}

void game_renderGui(){

    #ifdef TEST_SHOWPORTALPOS
	if(isingame){
		char hax[64];
		int x = game_player->mob.entity.x >> 4;
		int y = game_player->mob.entity.y >> 4;
		sprintf(hax, "P %d %d\0", x, y);
		font_draw(hax, strlen(hax), &game_screen, 2, 2, getColor4(000, 200, 500, 533));
		int Scnt = 10;

		for(x = 0; x < game_player->mob.entity.level->w; ++x){
			for(y = 0; y < game_player->mob.entity.level->h; ++y){
				if(level_get_tile(game_player->mob.entity.level, x, y) == STAIRS_UP){
					sprintf(hax, "U %d %d\0", x, y);
					font_draw(hax, strlen(hax), &game_screen, 2, Scnt, getColor4(000, 200, 500, 533));
					Scnt += 8;
				}

				if(level_get_tile(game_player->mob.entity.level, x, y) == STAIRS_DOWN){
					sprintf(hax, "D %d %d\0", x, y);
					font_draw(hax, strlen(hax), &game_screen, 2, Scnt, getColor4(000, 200, 500, 533));
					Scnt += 8;
				}
			}
		}

		if(game_player->mob.entity.level->depth == 1){
			for(int i = 0; i < game_player->mob.entity.level->entities.size; ++i){
				Entity* e = game_player->mob.entity.level->entities.elements[i];
				if(e->type == AIRWIZARD){
					sprintf(hax, "W %d %d\0", e->x>> 4, e->y>> 4);
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

	sprintf(fpsticks, "%dfps %dticks\0", g_frames, g_ticks);
	font_draw(fpsticks, strlen(fpsticks), &game_screen, 2, 2, getColor4(000, 200, 500, 533));
    #endif

	for(int y = 0; y < 2; ++y){
		for(int x = 0; x < 20; ++x){
			render_screen(&game_screen, x*8, game_screen.h - 16 + y*8, 0 + 12 * 32, getColor4(0, 0, 0, 0), 0);
		}
	}
	if(isingame){
		for(int i = 0; i < 10; ++i){
			if(i < game_player->mob.health){
				render_screen(&game_screen, i * 8, game_screen.h - 16, 0 + 12 * 32, getColor4(000, 200, 500, 533), 0);
			}else{
				render_screen(&game_screen, i * 8, game_screen.h - 16, 0 + 12 * 32, getColor4(000, 100, 000, 000), 0);
			}

			if(game_player->staminaRechargeDelay > 0){
				if(game_player->staminaRechargeDelay / 4 % 2 == 0){
					render_screen(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, getColor4(000, 555, 000, 000), 0);
				}else{
					render_screen(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, getColor4(000, 110, 000, 000), 0);
				}
			}else{
				if(i < game_player->stamina){
					render_screen(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, getColor4(000, 220, 550, 553), 0);
				}else{
					render_screen(&game_screen, i * 8, game_screen.h - 8, 1 + 12 * 32, getColor4(000, 110, 000, 000), 0);
				}
			}
		}

		if(game_player->activeItem){
			item_renderInventory(game_player->activeItem, &game_screen, 10*8, game_screen.h - 16);
		}
	}
	if(current_menu){
		render_menu(current_menu, &game_screen);
	}
}

char click_to_focus[] = "Click to focus!";
void game_renderFocusNagger(){
	//click_to_focus
	int c2fLen = strlen(click_to_focus);
	int xx = (WIDTH - c2fLen * 8) / 2;
	int yy = (HEIGHT - 8) / 2;

	int w = c2fLen;
	int h = 1;

	render_screen(&game_screen, xx - 8, yy - 8, 0 + 13 * 32, getColor4(-1, 1, 5, 445), 0);
	render_screen(&game_screen, xx + w * 8, yy - 8, 0 + 13 * 32, getColor4(-1, 1, 5, 445), 1);
	render_screen(&game_screen, xx - 8, yy + 8, 0 + 13 * 32, getColor4(-1, 1, 5, 445), 2);
	render_screen(&game_screen, xx + w * 8, yy + 8, 0 + 13 * 32, getColor4(-1, 1, 5, 445), 3);

	for(int x = 0; x < w; ++x){
		render_screen(&game_screen, xx + x * 8, yy - 8, 1 + 13 * 32, getColor4(-1, 1, 5, 445), 0);
		render_screen(&game_screen, xx + x * 8, yy + 8, 1 + 13 * 32, getColor4(-1, 1, 5, 445), 2);
	}

	for(int y = 0; y < h; ++y){
		render_screen(&game_screen, xx - 8, yy + y * 8, 2 + 13 * 32, getColor4(-1, 1, 5, 445), 0);
		render_screen(&game_screen, xx + w * 8, yy + y * 8, 2 + 13 * 32, getColor4(-1, 1, 5, 445), 1);
	}

	if((tickCount / 20) % 2 == 0){
		font_draw(click_to_focus, c2fLen, &game_screen, xx, yy, getColor4(5, 333, 333, 333));
	}else{
		font_draw(click_to_focus, c2fLen, &game_screen, xx, yy, getColor4(5, 555, 555, 555));
	}
}

void game_render(){
	if(isingame){
		int xScroll = game_player->mob.entity.x - game_screen.w / 2;
		int yScroll = game_player->mob.entity.y - (game_screen.h - 8) / 2;

		if (xScroll < 16) xScroll = 16;
		if (yScroll < 16) yScroll = 16;
		if (xScroll > game_level->w * 16 - game_screen.w - 16) xScroll = game_level->w * 16 - game_screen.w - 16;
		if (yScroll > game_level->h * 16 - game_screen.h - 16) yScroll = game_level->h * 16 - game_screen.h - 16;

		if(game_currentLevel > 3){
			int col = getColor4(20, 20, 121, 121);

			for(int y = 0; y < 14; ++y){
				for(int x = 0; x < 24; ++x){
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
	if(!game_hasfocus){
		game_renderFocusNagger();
	}
}

int main(int argc, char** argv){
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

	// Declarar variables para la ventana y superficie
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	SDL_Event event;
	SDL_KeyboardEvent* keyEvent = (SDL_KeyboardEvent*) &event;
	SDL_Rect pixel = {0, 0, SCALE, SCALE};

	game_init();

	// Inicializar SDL y crear ventana
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		ret = 1;
		goto QUIT;
	}

	window = SDL_CreateWindow("Minicraft", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, 0);
	if (!window) {
		printf("Failed to create window: %s\n", SDL_GetError());
		ret = 1;
		goto QUIT;
	}

	SDL_SetWindowTitle(window, "Minicraft");

	// Obtener la superficie de la ventana
	surface = SDL_GetWindowSurface(window);

	if (surface == NULL){
		printf("Failed to get window surface: %s\n", SDL_GetError());
		ret = 1;
		goto QUIT;
	}

	// Configurar paleta para 8 bits (si la superficie tiene paleta)
	if(surface->format->palette != NULL){
		SDL_SetPaletteColors(surface->format->palette, sdl_colors, 0, 256);
	}


    #ifdef LEVELGENTEST
	{
		#define set_px(x, y, color) {\
			pixel.x = (x)*SCALE; \
			pixel.y = (y)*SCALE; \
			SDL_FillRect(surface, &pixel, color); \
		}
		int w = 128;
		int h = 128;
		unsigned char* map;
		unsigned char* data;
		pixel.w = SCALE;
		pixel.h = SCALE;
		createAndValidateSkyMap(&map, &data, w, h, 3);
		printf("gen stopped\n");

		// Crear una nueva ventana para la prueba de levelgen
		SDL_Window* genWindow = SDL_CreateWindow("Minicraft LevelGen Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w * SCALE, h * SCALE, 0);
		SDL_Surface* genSurface = SDL_GetWindowSurface(genWindow);

		for(int y = 0; y < h; ++y){
			for(int x = 0; x < w; ++x){
				int i = x + y * w;
				if(map[i] == WATER) set_px(x, y, 0x000080);
				if(map[i] == GRASS) set_px(x, y, 0x208020);
				if(map[i] == ROCK) set_px(x, y, 0xa0a0a0);
				if(map[i] == DIRT) set_px(x, y, 0x604040);
				if(map[i] == SAND) set_px(x, y, 0xa0a040);
				if(map[i] == TREE) set_px(x, y, 0x003000);
				if(map[i] == LAVA) set_px(x, y, 0xff2020);
				if(map[i] == CLOUD) set_px(x, y, 0xa0a0a0);
				if(map[i] == STAIRS_DOWN) set_px(x, y, 0xffffff);
				if(map[i] == STAIRS_UP) set_px(x, y, 0xffffff);
				if(map[i] == CLOUD_CACTUS) set_px(x, y, 0xff00ff);
			}
		}
		SDL_UpdateWindowSurface(genWindow);
		while(running)
		{
			while(SDL_PollEvent(&event)){
				switch(event.type){
					case SDL_QUIT:
						running = 0;
						break;
				}
			}
		}
		free(map);
		free(data);
		goto QUIT;
	}
    #endif


	prevBuf = malloc(game_screen.h * game_screen.w);
	for(int i = 0; i < game_screen.h * game_screen.w; ++i) prevBuf[i] = 0;
	game_hasfocus = 1;

	while(running) {

		now = getTimeUS();
		unprocessed += (now - lastTime) / usPerTick;


		while(unprocessed >= 1){
			++ticks;
			game_tick();
			--unprocessed;
		}

		while(SDL_PollEvent(&event))
		{
			switch(event.type){
				case SDL_KEYUP:
					input_toggle(keyEvent->keysym.sym, 0);
					break;
				case SDL_KEYDOWN:
					input_toggle(keyEvent->keysym.sym, 1);
					break;
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_WINDOWEVENT:
					// Para manejo de focus se utiliza el evento WINDOWEVENT_FOCUS_GAINED/LOST
					if(event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
						game_hasfocus = 0;
					else if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
						game_hasfocus = 1;
					break;
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

		for(int y = 0; y < game_screen.h; ++y){
			pixel.y = y * SCALE;

			for(int x = 0; x < game_screen.w; ++x){
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
			SDL_UpdateWindowSurfaceRects(window, &updateRect, 1);
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
	SDL_Quit();
	crafting_free();
	delete_screen(&game_screen);
	delete_screen(&game_lightScreen);

	for (int i = 0; i < 5; ++i) {
		// printf("Freeing level %d\n", i);
		level_free(game_levels + i);
	}

	if (game_player) {
		call_entity_free((Entity *) game_player);
		free(game_player);
	}

	return ret;
}
