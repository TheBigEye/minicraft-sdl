/*
 * log.h - The game's logging levels.
 *
 * It replaces the printf() debug calls that used to be scattered over the
 * code: every message now has a level and is compiled out when it does not
 * apply, so a release build does not clutter the console.
 *
 *   LOG_ERROR(...)  things that stop the game (cannot open the window...)
 *   LOG_WARN(...)   unexpected but recoverable conditions (a menu that does
 *                   not exist, an entity that should not have died...)
 *   LOG_INFO(...)   startup and configuration information (the available
 *                   video drivers, the audio format...)
 *   LOG_TRACE(...)  development tracing (positions, counters, "WAT")
 *
 * LOG_LEVEL values:
 *   0 total silence | 1 errors | 2 errors+warnings | 3 +info | 4 +trace
 *
 * Default: release = 2, DEBUG=1 = 4. Overridable with `make LOG=n`.
 *
 * Arguments are still checked even when the level is off: the fprintf lives
 * inside an `if (0)` that the optimizer removes, so -Wformat keeps
 * validating the format strings in release, and no variable is left "unused"
 * because it only appears inside a LOG.
 */
#ifndef LOG_H
#define LOG_H 1

#include <stdio.h>

#ifndef LOG_LEVEL
    #ifdef DEBUG_BUILD
        #define LOG_LEVEL 4
    #else
        #define LOG_LEVEL 2
    #endif /* DEBUG_BUILD */
#endif /* LOG_LEVEL */

/* The "[file:line] " prefix shared by every level. */
#define LOG_PREFIX fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__)

/*
 * Base macro: when the level is enabled it prints to stderr with its tag;
 * otherwise the code disappears but the arguments stay part of the program
 * as far as -Wformat and -Wunused are concerned.
 */
#define LOG_AT(level_, tag_, ...)                                            \
    do {                                                                     \
        if (LOG_LEVEL >= (level_)) {                                         \
            LOG_PREFIX;                                                      \
            fprintf(stderr, tag_);                                           \
            fprintf(stderr, __VA_ARGS__);                                    \
            fputc('\n', stderr);                                             \
        } else if (0) {                                                      \
            fprintf(stderr, __VA_ARGS__);                                    \
        }                                                                    \
    } while (0)

#define LOG_ERROR(...) LOG_AT(1, "error: ", __VA_ARGS__)
#define LOG_WARN(...)  LOG_AT(2, "warn:  ", __VA_ARGS__)
#define LOG_INFO(...)  LOG_AT(3, "info:  ", __VA_ARGS__)
#define LOG_TRACE(...) LOG_AT(4, "trace: ", __VA_ARGS__)

#endif /* LOG_H */
