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

| Platform | Packages |
|---|---|
| Debian/Ubuntu | `build-essential`, `libsdl2-dev` (or `libsdl1.2-dev` for `SDL=1`) |
| Fedora | `gcc`, `make`, `SDL2-devel` |
| Windows x86 (MSYS2 MINGW32) | `mingw-w64-i686-gcc`, `mingw-w64-i686-make`, `mingw-w64-i686-SDL2` (or `mingw-w64-i686-SDL`) |
| Windows x64 (MSYS2 MINGW64) | `mingw-w64-x86_64-gcc`, `mingw-w64-x86_64-make`, `mingw-w64-x86_64-SDL2` (or `mingw-w64-x86_64-SDL`) |
| Embedded / cross | any GCC cross toolchain via `CC=...`; add `NO_AUDIO=1` / `FB=1` as needed |

`python3` is **optional**: it is only needed to re-pack assets when you modify them the generated C arrays are committed to the repo.

### Build & run

```sh
make          # build the game (SDL2 by default)
make run      # build and launch
```

On Windows (MSYS2 shell): `mingw32-make` (or `make`) produces `game.exe`.

### Build options

| Command | Effect |
|---|---|
| `make` | SDL2 (default) |
| `make SDL=1` | SDL 1.2 instead of SDL2 |
| `make FB=1` | Prefer the Linux framebuffer: KMSDRM (SDL2) or fbcon (SDL1) |
| `make NO_AUDIO=1` | Silent build - no audio code at all (embedded targets) |
| `make DEBUG=1` | Debug build (`-g -O0`), used by the VSCode F5 launcher |
| `CC=riscv64-linux-gnu-gcc make SDL=1 FB=1` | Cross-compilation example |

Options combine, e.g. `make SDL=1 NO_AUDIO=1 DEBUG=1`.

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
  inputhandler.*   keyboard state (up/down/left/right/attack/menu)
  gfx/             screen buffers, spritesheet, font drawing, light map
  level/           level generation, tiles and tile behavior
  entity/          entity system: vtables, mobs, furniture, items, particles
  item/            items and tools (including power glove)
  crafting/        crafting recipes
  screen/          menus (title, inventory, crafting, containers, …)
  sound/           SDL audio-callback mixer
  generated/       packed assets (icons_data.*, sound_data.c) - committed
```

---

## VSCode

```sh
make vsconfig             # default (SDL2) configuration
make vsconfig SDL=1       # or any combination of SDL= / FB= / NO_AUDIO= / DEBUG=
```

`vsconfig` detects your toolchain (compiler path, target triple, system include directories via `gcc -E -Wp,-v`, gdb location) and writes:
- `.vscode/c_cpp_properties.json`, IntelliSense configured with the detected compiler, include paths and the defines of the requested build flavor.
- `.vscode/tasks.json`, tasks for every build variant (`build`, `build-debug`, `build-sdl1`, `build-fb`, `build-no-audio`), plus `run`, `clean` and `assets`, all wired to the GCC problem matcher.
- `.vscode/launch.json`, F5 debugging with gdb, including a `build-debug` pre-launch task (`make DEBUG=1`).

Requires GNU Make ≥ 4.0.

---

## Credits

- **Markus "Notch" Persson**, the original *Minicraft* for Ludum Dare 22.
- Java sources used as logic reference: [skeeto/Minicraft](https://github.com/skeeto/Minicraft).

*Fan-made port for preservation and learning purposes.*
