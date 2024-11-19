# Compiler and flags
CC = gcc
CFLAGS = -I./box2d/include -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
LDFLAGS = -L./box2d/build/src -lbox2d -L/opt/homebrew/lib -lSDL2

# Target executable
TARGET = simulation

# Define the source folder
SRC_FOLDER = src

# Source files
SRC = main.c $(wildcard $(SRC_FOLDER)/*.c)

# Object files
OBJ = $(SRC:%.c=$(BUILD_FOLDER)/%.o)

# Header files
HEADERS = $(wildcard $(SRC_FOLDER)/*.h)

# Build target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)

# Rebuild target
rebuild: clean $(TARGET)

# Run the simulation
run: $(TARGET)
	./$(TARGET)

# Rebuild and run
rerun: rebuild run
