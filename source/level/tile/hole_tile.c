#include "tile.h"
#include "../../gfx/color.h"

void holetile_init(TileID id){
	tile_init(id);
	
	Tile* t = tiles + id;
	t->connectsToSand = 1;
	t->connectsToWater = 1;
	t->connectsToLava = 1;
}

void holetile_render(TileID id, Screen* screen, Level* level, int x, int y){
	int col = getColor4(111, 111, 110, 110);
	int transitionColor1 = getColor4(3, 111, level->dirtColor - 111, level->dirtColor);
	int transitionColor2 = getColor4(3, 111, level->sandColor - 110, level->sandColor);

	char u = !tile_connectsToLiquid(level_get_tile(level, x, y - 1));
	char d = !tile_connectsToLiquid(level_get_tile(level, x, y + 1));
	char l = !tile_connectsToLiquid(level_get_tile(level, x - 1, y));
	char r = !tile_connectsToLiquid(level_get_tile(level, x + 1, y));

	char su = u && tiles[level_get_tile(level, x, y - 1)].connectsToSand;
	char sd = d && tiles[level_get_tile(level, x, y + 1)].connectsToSand;
	char sl = l && tiles[level_get_tile(level, x - 1, y)].connectsToSand;
	char sr = r && tiles[level_get_tile(level, x + 1, y)].connectsToSand;

	if (!u && !l) render_screen(screen, x * 16 + 0, y * 16 + 0, 0, col, 0);
	else render_screen(screen, x * 16 + 0, y * 16 + 0, (l ? 14 : 15) + (u ? 0 : 1) * 32, (su || sl) ? transitionColor2 : transitionColor1, 0);

	if (!u && !r) render_screen(screen, x * 16 + 8, y * 16 + 0, 1, col, 0);
	else render_screen(screen, x * 16 + 8, y * 16 + 0, (r ? 16 : 15) + (u ? 0 : 1) * 32, (su || sr) ? transitionColor2 : transitionColor1, 0);

	if (!d && !l) render_screen(screen, x * 16 + 0, y * 16 + 8, 2, col, 0);
	else render_screen(screen, x * 16 + 0, y * 16 + 8, (l ? 14 : 15) + (d ? 2 : 1) * 32, (sd || sl) ? transitionColor2 : transitionColor1, 0);
	
	if (!d && !r) render_screen(screen, x * 16 + 8, y * 16 + 8, 3, col, 0);
	else render_screen(screen, x * 16 + 8, y * 16 + 8, (r ? 16 : 15) + (d ? 2 : 1) * 32, (sd || sr) ? transitionColor2 : transitionColor1, 0);
}
