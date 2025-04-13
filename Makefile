# Compiler
CC = gcc
LD = ld

# Directories
SOURCE_DIR = source

# -DLEVELGENTEST
# -DTEST_SHOWPORTALPOS
# -DTEST_INVENTORY
# -DGODMODE

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    # Windows
    OUTPUT  = game.exe
    CFLAGS += -m32 -Wall -Wextra -O2 -static -static-libgcc -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lm -lkernel32 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid -ladvapi32 -lsetupapi -lshell32 -ldinput8
else
    # Linux
    OUTPUT  = game
    CFLAGS += -m32 -Wall -Wextra -O2 -lSDL2 -lSDL2_ttf -lm
endif


# C Source files
SOURCES = $(wildcard 				  \
	$(SOURCE_DIR)/crafting/*.c 		  \
	$(SOURCE_DIR)/entity/*.c 		  \
	$(SOURCE_DIR)/entity/particle/*.c \
	$(SOURCE_DIR)/gfx/*.c 			  \
	$(SOURCE_DIR)/item/*.c 			  \
	$(SOURCE_DIR)/item/resource/*.c   \
	$(SOURCE_DIR)/level/*.c 		  \
	$(SOURCE_DIR)/level/levelgen/*.c  \
	$(SOURCE_DIR)/level/tile/*.c      \
	$(SOURCE_DIR)/screen/*.c          \
	$(SOURCE_DIR)/utils/*.c           \
	$(SOURCE_DIR)/*.c                 \
)

# C Header files
HEADERS = $(wildcard                  \
	$(SOURCE_DIR)/crafting/*.h        \
	$(SOURCE_DIR)/entity/*.h          \
	$(SOURCE_DIR)/entity/particle/*.h \
	$(SOURCE_DIR)/gfx/*.h             \
	$(SOURCE_DIR)/item/*.h            \
	$(SOURCE_DIR)/item/resource/*.h   \
	$(SOURCE_DIR)/level/*.h           \
	$(SOURCE_DIR)/level/levelgen/*.h \
	$(SOURCE_DIR)/level/tile/*.h \
	$(SOURCE_DIR)/screen/*.h \
	$(SOURCE_DIR)/utils/*.h \
	$(SOURCE_DIR)/*.h \
)

# Convert all .c files into .o files
OBJECTS = $(SOURCES:.c=.o)


# Default target: build the final executable
all: $(OUTPUT)


# Link all object files to create the executable
$(OUTPUT): $(OBJECTS)
	@$(CC) $(OBJECTS) $(CFLAGS) -o $@


# Compile each .c file into a .o file
%.o: %.c $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@


# Run the game
run: $(OUTPUT)
	@./$(OUTPUT)


# Clean up build artifacts
clean:
	@rm -fv $(OBJECTS) $(OUTPUT)
