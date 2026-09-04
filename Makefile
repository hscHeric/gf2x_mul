CC := gcc
CLANG_FORMAT := clang-format
TARGET := karatsuba
SRC := main.c 
HEADERS := karatsuba.h

CFLAGS := -std=c11 -O3 -march=native -flto -Wall -Wextra -Wpedantic

.PHONY: all run debug assembly format format-check clean

all:
	$(RM) $(TARGET)-debug karatsuba.s
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

debug:
	$(CC) -std=c11 -O0 -g3 -march=native -Wall -Wextra -Wpedantic $(SRC) -o $(TARGET)-debug

assembly:
	$(CC) -std=c11 -O3 -march=native -S -masm=intel karatsuba.c -o karatsuba.s

format:
	$(CLANG_FORMAT) -i $(SRC) $(HEADERS)

format-check:
	$(CLANG_FORMAT) --dry-run --Werror $(SRC) $(HEADERS)

clean:
	$(RM) $(TARGET) $(TARGET)-debug karatsuba.s
