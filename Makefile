CC = gcc
RM = rm -rf

CFLAGS = -Wall -Wextra -Wno-unknown-pragmas -pedantic -O3 -g3 -ggdb -Ilib/

SRC = $(shell find src/ -type f -name '*.c')
SRC_OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
TARGET = binclock

all: $(TARGET)

obj/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(SRC_OBJ) Makefile
	$(CC) $(XFLAGS) -o $@ $(TST_OBJ) $(SRC_OBJ)

clean:
	$(RM) $(SRC_OBJ) $(TST_OBJ) $(PROGRAM)

.PHONY: clean
