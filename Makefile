CC=gcc

OPTFLAGS = -O2
CFLAGS = $(OPTFLAGS) -std=c99 -Wall -Wextra -pedantic
CPPFLAGS = -MD -MP -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lxcb -lxcb-ewmh -lxcb-icccm -lxcb-util -lpthread

TARGET = custard
SRCS = $(wildcard src/*.c)
OBJS = $(subst src,build,$(SRCS:.c=.o))
PREFIX?=/usr/local
MANPREFIX?=$(PREFIX)/share/man

.PHONY: all install clean

all: prepare $(OBJS) $(TARGET)

-include $(SRCS:.c=.d)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o build/$@ $^

prepare:
	mkdir -p build

build/%.o: src/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

install:
	install -m 755 -D build/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -m 755 -D contrib/custardctl.py $(DESTDIR)$(PREFIX)/bin/custardctl
#	install -m 644 -D man/custard.man $(DESTDIR)$(MANPREFIX)/man1/custard.1

clean:
	$(RM) -r build

