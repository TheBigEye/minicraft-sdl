/*
 * javalang.h - A vocabulary layer that lets this C port read like Java.
 *
 * Minicraft's Java source uses `public`, `private`, `static`, `final`,
 * `boolean`, `true`, `false`, `null`, `this` and `new`. C has no equivalent
 * for most of them, so this header supplies what can be supplied and states
 * plainly what cannot.
 *
 * ACCESS MODIFIERS
 *
 *   PUBLIC    Expands to nothing. Marks a declaration that belongs to the
 *             class API, i.e. one that appears in the header.
 *
 *   PRIVATE   Compiles to `static`. Use it at FILE SCOPE, in front of the
 *             helpers a class keeps to itself. Never use it inside a struct:
 *             `static` is illegal there, so the compiler catches the slip.
 *
 *   STATIC    Compiles to `static`. Use it at file scope for the members
 *             Java would call static, meaning shared by every instance
 *             rather than per object.
 *
 *   FINAL     Compiles to `const`.
 *
 * There is deliberately no marker for struct members. C has no member level
 * access control, so a macro there could only ever be a comment pretending
 * to be a keyword. Every class that must be embedded by value (the whole
 * inheritance chain, plus ArrayList and Random) keeps a fully visible
 * struct; the classes that nothing embeds are genuinely opaque instead.
 *
 * WHAT IS MISSING, AND WHY
 *
 *   extends   C cannot express it usefully. It could only expand to a
 *             member name, and two levels of inheritance would then read
 *             `this->super.super.x`, which is further from Java than
 *             naming the base after its class: `this->mob.entity.x`.
 *             Inheritance is still the first-member rule, just written out.
 *
 *   this      Not a macro: it is the name every method gives its receiver,
 *             which C accepts as an ordinary identifier.
 */

#ifndef JAVALANG_H
#define JAVALANG_H 1

#include <stddef.h>
#include <stdlib.h>

/* ---- Access modifiers -------------------------------------------------- */

/** Belongs to the class API: declared in the header. */
#define PUBLIC

/** Internal to this class's .c file. File scope only. */
#define PRIVATE   static

/** Shared by every instance rather than per object. File scope only. */
#define STATIC    static

/** Cannot be reassigned or written through. */
#define FINAL     const
#define final     const

/* ---- Primitive types --------------------------------------------------- */

/**
 * Java's `boolean`. Plain `char`, which is what the port already used for
 * every predicate, so this is a drop-in rename and not a behaviour change.
 */
typedef char boolean;

#ifndef true
#define true   1
#endif /* true */

#ifndef false
#define false  0
#endif /* false */

/** Java's `null`. */
#ifndef null
#define null   NULL
#endif /* null */

/**
 * Java's `String`. Only for owned, mutable buffers: assigning a literal to
 * it draws -Wdiscarded-qualifiers, so literals stay `const char*`.
 */
typedef char* String;

/* ---- Overloading ------------------------------------------------------- */

/**
 * C has no overloads, but Java's Minicraft uses them in a handful of
 * places: Entity.hurt and Mob.hurt (a Mob or a Tile can be the source),
 * Inventory.add (with or without a slot), Color.get (one shade triple or
 * four), Player.use, and the Tile.hurt families. Three macros stand in.
 *
 *   LANG_OVERLOAD(prefix, ...)  dispatches on how many arguments there
 *                   are: it glues the count to the prefix, so
 *                   LANG_OVERLOAD(add, a, b) becomes add_2(a, b). Use it
 *                   through a small wrapper macro, which is the name the
 *                   rest of the code calls:
 *
 *                       #define inventory_add(...) \
 *                           LANG_OVERLOAD(inventory_add, __VA_ARGS__)
 *
 *                   so that inventory_add(&inv, item) becomes add_2 and
 *                   inventory_add(&inv, 0, item) becomes add_3.
 *
 *                   The per-arity implementations are usually PRIVATE and
 *                   are named only in the header comment; callers see one
 *                   name, exactly as in Java.
 *
 *   LANG_FIRST(...) hands back the first argument, so that _Generic can
 *                   choose an implementation from the type of that
 *                   argument. This is the one to reach for when two
 *                   overloads take the same number of arguments and differ
 *                   only in the type of one of them.
 *
 *                       #define game_set_menu(m)                      \
 *                           _Generic((m),                             \
 *                               menu_id: game_set_menu_id,            \
 *                               int:     game_set_menu_id,            \
 *                               default: game_set_menu_obj)(m)
 *
 *                   Two details make it work, and both are easy to get
 *                   wrong:
 *
 *                   - the branches hold the *function name*, never a call.
 *                     GCC type-checks every branch, selected or not, so a
 *                     branch written as `game_set_menu_id((m))` is checked
 *                     even when `m` is a pointer, and the build fails. The
 *                     call goes on the outside, applied to whichever
 *                     function the type picks.
 *                   - enumeration *constants* are of type int in C, not of
 *                     the enum type, so `mid_TITLE` needs an `int:` branch
 *                     next to the `menu_id:` one for variables.
 *
 *                   The overloads may then take different parameter types,
 *                   which is the whole point.
 *
 *   LANG_NARG(...)  counts its arguments, from 1 to 8. It is the engine
 *                   under LANG_OVERLOAD and is also useful on its own.
 *
 * One thing none of them can do is overload a method slot: a struct member
 * has a single fixed signature, so a class that Java overloads still needs
 * one slot per signature (`hurt` and `hurt_tile`, say). What the macros buy
 * is a single name at the call site that picks the right slot, virtual
 * dispatch included.
 */
#define LANG_NARG(...)                         LANG_NARG_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)
#define LANG_NARG_(_1, _2, _3, _4, _5, _6, _7, _8, N, ...)   N

#define LANG_CAT(a, b)                         LANG_CAT_(a, b)
#define LANG_CAT_(a, b)                        a ## b

#define LANG_OVERLOAD(prefix, ...)             LANG_CAT(prefix ## _, LANG_NARG(__VA_ARGS__))(__VA_ARGS__)

#define LANG_FIRST(first, ...)                 first

/* ---- Allocation -------------------------------------------------------- */

/**
 * Allocates one zeroed instance of `type`. It does NOT run the constructor:
 * call it yourself, exactly as Java calls the constructor after allocating.
 *
 *     Player* p = new(Player);
 *     player_create(p);
 */
#define new(type)           ((type*) calloc(1, sizeof(type)))

/** Allocates a zeroed array of `n` elements of `type`. */
#define new_array(type, n)  ((type*) calloc((size_t) (n), sizeof(type)))

/** Frees `obj` and clears the pointer, so a second delete is harmless. */
#define delete(obj)         do { free((void*) (obj)); (obj) = null; } while (0)

#endif /* JAVALANG_H */
