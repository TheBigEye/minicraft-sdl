# Minicraft SDL

A C port of **Minicraft** - the game that Markus "Notch" Persson wrote in 48 hours for
*Ludum Dare 22* (December 2011) now built on C and SDL.

The port keeps the original Java game's logic, classes and file organization as
close as possible (reference: [skeeto/Minicraft](https://github.com/skeeto/Minicraft))
while being fully playable and **completable**: gather resources, craft tools,
farm, fight monsters, dig down to the deepest caves, and climb to the sky
island to defeat the **Air Wizard**.

---

### Requirements

The build needs **one** of SDL 1.2, SDL2 or SDL3. Which one is picked is described under [Build options](#build-options); anything installed will do.

| Platform | Packages |
|---|---|
| Debian/Ubuntu | `build-essential`, `libsdl3-dev` (or `libsdl2-dev`, or `libsdl1.2-dev`). Note `libsdl3-dev` only exists from Ubuntu 25.10 onwards. |
| Fedora | `gcc`, `make`, `SDL3-devel` |
| macOS (Homebrew) | Xcode command line tools (Apple clang, `make`), then `brew install pkgconf sdl3`. Homebrew does not package SDL 1.2, so SDL3 is the supported flavor on macOS. |
| Windows x64 (MSYS2 MINGW64) | `mingw-w64-x86_64-gcc`, `mingw-w64-x86_64-make`, `mingw-w64-x86_64-sdl3` (or `mingw-w64-x86_64-SDL2`, or `mingw-w64-x86_64-SDL`) |
| Windows x86 (MSYS2 MINGW32) | `mingw-w64-i686-gcc`, `mingw-w64-i686-make`, `mingw-w64-i686-SDL2` (or `mingw-w64-i686-SDL`). MSYS2 has **no** 32-bit SDL3, so SDL3 on i686 Windows means building SDL3 from source. |
| Embedded / cross | any GCC cross toolchain via `CC=...`; add `NO_AUDIO=1` / `FB=1` as needed |

`python3` is **optional**: it is only needed to re-pack assets when you modify them the generated C arrays are committed to the repo.

### Build & run

```sh
make          # build the game
make run      # build and launch
```

On Windows (MSYS2 shell): `mingw32-make` (or `make`) produces `game.exe`.

On macOS: `make CC=cc SDL=3` builds `game` with Apple clang. Use `CC=cc`
because a bare `gcc` binary is not guaranteed to exist; SDL3 is selected on
purpose because Homebrew has no SDL 1.2 package.

### Build options

| Command | Effect |
|---|---|
| `make` | Auto: uses the newest SDL installed. |
| `make SDL=3` | SDL3, pinned: fails with a clear message if it is not installed |
| `make SDL=2` | SDL2, pinned |
| `make SDL=1` | SDL 1.2, pinned |
| `make FB=1` | Prefer the Linux framebuffer: KMSDRM (SDL2/SDL3) or fbcon (SDL1) |
| `make NO_AUDIO=1` | Silent build - no audio code at all (embedded targets) |
| `make DEBUG=1` | Debug build (`-g -O0`), used by the VSCode F5 launcher |
| `CC=riscv64-linux-gnu-gcc make SDL=1 FB=1` | Cross-compilation example |

Options combine, e.g. `make SDL=1 NO_AUDIO=1 DEBUG=1`.

With no `SDL=` (or `SDL=auto`) the Makefile walks 3 -> 2 -> 1 and uses the
first one it finds; when none is installed it stops and tells you what to
install. Naming a version pins it: the build then uses that one or fails, it
never quietly substitutes a different library.

### Make targets

| Target | Description |
|---|---|
| `make` / `make all` | Build the `game` binary |
| `make run` | Build and run |
| `make clean` | Remove objects and the binary |
| `make assets` | Force re-packing of `assets/*.png`/`*.wav` into C arrays (needs `python3`) |
| `make vsconfig` | Generate `.vscode/` configuration for this machine |

---

## Controls

| Action | Keys |
|---|---|
| Move | Arrow keys, or `W` `A` `S` + `6` for right (yes, `6`, a quirk of the original keymap, preserved) |
| Attack / use / place | `Space`, `C`, `Left Ctrl`, `Insert` |
| Menu / confirm / back | `Enter`, `X`, `Tab`, `Alt` |

The in-game menu also shows hints: *(ARROW KEYS, X AND C)*.


## Code layout

```
assets/        original game resources (PNG spritesheet, WAV sounds)
scripts/       Python 3 asset packers (stdlib only)
docs/          screenshots and documentation
source/
  game.*           main loop, window creation, game state, win/death logic
  sdlcompat.h      absorbs the SDL 1.2 / SDL2 / SDL3 differences
  inputhandler.*   keyboard state (up/down/left/right/attack/menu)
  gfx/             screen buffers, spritesheet, font drawing, light map
  level/           level generation, tiles and tile behavior
  entity/          entity system: vtables, mobs, furniture, items, particles
  item/            items and tools (including power glove)
  crafting/        crafting recipes
  screen/          menus (title, inventory, crafting, containers, ...)
  sound/           SDL audio-callback mixer
  extern/          packed assets (icons_data.*, sound_data.c)
```

---

## VSCode

```sh
make vsconfig             # default (auto-detected SDL) configuration
make vsconfig SDL=1       # or any combination of SDL= / FB= / NO_AUDIO= / DEBUG=
```


`vsconfig` detects your toolchain (compiler path, target triple, system include directories via `gcc -E -Wp,-v`, gdb location) and writes:
- `.vscode/c_cpp_properties.json`, IntelliSense configured with the detected compiler, include paths and the defines of the requested build flavor.
- `.vscode/tasks.json`, tasks for every build variant (`build`, `build-debug`, `build-sdl3`, `build-sdl2`, `build-sdl1`, `build-fb`, `build-no-audio`), plus `run`, `clean` and `assets`, all wired to the GCC problem matcher.
- `.vscode/launch.json`, F5 debugging with gdb, including a `build-debug` pre-launch task (`make DEBUG=1`).
- `.vscode/settings.json`, workspace settings: compiler, `gnu11`, C headers, and the IntelliSense setup.

Requires GNU Make >= 4.0.

---

## Object-oriented style

The port is C, but it is written to **read as close to the Java original as
possible**. One class per `struct`, methods as function pointers installed by
the constructor, and the receiver passed explicitly as `this`:

```c
Tile* t = level->get_tile(level, x, y);             /* Java: level.getTile(x, y) */
t->stepped_on(t, level, x, y, entity);              /* Java: t.steppedOn(level, x, y, entity) */
int n = this->random.next_int(&this->random, 4);    /* Java: random.nextInt(4)   */
if (current_menu) current_menu->tick(current_menu); /* Java: if (menu != null) menu.tick(); */
```
---

## Status & contributing

This project is in **active development**. The goal is not only to keep the
original game alive and building on modern (and not so modern) platforms, but
also to expand it over time with new content and features, always respecting
the style and spirit of the 2011 original.

Any contribution is welcome: bug fixes, portability work, new features or
documentation. Open an issue or a pull request and jump in.

---

## Credits

- **Markus "Notch" Persson**, the original *Minicraft* for Ludum Dare 22.
- Java sources used as logic reference: [skeeto/Minicraft](https://github.com/skeeto/Minicraft).

*Fan-made port for preservation and learning purposes.*
