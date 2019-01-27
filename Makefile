CC ?= gcc
LD  = $(CC)

CFLAGS   = -Wall -Wextra -pedantic -std=c99 -O2
CPPFLAGS = -MD -MP -D_POSIX_C_SOURCE=200809L
LDFLAGS  = -lxcb -lxcb-ewmh -lxcb-icccm -lxcb-util -lconfig -lpcre

PREFIX    = /usr/local
BINPREFIX = $(PREFIX)/bin
MANPREFIX = $(PREFIX)/share/man

# Needed so that $(SRCPREFIX)/something won't go to somewhere in the root dir
ifeq ($(SRCPREFIX),)
	SRCPREFIX=.
endif

TARGET = custard
SRC    = $(wildcard $(SRCPREFIX)/src/*.c)
OBJ    = $(subst $(SRCPREFIX)/src/,,$(SRC:.c=.o))
DEPS   = $(OBJ:.o=.d)
VPATH  = $(SRCPREFIX)/src

.PHONY: all clean install uninstall

all: $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

.c.o:
	$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) -c "$<"

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) $(DEPS)

install: all
	install -Dm755 $(TARGET) "$(DESTDIR)$(BINPREFIX)/$(TARGET)"
	install -Dm755 "$(SRCPREFIX)/contrib/custardctl.py" "$(DESTDIR)$(BINPREFIX)/custardctl"
#	install -Dm644 "$(SRCPREFIX)/man/custard.man" "$(DESTDIR)$(MANPREFIX)/man1/custard.1"

uninstall:
	$(RM) "$(DESTDIR)$(BINPREFIX)/$(TARGET)"
	$(RM) "$(DESTDIR)$(BINPREFIX)/custardctl"
#	$(RM) "$(DESTDIR)$(MANPREFIX)/man1/custardctl.1"
