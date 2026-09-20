# openkb -- plain Makefile (SDL2 only, no other external dependencies)
#
#   make            build ./openkb
#   make netkb      build the network combat prototype (needs SDL2_net)
#   make install    install to $(PREFIX) (default /usr/local)
#
# Override CC / CFLAGS / SDL2_CONFIG / PREFIX / DATADIR on the command line.

CC        ?= cc
SDL2_CONFIG ?= sdl2-config
PREFIX    ?= /usr/local
BINDIR    ?= $(PREFIX)/bin
DATADIR   ?= $(PREFIX)/share
MANDIR    ?= $(PREFIX)/share/man
INSTALL   ?= install

VERSION   = 0.0.3

SDL_CFLAGS := $(shell $(SDL2_CONFIG) --cflags)
SDL_LIBS   := $(shell $(SDL2_CONFIG) --libs)

CFLAGS  ?= -O2 -g
CFLAGS  += -Wall -Wno-unused $(SDL_CFLAGS)
LDLIBS  += $(SDL_LIBS) -lm

ifneq ($(DATADIR),/usr/local/share)
CFLAGS += -DPKGDATADIR=\"$(DATADIR)\"
endif

LIB_SOURCES = \
	vendor/strlcat.c vendor/strlcpy.c \
	src/lib/kbstd.c src/lib/kbconf.c \
	src/lib/kbfile.c src/lib/kbdir.c src/lib/kbres.c src/lib/kbpng.c \
	src/lib/free-data.c src/lib/free-snd.c \
	src/lib/dos-data.c src/lib/dos-cc.c src/lib/dos-img.c src/lib/dos-snd.c src/lib/dos-exe.c \
	src/lib/md-rom.c \
	src/lib/kbauto.c

VEND_SOURCES = vendor/scale2x.c vendor/inprint.c vendor/lodepng.c

GAME_SOURCES  = src/main.c src/save.c src/game.c src/play.c src/bounty.c src/env-sdl.c src/ui.c src/rogue.c
GAME2_SOURCES = src/combat.c src/bounty.c src/play.c src/env-sdl.c

LIB_OBJECTS   = $(LIB_SOURCES:.c=.o)
VEND_OBJECTS  = $(VEND_SOURCES:.c=.o)
GAME_OBJECTS  = $(GAME_SOURCES:.c=.o)
GAME2_OBJECTS = $(GAME2_SOURCES:.c=.o)

LIB_BINARY   = src/libkb.a
GAME_BINARY  = openkb
GAME2_BINARY = netkb

MAN_SOURCES = docs/openkb.man docs/netkb.man
MAN_PAGES   = $(MAN_SOURCES:.man=.6)

all: $(GAME_BINARY)

$(LIB_BINARY): $(LIB_OBJECTS)
	$(AR) rcs $@ $(LIB_OBJECTS)

$(GAME_BINARY): $(GAME_OBJECTS) $(VEND_OBJECTS) $(LIB_BINARY)
	$(CC) $(LDFLAGS) $(GAME_OBJECTS) $(VEND_OBJECTS) $(LIB_BINARY) $(LDLIBS) -o $@

$(GAME2_BINARY): $(GAME2_OBJECTS) $(VEND_OBJECTS) $(LIB_BINARY)
	$(CC) $(LDFLAGS) $(GAME2_OBJECTS) $(VEND_OBJECTS) $(LIB_BINARY) $(LDLIBS) -lSDL2_net -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# lodepng is third-party and noisy under -Wall; keep it quiet
vendor/lodepng.o: vendor/lodepng.c
	$(CC) -c $(CFLAGS) -w $< -o $@

%.6: %.man
	nroff -man $< > $@

mans: $(MAN_PAGES)

clean:
	rm -f $(LIB_OBJECTS) $(VEND_OBJECTS) $(GAME_OBJECTS) $(GAME2_OBJECTS) \
	      $(LIB_BINARY) $(GAME_BINARY) $(GAME2_BINARY) $(MAN_PAGES)

install: all
	$(INSTALL) -d $(DESTDIR)$(BINDIR) $(DESTDIR)$(DATADIR)/openkb $(DESTDIR)$(MANDIR)/man6
	$(INSTALL) -m 755 $(GAME_BINARY) $(DESTDIR)$(BINDIR)/openkb
	cp -R data/. $(DESTDIR)$(DATADIR)/openkb/
	-nroff -man docs/openkb.man > $(DESTDIR)$(MANDIR)/man6/openkb.6

freeworld: data/free/land.tmx
	python3 src/tools/kbtmx.py data/free/land.tmx data/free

.PHONY: all clean install mans freeworld
