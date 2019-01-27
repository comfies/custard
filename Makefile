CC ?= gcc
LD  = $(CC)

CFLAGS		=	-Wall -Wextra -pedantic -std=c99 -O2
CPPFLAGS	=	-MD -MP -D_POSIX_C_SOURCE=200809L
LDFLAGS		=	-lxcb -lxcb-ewmh -lxcb-icccm -lxcb-util -lconfig -lpcre

PREFIX		= 	/usr/local
BINPREFIX	= 	$(PREFIX)/bin
MANPREFIX	= 	$(PREFIX)/share/man

SRCPREFIX	=	src
BUILDPREFIX = 	build

TARGET	=	custard
SRC		:=	$(wildcard $(SRCPREFIX)/*.c)
OBJ		:=	$(subst $(SRCPREFIX),$(BUILDPREFIX),$(SRC:.c=.o))
DEPS	=	$(OBJ:.o=.d)
VPATH	=	$(SRCPREFIX)

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJ)
	@ #printf "[34mLD[0m :: $@\n"
	$(LD) -o $(BUILDPREFIX)/$@ $(OBJ) $(LDFLAGS)

$(BUILDPREFIX)/%.o: $(SRCPREFIX)/%.c
	@[ -d build ] || mkdir -p $(BUILDPREFIX)
	@ #printf "[32mCC[0m :: $@\n"
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	$(RM) -r $(BUILDPREFIX)

install: all
	install -Dm755 $(TARGET) "$(DESTDIR)$(BINPREFIX)/$(TARGET)"
	install -Dm755 "$(SRCPREFIX)/contrib/custardctl.py" "$(DESTDIR)$(BINPREFIX)/custardctl"
#	install -Dm644 "$(SRCPREFIX)/man/custard.man" "$(DESTDIR)$(MANPREFIX)/man1/custard.1"

uninstall:
	$(RM) "$(DESTDIR)$(BINPREFIX)/$(TARGET)"
	$(RM) "$(DESTDIR)$(BINPREFIX)/custardctl"
#	$(RM) "$(DESTDIR)$(MANPREFIX)/man1/custardctl.1"
