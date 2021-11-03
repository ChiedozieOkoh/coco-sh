CC := gcc
C_FLAGS := -g -Wextra -Wall -Wpedantic -Werror -iquoteinclude
SRC := $(wildcard src/*.c)
BIN_DIR := bin/
EXEC := coconut

all: coconut


coconut: $(SRC)
	$(CC) $^  $(C_FLAGS) -o $(BIN_DIR)$(EXEC)

clean: 
	rm $(BIN_DIR)*
