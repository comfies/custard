
CC=g++

OPTFLAGS = -O3
CFLAGS = $(OPTFLAGS) -std=c++14 -lxcb -lxcb-ewmh -lxcb-icccm -lxcb-xrm -lxcb-util -lpthread -Wno-unused-result

TARGET = custard
PREFIX?=/usr/local
MANPREFIX?=$(PREFIX)/share/man

all: $(TARGET)

$(TARGET): src/main.cpp
	mkdir -p build
	$(CC) src/main.cpp $(CFLAGS) -o build/$(TARGET)

install: $(TARGET)
	install -m 755 -D build/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -m 755 -D contrib/custardctl.py $(DESTDIR)$(PREFIX)/bin/custardctl
	install -m 644 -D man/custard.man $(DESTDIR)$(MANPREFIX)/man1/custard.1

clean:
	$(RM) build/$(TARGET)

