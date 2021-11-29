CC := gcc
C_FLAGS := -ggdb3 -Wextra -Wall -Wpedantic -Werror -iquoteinclude
SRC := $(wildcard src/*.c)
DEBUG_SRC := $(filter-out src/test.c,$(SRC))
TEST_SRC := $(filter-out src/main.c,$(SRC))
BIN_DIR := bin/
EXEC := coconut
TEST_EXEC := testsh

all: coconut


coconut: $(DEBUG_SRC)
	$(CC) $^  $(C_FLAGS) -o $(BIN_DIR)$(EXEC)

tests: $(TEST_SRC)
	$(CC) $^ $(C_FLAGS) -o $(BIN_DIR)$(TEST_EXEC)

clean: 
	rm $(BIN_DIR)*
