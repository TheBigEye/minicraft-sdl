/*
 * sdlcompat.h - The one place that knows how SDL 1.2, SDL2 and SDL3 differ.
 *
 * The port builds against any of the three; the Makefile picks SDL3 by
 * default and walks down to SDL2 and then to SDL1 when a newer one is not
 * installed (SDL=1, SDL=2 or SDL=3 pin a version and stop the walk).
 *
 * Everything else in the tree is written once, against the SDL2 shape. This
 * header supplies the names SDL3 renamed, events gained an SDL_EVENT_
 * prefix, the keysym indirection is gone, the pixel format is no longer a
 * struct hanging off the surface, FillRect became FillSurfaceRect and
 * CreateWindow lost its position arguments, plus the two that SDL1 spelled
 * differently. Keeping them here means game.c and sound.c stay readable
 * instead of turning into a thicket of nested #ifdefs.
 */
#ifndef SDLCONFIG_H_
#define SDLCONFIG_H_ 1

#if defined(USE_SDL3)
    #include <SDL3/SDL.h>
#elif defined(USE_SDL1)
    #include <SDL/SDL.h>
#else
    #include <SDL2/SDL.h>
#endif


/*
 * Did an SDL initialisation call succeed?
 *
 * SDL 1.2 and SDL2 return 0 on success and a negative code on failure; SDL3
 * returns true on success and false on failure. The two tests are exact
 * opposites, which is an easy way to turn a perfectly good build into a
 * mysterious "SDL_Init failed" with an empty SDL_GetError().
 */
#ifdef USE_SDL3
    #define SDL_INIT_SUCCEEDED(call) ((call) != false)
#else
    #define SDL_INIT_SUCCEEDED(call) ((call) == 0)
#endif


/* --- window ------------------------------------------------------------ */

#ifdef USE_SDL1
    /*
     * SDL1 has no window objects: SDL_SetVideoMode hands back the display
     * surface, and that is what "the window" means throughout the port.
     */
    typedef SDL_Surface* SDL_Window;
    #define SDL_WINDOWPOS_UNDEFINED 0
#endif

/* SDL3 dropped the x/y position; the window manager places the window. */
#ifdef USE_SDL3
    #define SDL_CREATE_WINDOW(title, w, h, flags) \
        SDL_CreateWindow((title), (w), (h), (flags))
#else
    #define SDL_CREATE_WINDOW(title, w, h, flags) \
        SDL_CreateWindow((title), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (w), (h), (flags))
#endif


/* --- events ------------------------------------------------------------ */

#ifdef USE_SDL3
    #define GAME_EV_QUIT           SDL_EVENT_QUIT
    #define GAME_EV_KEY_DOWN       SDL_EVENT_KEY_DOWN
    #define GAME_EV_KEY_UP         SDL_EVENT_KEY_UP
    #define GAME_EV_FOCUS_LOST     SDL_EVENT_WINDOW_FOCUS_LOST
    #define GAME_EV_FOCUS_GAINED   SDL_EVENT_WINDOW_FOCUS_GAINED
#else
    #define GAME_EV_QUIT           SDL_QUIT
    #define GAME_EV_KEY_DOWN       SDL_KEYDOWN
    #define GAME_EV_KEY_UP         SDL_KEYUP
    /*
     * SDL2 bundles every window change under SDL_WINDOWEVENT and tells them
     * apart with a sub-type. SDL3 gave each its own event. The sub-type
     * names exist here too, so the switch below reads the same either way;
     * with SDL3 they simply never match, because the events arrive under
     * GAME_EV_FOCUS_LOST / GAME_EV_FOCUS_GAINED instead.
     */
    #define GAME_EV_FOCUS_LOST     SDL_WINDOWEVENT_FOCUS_LOST
    #define GAME_EV_FOCUS_GAINED   SDL_WINDOWEVENT_FOCUS_GAINED
    #define GAME_EV_WINDOW         SDL_WINDOWEVENT
#endif

/*
 * The keycode carried by a keyboard event.
 *
 * SDL1 and SDL2 hide it one level deeper, behind the keysym struct; SDL3
 * removed that indirection. Takes an SDL_KeyboardEvent*, which is what the
 * event loop keeps a typed pointer to.
 */
#ifdef USE_SDL3
    #define GAME_KEYCODE(ev) ((ev)->key)
#else
    #define GAME_KEYCODE(ev) ((ev)->keysym.sym)
#endif

/*
 * Focus tracking, the only window event the game cares about.
 *
 * SDL1 reports it through SDL_ACTIVEEVENT with a state mask; SDL2 through
 * SDL_WINDOWEVENT plus a sub-type; SDL3 through two events of its own.
 */
#ifdef USE_SDL3
    #define GAME_IS_FOCUS_LOST(ev)     ((ev)->type == GAME_EV_FOCUS_LOST)
    #define GAME_IS_FOCUS_GAINED(ev)   ((ev)->type == GAME_EV_FOCUS_GAINED)
#elif defined(USE_SDL1)
    #define GAME_IS_FOCUS_LOST(ev)                                  \
        (((ev)->type == SDL_ACTIVEEVENT) &&                         \
         ((ev)->active.state & SDL_APPACTIVE) && !(ev)->active.gain)
    #define GAME_IS_FOCUS_GAINED(ev)                                \
        (((ev)->type == SDL_ACTIVEEVENT) &&                         \
         ((ev)->active.state & SDL_APPACTIVE) &&  (ev)->active.gain)
#else
    #define GAME_IS_FOCUS_LOST(ev)                                  \
        (((ev)->type == GAME_EV_WINDOW) &&                          \
         ((ev)->window.event == GAME_EV_FOCUS_LOST))
    #define GAME_IS_FOCUS_GAINED(ev)                                \
        (((ev)->type == GAME_EV_WINDOW) &&                          \
         ((ev)->window.event == GAME_EV_FOCUS_GAINED))
#endif


/* --- pixels ------------------------------------------------------------ */

/*
 * SDL3 split the pixel format in two: the surface stores an SDL_PixelFormat
 * enumeration value, and the byte layout SDL_MapRGB wants lives behind
 * SDL_GetPixelFormatDetails(). The palette moved off the format too and is
 * reached with SDL_GetSurfacePalette().
 */
#ifdef USE_SDL3
    #define SDL_MAP_RGB(surface, r, g, b)                                    \
        SDL_MapRGB(SDL_GetPixelFormatDetails((surface)->format),              \
                   SDL_GetSurfacePalette(surface), (r), (g), (b))

    #define SDL_FILL_RECT(surface, rect, color) \
        SDL_FillSurfaceRect((surface), (rect), (color))
#else
    #define SDL_MAP_RGB(surface, r, g, b) \
        SDL_MapRGB((surface)->format, (r), (g), (b))

    #define SDL_FILL_RECT(surface, rect, color) \
        SDL_FillRect((surface), (rect), (color))
#endif

/*
 * Painting the 256-entry colour cube onto the window surface. SDL1 reaches
 * for SDL_SetColors(), which takes the surface itself; SDL2 and SDL3 set the
 * colours of the surface's palette.
 */
#ifdef USE_SDL1
    #define SDL_SET_PALETTE_COLORS(surface, colors, first, n) \
        SDL_SetColors((surface), (SDL_Color*) (colors), (first), (n))
#elif defined(USE_SDL3)
    #define SDL_SET_PALETTE_COLORS(surface, colors, first, n) \
        SDL_SetPaletteColors(SDL_GetSurfacePalette(surface), (colors), (first), (n))
#else
    #define SDL_SET_PALETTE_COLORS(surface, colors, first, n) \
        SDL_SetPaletteColors((surface)->format->palette, (colors), (first), (n))
#endif

/* Whether the window surface is palettised at all, i.e. worth painting. */
#ifdef USE_SDL3
    #define SDL_SURFACE_PALETTE(surface) SDL_GetSurfacePalette(surface)
#else
    #define SDL_SURFACE_PALETTE(surface) ((surface)->format->palette)
#endif


/* --- hints ------------------------------------------------------------- */

/*
 * The video driver hint. SDL3 renamed it, along with the rest of the
 * subsystem-prefixed hints: SDL_HINT_VIDEODRIVER became
 * SDL_HINT_VIDEO_DRIVER.
 */
#ifdef USE_SDL3
    #define SDL_HINT_VIDEODRIVER_NAME SDL_HINT_VIDEO_DRIVER
#else
    #define SDL_HINT_VIDEODRIVER_NAME SDL_HINT_VIDEODRIVER
#endif

/*
 * SDL2 returns int and SDL3 returns bool from SDL_SetHint(); neither result
 * matters here, and -Wunused-result should not be able to fire on it.
 */
#define SDL_SET_HINT(name, value) do { (void) SDL_SetHint((name), (value)); } while (0)

#endif /* SDLCONFIG_H_ */
