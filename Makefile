CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -O2
TARGET = monopoly
SRC_DIR = src

all: $(TARGET)

$(TARGET): $(SRC_DIR)/sim.c $(SRC_DIR)/sim_header.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_DIR)/sim.c

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

.PHONY: all clean run valgrind
