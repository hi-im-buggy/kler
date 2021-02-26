CC = gcc
CLIBS = -lncurses
CFLAGS = -Wall
BINARY = kler

build: src/*.c
	$(CC) $(CFLAGS) $? -o bin/$(BINARY) $(CLIBS)

debug: src/*.c
	$(CC) $(CFLAGS) -g $? -o bin/$(BINARY) $(CLIBS)

install: build
	cp bin/$(BINARY) ~/.local/bin

run: build
	bin/$(BINARY)

clean:
	rm bin/*
