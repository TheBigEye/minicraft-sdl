CC = gcc
LD = ld

CFLAGS = -static-libgcc -m32 -Wall -Wextra -Wl,-Bstatic -Wl,--whole-archive -O2
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -limm32 -lgdi32 -lwinmm -lole32 -lcomdlg32 -lopengl32 -lcfgmgr32

SOURCE_DIR = source

# C Source files
SOURCES = $(wildcard                           \
	$(SOURCE_DIR)/crafting/*.c             \
	$(SOURCE_DIR)/entity/*.c               \
	$(SOURCE_DIR)/entity/particle/*.c      \
	$(SOURCE_DIR)/gfx/*.c                  \
	$(SOURCE_DIR)/item/*.c                 \
	$(SOURCE_DIR)/item/resource/*.c        \
	$(SOURCE_DIR)/level/*.c                \
	$(SOURCE_DIR)/level/levelgen/*.c       \
	$(SOURCE_DIR)/level/tile/*.c           \
	$(SOURCE_DIR)/screen/*.c               \
	$(SOURCE_DIR)/utils/*.c                \
	$(SOURCE_DIR)/*.c                      \
)

# C Headers files
HEADERS = $(wildcard                       \
	$(SOURCE_DIR)/crafting/*.h             \
	$(SOURCE_DIR)/entity/*.h               \
	$(SOURCE_DIR)/entity/particle/*.h      \
	$(SOURCE_DIR)/gfx/*.h                  \
	$(SOURCE_DIR)/item/*.h                 \
	$(SOURCE_DIR)/item/resource/*.h        \
	$(SOURCE_DIR)/level/*.h                \
	$(SOURCE_DIR)/level/levelgen/*.h       \
	$(SOURCE_DIR)/level/tile/*.h           \
	$(SOURCE_DIR)/screen/*.h               \
	$(SOURCE_DIR)/utils/*.h                \
	$(SOURCE_DIR)/*.h                      \
)

# TODO: add support for Linux (ELF instead of EXE LOL)

OBJECTS = $(SOURCES:.c=.o)                 \

all: game.exe

game.exe: ${OBJECTS}
	@$(CC) ${OBJECTS} $(LDFLAGS) -o $@


%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

run: game.exe
	@./game.exe

clean:
	@rm -fv $(OBJECTS) game.exe
