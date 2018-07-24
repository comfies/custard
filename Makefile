CC=gcc

OPTFLAGS = -O3
CFLAGS = $(OPTFLAGS) -std=c99 -pedantic -lxcb -lxcb-ewmh -lxcb-icccm -lxcb-xrm -lxcb-util -lpthread

TARGET = custard
PREFIX?=/usr/local
MANPREFIX?=$(PREFIX)/share/man

all: $(TARGET)

$(TARGET): src/main.c
	mkdir -p build
	$(CC) src/main.c $(CFLAGS) -o build/$(TARGET)

install:
	install -m 755 -D build/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -m 755 -D contrib/custardctl.py $(DESTDIR)$(PREFIX)/bin/custardctl
#	install -m 644 -D man/custard.man $(DESTDIR)$(MANPREFIX)/man1/custard.1

clean:
	$(RM) build/$(TARGET)

