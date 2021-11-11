CC = gcc
CFLAGS = -Wall -Wextra -Wno-unknown-pragmas -pedantic -O3 -g3 -ggdb -Ilib/

SRC = $(shell find src/ -type f -name '*.c')
HEADERS = $(shell find lib/ -type f -name '*.h')
SRC_OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
TARGET = binclock
SERVICE_TARGET = $(TARGET).service

all: $(TARGET)

test:
	@echo "testing is unavailable in gcc."

%.1: %.man
	groff -Tascii -man $< > $@

docs: $(TARGET).1

obj/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(SRC_OBJ) $(HEADERS) Makefile
	$(CC) $(XFLAGS) -o $@ $(TST_OBJ) $(SRC_OBJ)

clean:
	rm -rf $(SRC_OBJ) $(TST_OBJ) $(PROGRAM)

install: $(TARGET) $(SERVICE_TARGET) $(TARGET).1
	sudo cp -f $(TARGET) /usr/bin/$(TARGET)
	sudo cp -f $(SERVICE_TARGET) /etc/systemd/system/$(SERVICE_TARGET)
	sudo cp -f $(TARGET).1 /usr/share/man/man1/$(TARGET).1
	sudo mandb

uninstall:
	sudo rm -f $(INSTALL_TARGET)
	sudo rm -f $(INSTALL_SERVICE_TARGET)

.PHONY: clean
