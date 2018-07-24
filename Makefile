CC=gcc

OPTFLAGS = -O2
CFLAGS = $(OPTFLAGS) -std=c99 -Wall -Wextra -pedantic
CPPFLAGS = -MD -MP -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lxcb -lxcb-ewmh -lxcb-icccm -lxcb-xrm -lxcb-util -lpthread

TARGET = custard
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
PREFIX?=/usr/local
MANPREFIX?=$(PREFIX)/share/man

.PHONY: all install clean

all: $(TARGET)

-include $(SRCS:.c=.d)

$(TARGET): $(OBJS)
	mkdir -p build
	$(CC) $(LDFLAGS) -o $@ $^

install:
	install -m 755 -D build/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -m 755 -D contrib/custardctl.py $(DESTDIR)$(PREFIX)/bin/custardctl
#	install -m 644 -D man/custard.man $(DESTDIR)$(MANPREFIX)/man1/custard.1

clean:
	$(RM) build/$(TARGET)
	$(RM) src/*.o
	$(RM) src/*.d

