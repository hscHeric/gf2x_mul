CC := gcc
CLANG_FORMAT := clang-format
TARGET := gf2x_mul
SRC_DIR := src
SRC := $(SRC_DIR)/main.c
HEADERS := $(SRC_DIR)/gf2x_mul.h
ASSEMBLY := $(TARGET).s

CFLAGS := -std=c11 -O3 -march=native -flto -Wall -Wextra -Wpedantic

.PHONY: all run debug assembly format format-check clean

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(RM) $(TARGET)-debug $(ASSEMBLY)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

debug:
	$(CC) -std=c11 -O0 -g3 -march=native -Wall -Wextra -Wpedantic $(SRC) -o $(TARGET)-debug

assembly: $(SRC) $(HEADERS)
	$(CC) -std=c11 -O3 -march=native -S -masm=intel $(SRC) -o $(ASSEMBLY)

format:
	$(CLANG_FORMAT) -i $(SRC) $(HEADERS)

format-check:
	$(CLANG_FORMAT) --dry-run --Werror $(SRC) $(HEADERS)

clean:
	$(RM) $(TARGET) $(TARGET)-debug $(ASSEMBLY)
