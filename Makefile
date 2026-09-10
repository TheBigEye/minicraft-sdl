### Compiler
# (override for cross-compilation, e.g. CC=riscv64-linux-gnu-gcc)
CC ?= gcc
LD ?= ld

# Directories
SOURCE_DIR = source

# Build options:
#   make                   SDL2 (default)
#   make SDL=1             SDL 1.2 (SDL1)
#   make SDL=1 FB=1        SDL1 + prefer framebuffer (fbcon)   [Linux only]
#   make FB=1              SDL2 + prefer framebuffer (KMSDRM)   [Linux only]
#   make NO_AUDIO=1        compile without audio (silent build, for
#                          embedded targets without sound support)
#   make DEBUG=1           debug build (-g -O0), used by the VSCode launcher
#   make assets            force re-pack of assets (PNG/WAV -> C arrays,
#                          needs python3; stdlib only, no pip packages)
#   make vsconfig          generate .vscode/ config (c_cpp_properties, tasks,
#                          launch) auto-detecting this machine's GCC toolchain
#                          (needs GNU Make >= 4.0; honors SDL=/FB=/NO_AUDIO=)
#
# Windows (MinGW) examples:
#   make                   SDL2
#   make SDL=1             SDL 1.2
#
# Cross example (Linux):
#   CC=riscv64-linux-gnu-gcc make SDL=1 FB=1
#
# Asset packing:
#   scripts/spritesheet2c.py converts assets/icons.png into
#   source/extern/icons_data.c/.h (same formula as the original Java
#   SpriteSheet: blue_channel / 64).
#   scripts/sound2c.py converts assets/*.wav into source/extern/sound_data.c
#   (mono/16-bit/44100 Hz arrays, played by the mixer in source/sound/sound.c).
#   The generated files are committed to the repo, so python3 is only needed
#   when the assets change (fallback for cross-compilation environments).

SDL ?= 2
FB ?= 0
NO_AUDIO ?= 0
PYTHON ?= python3

# -DLEVELGENTEST
# -DTEST_SHOWPORTALPOS
# -DTEST_INVENTORY
# -DGODMODE

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    # ===================== Windows (MinGW) =====================
    OUTPUT  = game.exe
    CFLAGS += -Wall -Wextra -O2 -static -static-libgcc

    ifeq ($(SDL),1)
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
        LDFLAGS += -lmingw32 -lSDLmain -lSDL -ldxguid -lddraw -ldinput8 -lm \
                   -lkernel32 -luser32 -lgdi32 -lwinmm -limm32 \
                   -lole32 -loleaut32 -lversion -luuid -ladvapi32 \
                   -lsetupapi -lshell32
    else
        # SDL2 (default)
        CFLAGS += -DUSE_SDL2
        LDFLAGS += -lmingw32 -lSDL2main -lSDL2 -lm \
                   -lkernel32 -luser32 -lgdi32 -lwinmm -limm32 \
                   -lole32 -loleaut32 -lversion -luuid -ladvapi32 \
                   -lsetupapi -lshell32 -ldinput8
    endif

else
    # ===================== Linux / Unix =====================
    OUTPUT  = game

    ifeq ($(SDL),1)
        # SDL 1.2
        SDL_CFLAGS := $(shell sdl-config --cflags 2>/dev/null || echo "")
        SDL_LIBS   := $(shell sdl-config --libs 2>/dev/null || echo "-lSDL")
        CFLAGS += -DUSE_SDL1 $(SDL_CFLAGS)
        LDFLAGS += $(SDL_LIBS) -lm
    else
        # SDL 2 (default)
        CFLAGS += -DUSE_SDL2
        LDFLAGS += -lSDL2 -lm
    endif

    # Ensure one of the defines is always present (Linux)
    ifneq ($(filter -DUSE_SDL%,$(CFLAGS)),)
    else
        CFLAGS += -DUSE_SDL2
    endif

    # Framebuffer hints (Linux only)
    ifeq ($(FB),1)
        CFLAGS += -DUSE_FB
    endif
endif

# Audio (embedded targets without sound can build with NO_AUDIO=1)
ifeq ($(NO_AUDIO),1)
    CFLAGS += -DNO_AUDIO
endif

# Common CFLAGS
DEBUG ?= 0
ifeq ($(DEBUG),1)
    # Debug build (used by "make vsconfig" launch.json / F5 debugging)
    CFLAGS += -Wall -Wextra -g -O0
else
    CFLAGS += -Wall -Wextra -O2
endif

# C Source files
SOURCES = $(wildcard 				  \
	$(SOURCE_DIR)/crafting/*.c 	      \
	$(SOURCE_DIR)/entity/*.c 	      \
	$(SOURCE_DIR)/entity/particle/*.c \
	$(SOURCE_DIR)/extern/*.c          \
	$(SOURCE_DIR)/gfx/*.c 			  \
	$(SOURCE_DIR)/item/*.c 			  \
	$(SOURCE_DIR)/item/resource/*.c   \
	$(SOURCE_DIR)/level/*.c 		  \
	$(SOURCE_DIR)/level/levelgen/*.c  \
	$(SOURCE_DIR)/level/tile/*.c      \
	$(SOURCE_DIR)/screen/*.c          \
	$(SOURCE_DIR)/sound/*.c           \
	$(SOURCE_DIR)/utils/*.c           \
	$(SOURCE_DIR)/*.c                 \
)

# C Header files
HEADERS = $(wildcard                  \
	$(SOURCE_DIR)/crafting/*.h        \
	$(SOURCE_DIR)/entity/*.h          \
	$(SOURCE_DIR)/entity/particle/*.h \
	$(SOURCE_DIR)/extern/*.h          \
	$(SOURCE_DIR)/gfx/*.h             \
	$(SOURCE_DIR)/item/*.h            \
	$(SOURCE_DIR)/item/resource/*.h   \
	$(SOURCE_DIR)/level/*.h           \
	$(SOURCE_DIR)/level/levelgen/*.h  \
	$(SOURCE_DIR)/level/tile/*.h      \
	$(SOURCE_DIR)/screen/*.h          \
	$(SOURCE_DIR)/sound/*.h           \
	$(SOURCE_DIR)/utils/*.h           \
	$(SOURCE_DIR)/*.h                 \
)

# Convert all .c files into .o files
OBJECTS = $(SOURCES:.c=.o)


# ===================== Asset packing (Python 3, stdlib only) =====================
# (the rules below appear before "all:", so make sure "all" stays the default)
.DEFAULT_GOAL := all

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
# Order-only prerequisite: they are committed to the repo, so when python3 is
# not available (cross-compilation hosts, CI, embedded toolchains) the build
# simply uses the committed files.
$(OBJECTS): | $(GEN_FILES)

# Force re-pack of all assets
.PHONY: assets
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


# ===================== VSCode config generation (make vsconfig) =====================
# Generates .vscode/c_cpp_properties.json, .vscode/tasks.json and
# .vscode/launch.json tailored to THIS machine:
#   - detects the compiler from $(CC) (absolute path, target triple)
#   - queries the compiler for its system include dirs (gcc -E -Wp,-v)
#   - reflects the active build defines (SDL=, FB=, NO_AUDIO=, DEBUG=)
# Works on Linux and Windows (MinGW/MSYS2). Requires GNU Make >= 4.0.
#
# Examples:
#   make vsconfig                 config for the default build (SDL2)
#   make vsconfig SDL=1           config for the SDL 1.2 build
#   CC=riscv64-linux-gnu-gcc make vsconfig   cross-toolchain config

VSCODE_DIR = .vscode

comma := ,
vsc_empty :=
vsc_sp := $(vsc_empty) $(vsc_empty)
# $(call vsc_list,a b c)  ->  "a", "b", "c"     (strict JSON list content)
vsc_list = $(subst @,,$(subst @$(vsc_sp),$(comma)$(vsc_sp),$(strip $(foreach i,$(strip $1),"$(subst \,/,$i)"@))))

# --- compiler detection -----------------------------------------------------
# NOTE: everything that shells out below is guarded by MAKECMDGOALS on
# purpose. These assignments run at parse time, so an unguarded $(shell)
# would execute on every make invocation (`make clean` included).
# The redirects also depend on which shell make uses for $(shell): POSIX
# sh syntax (`2>/dev/null`) would create a literal file named "nul" under
# cmd.exe, and cmd syntax (`2>nul`) would create one under sh - so pick
# per shell. (make picks sh.exe when it is in PATH, cmd.exe otherwise.)
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
    # platform (an argument like `nul`/`/dev/null` is shell-dependent).
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

# $(VSCODE_DIR) is created at parse time by the guarded detection block
# above (GNU Make expands a whole recipe before executing its first line,
# so $(file > ...) could never rely on a mkdir recipe line).

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
            "command": "$(subst \,/,$(MAKE))",
            "args": [],
            "group": { "kind": "build", "isDefault": true },
            "problemMatcher": [ "$$gcc" ],
            "detail": "Default build (SDL$(SDL))"
        },
        {
            "label": "build-debug",
            "type": "shell",
            "command": "$(subst \,/,$(MAKE))",
            "args": [ "DEBUG=1" ],
            "group": "build",
            "problemMatcher": [ "$$gcc" ],
            "detail": "Debug build (-g -O0) - used by F5"
        },
        {
            "label": "build-sdl1",
            "type": "shell",
            "command": "$(subst \,/,$(MAKE))",
            "args": [ "SDL=1" ],
            "group": "build",
            "problemMatcher": [ "$$gcc" ],
            "detail": "SDL 1.2 build"
        },
        {
            "label": "build-fb",
            "type": "shell",
            "command": "$(subst \,/,$(MAKE))",
            "args": [ "FB=1" ],
            "group": "build",
            "problemMatcher": [ "$$gcc" ],
            "detail": "Framebuffer build (KMSDRM/fbcon, Linux)"
        },
        {
            "label": "build-no-audio",
            "type": "shell",
            "command": "$(subst \,/,$(MAKE))",
            "args": [ "NO_AUDIO=1" ],
            "group": "build",
            "problemMatcher": [ "$$gcc" ],
            "detail": "Silent build (embedded, no audio)"
        },
        {
            "label": "run",
            "type": "shell",
            "command": "$(subst \,/,$(MAKE))",
            "args": [ "run" ],
            "dependsOn": [ "build" ],
            "problemMatcher": [],
            "detail": "Build and run the game"
        },
        {
            "label": "clean",
            "type": "shell",
            "command": "$(subst \,/,$(MAKE))",
            "args": [ "clean" ],
            "problemMatcher": [],
            "detail": "Remove objects and binary"
        },
        {
            "label": "assets",
            "type": "shell",
            "command": "$(subst \,/,$(MAKE))",
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

.PHONY: vsconfig
vsconfig:
ifneq ($(firstword $(sort 4.0 $(MAKE_VERSION))),4.0)
	@echo "ERROR: 'make vsconfig' requires GNU Make >= 4.0 (found $(MAKE_VERSION))." && exit 1
else
ifneq ($(VSC_MKDIR_ERR),)
	@echo "vsconfig: ERROR: cannot create $(VSCODE_DIR)/: $(VSC_MKDIR_ERR)" && exit 1
endif
	@$(file > $(VSCODE_DIR)/c_cpp_properties.json,$(VSCODE_CPROPS))$(file > $(VSCODE_DIR)/tasks.json,$(VSCODE_TASKS))$(file > $(VSCODE_DIR)/launch.json,$(VSCODE_LAUNCH))echo "vsconfig: generated $(VSCODE_DIR)/c_cpp_properties.json, tasks.json, launch.json"
	@echo "vsconfig: compiler         = $(VSC_CC)  [$(VSC_TRIPLE)]"
	@echo "vsconfig: intelliSenseMode = $(VSC_MODE)"
	@echo "vsconfig: defines          = $(VSC_DEFINES)"
	@echo "vsconfig: gdb              = $(VSC_GDB)"
endif


# Default target: build the final executable
all: $(OUTPUT)


# Link all object files to create the executable
$(OUTPUT): $(OBJECTS)
	@$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $@


# Compile each .c file into a .o file
%.o: %.c $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@


# Run the game
run: $(OUTPUT)
	@./$(OUTPUT)


# Clean up build artifacts
clean:
	@rm -fv $(OBJECTS) $(OUTPUT)
