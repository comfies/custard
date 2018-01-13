
CC=g++

CFLAGS = -lxcb -lxcb-ewmh -lxcb-xrm -lpthread

TARGET = custard
PREFIX?=/usr/local
MANPREFIX?=$(PREFIX)/share/man

all: $(TARGET)

$(TARGET): workspace/src/main.cpp
	mkdir -p workspace/build
	$(CC) workspace/src/main.cpp $(CFLAGS) -o workspace/build/$(TARGET)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p workspace/build
	install workspace/build/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install workspace/man/custard.man $(DESTDIR)$(MANPREFIX)/man1/custard.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/custard.1

clean:
	$(RM) workspace/build/$(TARGET)

