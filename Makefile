CC?=gcc

OPTFLAGS = -O2
CFLAGS = $(OPTFLAGS) -std=c99 -pedantic

ifeq ($(CC), clang)
	CFLAGS+=-Weverything
else
	CFLAGS+=-Wall -Wextra
endif

CPPFLAGS = -MD -MP -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lxcb -lxcb-ewmh -lxcb-icccm -lxcb-util -lpthread

TARGET = custard
BUILDPREFIX=build
SRCS = $(wildcard src/*.c)
OBJS = $(subst src,$(BUILDPREFIX),$(SRCS:.c=.o))
PREFIX?=/usr/local
MANPREFIX?=$(PREFIX)/share/man

.PHONY: all install clean

all: prepare $(OBJS) $(TARGET)

-include $(subst src,$(BUILDPREFIX),$(SRCS:.c=.d))

$(TARGET): $(OBJS)
	$(CC) -o $(BUILDPREFIX)/$@ $^ $(LDFLAGS)

prepare:
	mkdir -p $(BUILDPREFIX)

$(BUILDPREFIX)/%.o: src/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

install:
	install -m 755 -D $(BUILDPREFIX)/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -m 755 -D contrib/custardctl.py $(DESTDIR)$(PREFIX)/bin/custardctl
#	install -m 644 -D man/custard.man $(DESTDIR)$(MANPREFIX)/man1/custard.1

clean:
	$(RM) -r $(BUILDPREFIX)

