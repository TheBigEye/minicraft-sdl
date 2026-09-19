# =============================================================================
#  Minicraft (SDL port) -- Makefile
# =============================================================================
#
#  Builds the game against SDL 1.2, SDL 2 or SDL 3, on Windows (MinGW) and
#  on Unix-like systems (Linux, macOS, BSDs). Plain GNU Make; the vsconfig
#  target additionally needs GNU Make >= 4.0.
#
#  ---------------------------------------------------------------------------
#  Targets
#  ---------------------------------------------------------------------------
#    all       (default) build the game into ./game (game.exe on Windows)
#    run       build, then run the game
#    clean     remove object files and the executable
#    assets    re-pack assets/icons.png and assets/*.wav into C sources
#    vsconfig  generate .vscode/ config for THIS machine (Make >= 4.0)
#
#  ---------------------------------------------------------------------------
#  Options (make VAR=value, or export VAR=value in the environment)
#  ---------------------------------------------------------------------------
#    SDL=1|2|3  pin one SDL version. Unset (or SDL=auto) takes the newest
#               installed, walking 3 -> 2 -> 1. A pinned version that is
#               missing is a hard error - never a silent fallback.
#    FB=1       prefer the framebuffer video driver (fbcon on SDL1,
#               KMSDRM on SDL2/3). Linux only.
#    NO_AUDIO=1 build without audio (for targets without sound support).
#    DEBUG=1    -g -O0, LOG=4 by default; the build "vsconfig" debugs with.
#    WERROR=0   relax the zero-warnings rule (exotic / very old toolchains).
#    LOG=0..4   log level: 0 silent | 1 errors | 2 +warnings | 3 +info |
#               4 +trace. Defaults: release 2, debug 4.
#    CC=...     cross-compile, e.g. CC=riscv64-linux-gnu-gcc make SDL=1 FB=1
#    PYTHON=... interpreter for the asset packers (default: python3)
#
#  ---------------------------------------------------------------------------
#  Examples
#  ---------------------------------------------------------------------------
#    make                                newest SDL installed
#    make SDL=3 FB=1                     SDL3 + framebuffer (Linux)
#    make SDL=1 NO_AUDIO=1               SDL 1.2, silent build (embedded)
#    make SDL=1 DEBUG=1                  SDL 1.2 debug build
#    make vsconfig SDL=1                 VSCode config for the SDL 1.2 build
#    CC=riscv64-linux-gnu-gcc make SDL=1 FB=1
#    Windows (MSYS2):  make SDL=1 | make SDL=2 | make SDL=3
#    macOS:            brew install sdl3 && make SDL=3
#
#  ---------------------------------------------------------------------------
#  Asset pipeline
#  ---------------------------------------------------------------------------
#    scripts/spritesheet2c.py converts assets/icons.png into
#    source/extern/icons_data.c/.h (same formula as the original Java
#    SpriteSheet: blue_channel / 64).
#    scripts/sound2c.py converts assets/*.wav into source/extern/sound_data.c
#    (mono/16-bit/44100 Hz arrays, played by the mixer in source/sound/sound.c).
#    The generated files are committed to the repo, so python3 is only needed
#    when the assets change (fallback for cross-compilation environments).
#
#  ---------------------------------------------------------------------------
#  Test hooks (source/, off by default)
#  ---------------------------------------------------------------------------
#    Seed CFLAGS through the environment to try the experimental defines:
#      CFLAGS="-DLEVELGENTEST -DGODMODE" make
#    Available defines: LEVELGENTEST, TEST_SHOWPORTALPOS, TEST_INVENTORY,
#    GODMODE.
#
#  ---------------------------------------------------------------------------
#  File layout (where to look for what)
#  ---------------------------------------------------------------------------
#    01  defaults & housekeeping
#    02  SDL detection
#    03  SDL flags per version
#    04  version selection
#    05  platform settings (Windows / Unix)
#    06  build flags (optimization, warnings, WERROR, logging)
#    07  sources & objects
#    08  asset packing (python3, optional)
#    09  build rules
#    10  targets
#    11  developer tooling (vsconfig)
# =============================================================================


# =============================================================================
# 01. Defaults & housekeeping
# =============================================================================
# (CC is overridable for cross-compilation, e.g. CC=riscv64-linux-gnu-gcc)
CC ?= gcc
PYTHON ?= python3

# Build options. ?= keeps the command line / environment in control.
SDL ?= auto
FB ?= 0
NO_AUDIO ?= 0
DEBUG ?= 0
WERROR ?= 1

SOURCE_DIR = source

# "all" is the default goal even though other targets (assets, vsconfig)
# appear earlier in this file.
.DEFAULT_GOAL := all
.PHONY: all run clean assets vsconfig
.DELETE_ON_ERROR:


# =============================================================================
# 02. SDL detection
# =============================================================================
# A package counts as installed when pkg-config can describe it or, where
# pkg-config is absent (a bare MinGW toolchain, for instance), when the
# compiler finds its main header on its own search path.
SDL3_FOUND := $(shell pkg-config sdl3 >/dev/null 2>&1 && echo yes)
ifeq ($(SDL3_FOUND),)
    SDL3_FOUND := $(shell printf '#include <SDL3/SDL.h>\n' | $(CC) -E -x c - >/dev/null 2>&1 && echo yes)
endif
SDL2_FOUND := $(shell pkg-config sdl2 >/dev/null 2>&1 && echo yes)
ifeq ($(SDL2_FOUND),)
    SDL2_FOUND := $(shell printf '#include <SDL2/SDL.h>\n' | $(CC) -E -x c - >/dev/null 2>&1 && echo yes)
endif
SDL1_FOUND := $(shell sdl-config --version >/dev/null 2>&1 && echo yes)
ifeq ($(SDL1_FOUND),)
    SDL1_FOUND := $(shell printf '#include <SDL/SDL.h>\n' | $(CC) -E -x c - >/dev/null 2>&1 && echo yes)
endif


# =============================================================================
# 03. SDL flags per version
# =============================================================================
# Compiler and linker flags per version. pkg-config is preferred; the
# fallbacks keep a build working where pkg-config is missing.
SDL3_CFLAGS := $(shell pkg-config --cflags sdl3 2>/dev/null)
SDL3_LIBS   := $(shell pkg-config --libs   sdl3 2>/dev/null || echo "-lSDL3")
# The Windows build links with -static, which makes ld prefer SDL3's static
# archive (libSDL3.a) over the import library. That archive still refers to
# the libraries SDL3 was compiled against - libiconv above all - and only the
# static closure lists them, so an ordinary pkg-config libs line leaves
# the link with undefined references to libiconv_open and friends.
SDL3_LIBS_STATIC := $(shell pkg-config --libs --static sdl3 2>/dev/null || echo "-lSDL3 -liconv")
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL2_LIBS   := $(shell pkg-config --libs   sdl2 2>/dev/null || echo "-lSDL2")
SDL1_CFLAGS := $(shell sdl-config --cflags 2>/dev/null)
SDL1_LIBS   := $(shell sdl-config --libs   2>/dev/null || echo "-lSDL")


# =============================================================================
# 04. Version selection
# =============================================================================
# SDL=1|2|3 pins one version and never substitutes another, so a build can
# not quietly come out against a library you did not ask for. Anything else
# (SDL unset, SDL=auto, SDL=anything-else) walks 3 -> 2 -> 1.
SDL_PIN := $(filter 1 2 3,$(strip $(SDL)))

ifeq ($(SDL_PIN),)
    ifeq ($(SDL3_FOUND),yes)
        SDL_VER := 3
    else
        ifeq ($(SDL2_FOUND),yes)
            SDL_VER := 2
        else
            ifeq ($(SDL1_FOUND),yes)
                SDL_VER := 1
            else
                SDL_VER :=
            endif
        endif
    endif
else
    SDL_VER := $(SDL_PIN)
endif

# A pinned version that is not installed is an error, not a surprise.
ifneq ($(SDL_VER),)
    ifeq ($(SDL_VER),3)
        ifneq ($(SDL3_FOUND),yes)
            $(error SDL=3 was requested but SDL3 was not found (install libsdl3-dev or mingw-w64-*-sdl3))
        endif
    endif
    ifeq ($(SDL_VER),2)
        ifneq ($(SDL2_FOUND),yes)
            $(error SDL=2 was requested but SDL2 was not found (install libsdl2-dev or mingw-w64-*-SDL2))
        endif
    endif
    ifeq ($(SDL_VER),1)
        ifneq ($(SDL1_FOUND),yes)
            $(error SDL=1 was requested but SDL 1.2 was not found (install libsdl1.2-dev or mingw-w64-*-SDL))
        endif
    endif
endif

# Nothing at all: explain the situation and stop.
ifeq ($(SDL_VER),)
    $(info )
    $(info Minicraft builds against SDL 1.2, SDL2 or SDL3, and none of them is installed.)
    $(info )
    $(info   SDL3 found : $(if $(SDL3_FOUND),yes,no))
    $(info   SDL2 found : $(if $(SDL2_FOUND),yes,no))
    $(info   SDL1 found : $(if $(SDL1_FOUND),yes,no))
    $(info )
    $(info Debian/Ubuntu : sudo apt-get install libsdl3-dev)
    $(info MSYS2/MinGW   : pacman -S $$MINGW_PACKAGE_PREFIX-sdl3)
    $(info )
    $(error no usable SDL found - install SDL3 (preferred), SDL2 or SDL 1.2 and try again)
endif


# =============================================================================
# 05. Platform settings
# =============================================================================
# $(OS) is make's built-in: "Windows_NT" on Windows, the uname string
# (Linux, Darwin, ...) everywhere else, so the Unix block covers Linux,
# macOS and the BSDs with no extra detection.
ifeq ($(OS),Windows_NT)
    # ===================== Windows (MinGW) =====================
    OUTPUT = game.exe

    # Self-contained executable: the C runtime and SDL link in statically,
    # so the .exe runs on a machine with nothing but Windows installed.
    CFLAGS += \
        -static \
        -static-libgcc

    # Windows system libraries every SDL backend needs.
    WIN_SYS_LIBS := \
        -lkernel32 -luser32 -lgdi32 -lwinmm -limm32 \
        -lole32 -loleaut32 -lversion -luuid -ladvapi32 \
        -lsetupapi -lshell32

    ifeq ($(SDL_VER),1)
        # SDL 1.2 (SDL1) on Windows (MSYS2 MINGW32)
        # Package: mingw-w64-i686-SDL
        #
        # The static libSDL.a was built with DirectX 5/6/7 support.
        # It references many COM GUIDs that are defined in:
        #   - dxguid  (IID_*, GUID_*)
        #   - ddraw   (some DirectDraw symbols)
        #
        # You MUST add -ldxguid -lddraw right after -lSDL.
        CFLAGS += -DUSE_SDL1
        LDFLAGS += \
            -lmingw32 -lSDLmain -lSDL -ldxguid -lddraw -ldinput8 -lm \
            $(WIN_SYS_LIBS)
    else ifeq ($(SDL_VER),2)
        # SDL2
        CFLAGS += -DUSE_SDL2
        LDFLAGS += \
            -lmingw32 -lSDL2main -lSDL2 -lm -ldinput8 \
            $(WIN_SYS_LIBS)
    else
        # SDL3. There is no SDL3main: SDL 3 dropped SDLmain altogether, so a
        # plain main() links as usual.
        #
        # SDL3_LIBS_STATIC rather than the plain libs variable: -static makes
        # this a static link, so SDL3's own dependencies (libiconv) have to
        # be named here, after -lSDL3. See the definition above.
        CFLAGS += -DUSE_SDL3
        LDFLAGS += \
            -lmingw32 \
            $(SDL3_LIBS_STATIC) \
            -lm -ldinput8 \
            $(WIN_SYS_LIBS)
    endif

else
    # ===================== Unix (Linux, macOS, BSDs) =====================
    OUTPUT = game

    ifeq ($(SDL_VER),1)
        # SDL 1.2
        CFLAGS += -DUSE_SDL1 $(SDL1_CFLAGS)
        LDFLAGS += $(SDL1_LIBS) -lm
    else ifeq ($(SDL_VER),2)
        # SDL2
        CFLAGS += -DUSE_SDL2 $(SDL2_CFLAGS)
        LDFLAGS += $(SDL2_LIBS) -lm
    else
        # SDL3
        CFLAGS += -DUSE_SDL3 $(SDL3_CFLAGS)
        LDFLAGS += $(SDL3_LIBS) -lm
    endif

    # Defensive: SDL_VER is guaranteed to be 1/2/3 by section 04, so this
    # can only fire if the platform block above is edited wrongly. A -DUSE_SDL
    # define is mandatory in the sources; this keeps the build loud instead
    # of silently compiling against nothing.
    ifeq ($(filter -DUSE_SDL%,$(CFLAGS)),)
        CFLAGS += -DUSE_SDL3
    endif

    # Framebuffer hint (Linux only; the KMSDRM / fbcon preference is gated
    # behind it, see source/sdlcompat.h).
    ifeq ($(FB),1)
        CFLAGS += -DUSE_FB
    endif
endif

# Embedded targets without sound.
ifeq ($(NO_AUDIO),1)
    CFLAGS += -DNO_AUDIO
endif


# =============================================================================
# 06. Build flags
# =============================================================================
# Optimization and base warnings, shared by every platform. DEBUG=1 gives
# the unoptimized build that "vsconfig" uses for F5 debugging.
ifeq ($(DEBUG),1)
    CFLAGS += \
        -Wall \
        -Wextra \
        -g \
        -O0
else
    CFLAGS += \
        -Wall \
        -Wextra \
        -O2
endif

# ---- Warnings ------------------------------------------------------------
# The "level 1" set: -Wall -Wextra plus the flags that cost no work and do
# catch real bugs (undefined macros, VLAs, obvious NULL dereferences, jumps
# that skip an initialization, dubious formats...).
#
# -Wno-alloc-size-larger-than is the one subtraction, and it is a
# subtraction from GCC's OWN defaults, not from the set above: GCC turns
# -Walloc-size-larger-than=PTRDIFF_MAX on by itself. Its value-range
# analysis cannot prove that an int * int is positive even when the two
# operands were validated on the line before, so casting the product to
# size_t looks to it like an astronomical allocation. Every dimension
# product in the tree (level_create, screen_create, prevBuf, the three
# levelgen maps) is that same shape. GCC 15 sharpened the analysis enough
# to start failing the build on it.
#
# The dimensions are checked at the constructors anyway, which is what
# actually stops a bogus allocation at run time; this flag only stops the
# compiler from second-guessing a product it cannot bound.
WARNINGS ?= \
    -Wall -Wextra \
    -Wundef \
    -Wvla \
    -Wnull-dereference \
    -Wjump-misses-init \
    -Wunused-macros \
    -Wformat=2 \
    -Wno-alloc-size-larger-than
CFLAGS += $(WARNINGS)

# The zero-warnings rule: -Werror on by default.
# make WERROR=0 turns it off for exotic toolchains / very old GCC.
ifeq ($(WERROR),1)
    CFLAGS += -Werror
endif

# ---- Logging (source/log.h) -----------------------------------------------
#   LOG=0 silent | 1 errors | 2 +warnings | 3 +info | 4 +trace
#   Release starts at 2 and DEBUG=1 at 4 (everything visible).
ifeq ($(DEBUG),1)
    LOG ?= 4
    CFLAGS += -DDEBUG_BUILD
else
    LOG ?= 2
endif
CFLAGS += -DLOG_LEVEL=$(LOG)


# =============================================================================
# 07. Sources & objects
# =============================================================================
# Every .c / .h under source/ at depth 1-3 is part of the build, so adding
# a directory needs no Makefile change.
SOURCES = $(wildcard \
    $(SOURCE_DIR)/*.c \
    $(SOURCE_DIR)/*/*.c \
    $(SOURCE_DIR)/*/*/*.c \
)
HEADERS = $(wildcard \
    $(SOURCE_DIR)/*.h \
    $(SOURCE_DIR)/*/*.h \
    $(SOURCE_DIR)/*/*/*.h \
)

# Convert all .c files into .o files
OBJECTS = $(SOURCES:.c=.o)


# =============================================================================
# 08. Asset packing (python3, stdlib only)
# =============================================================================
# The generated files are committed to the repo; these rules only run when
# an asset is newer than its C source. On hosts without python3 (CI,
# cross-compilation hosts, embedded toolchains) the committed files are
# simply used, which is what the order-only prerequisite below arranges.
GENERATED_DIR = $(SOURCE_DIR)/extern
GEN_ICONS     = $(GENERATED_DIR)/icons_data.c $(GENERATED_DIR)/icons_data.h
GEN_SOUNDS    = $(GENERATED_DIR)/sound_data.c
GEN_FILES     = $(GEN_ICONS) $(GEN_SOUNDS)

# Is python3 available? (both scripts use only the standard library)
HAVE_PYTHON := $(shell $(PYTHON) --version >/dev/null 2>&1 && echo 1)

ifeq ($(HAVE_PYTHON),1)
# Spritesheet: assets/icons.png -> source/extern/icons_data.c/.h
$(GEN_ICONS): assets/icons.png scripts/spritesheet2c.py
	@mkdir -p $(GENERATED_DIR)
	@$(PYTHON) scripts/spritesheet2c.py assets/icons.png $(GENERATED_DIR)/icons_data icons

# Sounds: assets/*.wav -> source/extern/sound_data.c
$(GEN_SOUNDS): $(wildcard assets/*.wav) scripts/sound2c.py $(SOURCE_DIR)/sound/sound.h
	@mkdir -p $(GENERATED_DIR)
	@$(PYTHON) scripts/sound2c.py $(GENERATED_DIR)/sound_data.c $(wildcard assets/*.wav)
endif

# Object files need the extern sources/headers to EXIST before compiling.
# Order-only prerequisite: timestamps are ignored, only presence matters.
$(OBJECTS): | $(GEN_FILES)


# =============================================================================
# 09. Build rules
# =============================================================================
# Compile each .c file into a .o file
%.o: %.c $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

# Link all object files to create the executable
$(OUTPUT): $(OBJECTS)
	@$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $@


# =============================================================================
# 10. Targets
# =============================================================================
# Default target: build the final executable
all: $(OUTPUT)

# Run the game
run: $(OUTPUT)
	@./$(OUTPUT)

# Clean up build artifacts
clean:
	@rm -fv $(OBJECTS) $(OUTPUT)

# Force re-pack of all assets
assets:
ifeq ($(HAVE_PYTHON),1)
	@mkdir -p $(GENERATED_DIR)
	@$(PYTHON) scripts/spritesheet2c.py assets/icons.png $(GENERATED_DIR)/icons_data icons
	@$(PYTHON) scripts/sound2c.py $(GENERATED_DIR)/sound_data.c assets/*.wav
else
	@echo "ERROR: $(PYTHON) not found - cannot regenerate assets." >&2; \
	echo "       The committed files in $(GENERATED_DIR)/ will be used by the normal build." >&2; \
	exit 1
endif


# =============================================================================
# 11. Developer tooling: VSCode config generation (make vsconfig)
# =============================================================================
# Generates .vscode/c_cpp_properties.json, .vscode/tasks.json and
# .vscode/launch.json tailored to THIS machine:
#   - detects the compiler from CC (absolute path, target triple)
#   - queries the compiler for its system include dirs (gcc -E -Wp,-v)
#   - reflects the active build defines (SDL=, FB=, NO_AUDIO=, DEBUG=)
# Works on Linux, macOS and Windows (MinGW/MSYS2). Requires GNU Make >= 4.0.
#
# Examples:
#   make vsconfig                 config for the default build
#   make vsconfig SDL=1           config for the SDL 1.2 build
#   CC=riscv64-linux-gnu-gcc make vsconfig   cross-toolchain config

VSCODE_DIR = .vscode

# make's own path with backslashes converted for JSON strings (Windows).
# Uses the single-backslash form of the substitution: a doubled backslash
# would DELETE the separators instead of converting them, breaking the
# command on Windows.
VSC_MAKE := $(subst \,/,$(MAKE))

comma := ,
vsc_empty :=
vsc_sp := $(vsc_empty) $(vsc_empty)
# vsc_list, a b c  ->  "a", "b", "c"     (strict JSON list content)
vsc_list = $(subst @,,$(subst @$(vsc_sp),$(comma)$(vsc_sp),$(strip $(foreach i,$(strip $1),"$(subst \,/,$i)"@))))

# --- compiler detection -----------------------------------------------------
# NOTE: everything that shells out below is guarded by MAKECMDGOALS on
# purpose. These assignments run at parse time, so an unguarded $(shell)
# would execute on every make invocation (`make clean` included).
# The redirects also depend on which shell make uses for $(shell): POSIX
# sh syntax would create a literal file named "nul" under cmd.exe, and cmd
# syntax would create one under sh - so pick per shell. (make picks sh.exe
# when it is in PATH, cmd.exe otherwise.)
ifneq ($(filter sh sh.exe bash bash.exe dash,$(notdir $(SHELL))),)
    VSC_POSIX_SHELL := 1
else
    VSC_POSIX_SHELL :=
endif

ifeq ($(filter vsconfig,$(MAKECMDGOALS)),vsconfig)
    ifeq ($(VSC_POSIX_SHELL),1)
        VSC_CC  := $(shell command -v $(firstword $(CC)) 2>/dev/null)
        VSC_GDB := $(shell command -v gdb 2>/dev/null)
        VSC_MKDIR_ERR := $(shell mkdir -p $(VSCODE_DIR) 2>&1)
    else
        VSC_CC  := $(subst \,/,$(firstword $(shell where $(firstword $(CC)) 2>nul)))
        VSC_GDB := $(subst \,/,$(firstword $(shell where gdb 2>nul)))
        VSC_MKDIR_ERR := $(shell if not exist "$(VSCODE_DIR)" mkdir "$(VSCODE_DIR)" 2>&1)
    endif
    # Target triple, e.g. x86_64-linux-gnu / i686-w64-mingw32 / riscv64-linux-gnu
    VSC_TRIPLE := $(shell $(CC) -dumpmachine)
    # System include dirs reported by the compiler itself. Empty input is
    # piped via stdin so no null-device file argument is needed on any
    # platform (an argument like nul or /dev/null is shell-dependent).
    VSC_SYSINC := $(shell echo | $(CC) -E -Wp,-v -xc - 2>&1 | sed -n 's/^ \{1,\}\(.*\)/\1/p')
endif
ifeq ($(VSC_CC),)
    VSC_CC := $(firstword $(CC))
endif
ifeq ($(VSC_GDB),)
    VSC_GDB := gdb
endif
ifeq ($(OS),Windows_NT)
    VSC_OSNAME := Windows
else
    VSC_OSNAME := Linux
endif
VSC_ARCH := $(firstword $(subst -, ,$(VSC_TRIPLE)))

# --- intelliSenseMode from OS + arch ----------------------------------------
ifeq ($(OS),Windows_NT)
    VSC_IDE_OS := windows
else
    VSC_IDE_OS := linux
endif
ifneq ($(filter i%86 i386,$(VSC_ARCH)),)
    VSC_MODE := $(VSC_IDE_OS)-gcc-x86
else ifneq ($(filter arm armv% armhf,$(VSC_ARCH)),)
    VSC_MODE := $(VSC_IDE_OS)-gcc-arm
else ifneq ($(filter aarch64 arm64,$(VSC_ARCH)),)
    VSC_MODE := $(VSC_IDE_OS)-gcc-arm64
else
    # x86_64, riscv64, mips64, ... -> best-effort 64-bit mode
    VSC_MODE := $(VSC_IDE_OS)-gcc-x64
endif

# --- include dirs & defines of the ACTIVE configuration ---------------------
# Extra -I flags (e.g. sdl-config --cflags with SDL=1):
VSC_SDLINC := $(patsubst -I%,%,$(filter -I%,$(CFLAGS)))
VSC_DEFINES := $(patsubst -D%,%,$(filter -D%,$(CFLAGS)))

VSC_ALLINC := $${workspaceFolder}/source $(VSC_SDLINC) $(VSC_SYSINC)

VSC_CONFNAME := Minicraft-$(VSC_OSNAME)-SDL$(SDL)$(if $(filter 1,$(FB)),-FB)$(if $(filter 1,$(NO_AUDIO)),-NOAUDIO)$(if $(filter 1,$(DEBUG)),-DEBUG)

# VSCODE_DIR is created at parse time by the guarded detection block
# above (GNU Make expands a whole recipe before executing its first line,
# so the file functions below could never rely on a mkdir recipe line).

# --- JSON payloads ------------------------------------------------------------
define VSCODE_CPROPS
{
    "version": 4,
    "configurations": [
        {
            "name": "$(VSC_CONFNAME)",
            "compilerPath": "$(VSC_CC)",
            "compilerArgs": [],
            "intelliSenseMode": "$(VSC_MODE)",
            "cStandard": "gnu11",
            "includePath": [ $(call vsc_list,$(VSC_ALLINC)) ],
            "defines": [ $(call vsc_list,$(VSC_DEFINES)) ],
            "browse": {
                "path": [ "$${workspaceFolder}/source" ],
                "limitSymbolsToIncludedHeaders": true
            }
        }
    ]
}
endef

define VSCODE_TASKS
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [],
            "group": { "kind": "build", "isDefault": true },
            "problemMatcher": [ "$$gcc" ],
            "detail": "Default build (SDL$(SDL_VER))"
        },
        {
            "label": "build-debug",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [ "DEBUG=1" ],
            "group": "build",
            "problemMatcher": [ "$$gcc" ],
            "detail": "Debug build (-g -O0) - used by F5"
        },
        {
            "label": "build-sdl3",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [ "SDL=3" ],
            "group": "build",
            "detail": "SDL 3 build (pinned)"
        },
        {
            "label": "build-sdl2",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [ "SDL=2" ],
            "group": "build",
            "detail": "SDL 2 build (pinned)"
        },
        {
            "label": "build-sdl1",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [ "SDL=1" ],
            "group": "build",
            "problemMatcher": [ "$$gcc" ],
            "detail": "SDL 1.2 build"
        },
        {
            "label": "build-fb",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [ "FB=1" ],
            "group": "build",
            "problemMatcher": [ "$$gcc" ],
            "detail": "Framebuffer build (KMSDRM/fbcon, Linux)"
        },
        {
            "label": "build-no-audio",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [ "NO_AUDIO=1" ],
            "group": "build",
            "problemMatcher": [ "$$gcc" ],
            "detail": "Silent build (embedded, no audio)"
        },
        {
            "label": "run",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [ "run" ],
            "dependsOn": [ "build" ],
            "problemMatcher": [],
            "detail": "Build and run the game"
        },
        {
            "label": "clean",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [ "clean" ],
            "problemMatcher": [],
            "detail": "Remove objects and binary"
        },
        {
            "label": "assets",
            "type": "shell",
            "command": "$(VSC_MAKE)",
            "args": [ "assets" ],
            "problemMatcher": [],
            "detail": "Re-pack PNG/WAV assets into C arrays (needs python3)"
        }
    ]
}
endef

define VSCODE_LAUNCH
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Minicraft (gdb)",
            "type": "cppdbg",
            "request": "launch",
            "program": "$${workspaceFolder}/$(OUTPUT)",
            "args": [],
            "stopAtEntry": false,
            "cwd": "$${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "$(VSC_GDB)",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build-debug"
        }
    ]
}
endef

define VSCODE_SETTINGS
{
    /*
     * Workspace settings written by `make vsconfig`.
     * Regenerated on every run: edit the Makefile, not this file.
     */
    "C_Cpp.default.compilerPath": "$(VSC_CC)",
    "C_Cpp.default.cStandard": "gnu11",
    "C_Cpp.default.intelliSenseMode": "$(VSC_MODE)",

    /*
     * Squiggles stay ON. The one false positive this workspace used to
     * show - "duplicate association type (\"int\") in _Generic selection"
     * on the game_set_menu() overload in game.h - comes from the
     * extension's front-end treating the menu_id enum and int as the same
     * type, something neither GCC nor Clang does with -Wall -Wextra.
     *
     * It is fixed at the source: game.h hides the redundant `int`
     * association behind #ifdef __INTELLISENSE__, so the front-end sees a
     * selection with no duplicate types while every real compiler keeps
     * the branch. Nothing is being masked here, so there is no reason to
     * reach for "C_Cpp.errorSquiggles": "Disabled", which would throw
     * away every genuine diagnostic along with it.
     */
    "C_Cpp.errorSquiggles": "enabledIfIncludesResolve",
    "C_Cpp.intelliSenseEngine": "default",

    /* Headers here are C, not C++. */
    "files.associations": {
        "*.h": "c"
    },
    "C_Cpp.autoAddFileAssociations": false
}
endef

vsconfig:
ifneq ($(firstword $(sort 4.0 $(MAKE_VERSION))),4.0)
	@echo "ERROR: 'make vsconfig' requires GNU Make >= 4.0 (found $(MAKE_VERSION))." && exit 1
else
ifneq ($(VSC_MKDIR_ERR),)
	@echo "vsconfig: ERROR: cannot create $(VSCODE_DIR)/: $(VSC_MKDIR_ERR)" && exit 1
endif
	@$(file > $(VSCODE_DIR)/c_cpp_properties.json,$(VSCODE_CPROPS))$(file > $(VSCODE_DIR)/tasks.json,$(VSCODE_TASKS))$(file > $(VSCODE_DIR)/launch.json,$(VSCODE_LAUNCH))$(file > $(VSCODE_DIR)/settings.json,$(VSCODE_SETTINGS))echo "vsconfig: generated $(VSCODE_DIR)/c_cpp_properties.json, tasks.json, launch.json, settings.json"
	@echo "vsconfig: compiler         = $(VSC_CC)  [$(VSC_TRIPLE)]"
	@echo "vsconfig: intelliSenseMode = $(VSC_MODE)"
	@echo "vsconfig: defines          = $(VSC_DEFINES)"
	@echo "vsconfig: gdb              = $(VSC_GDB)"
endif
