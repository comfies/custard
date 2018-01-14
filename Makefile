
CC=g++

CFLAGS = -std=c++14 -lxcb -lxcb-ewmh -lxcb-xrm -lpthread

TARGET = custard
PREFIX?=/usr/local
MANPREFIX?=$(PREFIX)/share/man

all: $(TARGET)

$(TARGET): src/main.cpp
	mkdir -p build
	$(CC) src/main.cpp $(CFLAGS) -o build/$(TARGET)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p build
	install build/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install man/custard.man $(DESTDIR)$(MANPREFIX)/man1/custard.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/custard.1

clean:
	$(RM) build/$(TARGET)

