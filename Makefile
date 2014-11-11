#
# Telegram Flags
#

srcdir=.
CFLAGS=-g -Wall -Wextra -Werror -Wno-unused-parameter
LDFLAGS=-L/usr/local/lib
CPPFLAGS=-I/usr/local/include -Itg
DEFS=
COMPILE_FLAGS=${CFLAGS} ${CPPFLAGS} ${DEFS} -Wall -Wextra -Wno-deprecated-declarations -fno-strict-aliasing -fno-omit-frame-pointer -ggdb
EXTRA_LIBS=-lcrypto -lz -lm
LOCAL_LDFLAGS=-rdynamic -ggdb ${EXTRA_LIBS}
LINK_FLAGS=${LDFLAGS} ${LOCAL_LDFLAGS}

INCLUDE=-I. -I${srcdir}
CC=cc
OBJECTS=tgp-net.o tgp-timers.o msglog.o telegram-base.o

.SUFFIXES:

.SUFFIXES: .c .h .o


#
# Plugin Flags
#

LIBS_PURPLE = $(shell pkg-config --libs purple)
CFLAGS_PURPLE = $(shell pkg-config --cflags purple)
PLUGIN_DIR_PURPLE:=$(shell pkg-config --variable=plugindir purple)
DATA_ROOT_DIR_PURPLE:=$(shell pkg-config --variable=datarootdir purple)

ARCH = ""
ifeq ($(ARCH),i686)
	ARCHFLAGS = -m32
else ifeq ($(ARCH),x86_64)
	ARCHFLAGS = -m64
else
   ARCHFLAGS =
endif

LD = $(CC)
PRPL_C_SRCS = telegram-purple.c
PRPL_C_OBJS = $(PRPL_C_SRCS:.c=.o)
PRPL_LIBNAME = telegram-purple.so
PRPL_INCLUDE = -I. -I./purple-plugin
PRPL_LDFLAGS =  $(ARCHFLAGS) -shared
STRIP = strip
PRPL_CFLAGS = \
	$(ARCHFLAGS) \
	-fPIC \
	-DPURPLE_PLUGINS \
	-DPIC \
    -DDEBUG \
	-g \
	$(CFLAGS_PURPLE)

#
# Telegram Objects
#

.c.o :
	${CC} -fPIC -DPIC ${CFLAGS_PURPLE} ${COMPILE_FLAGS} ${INCLUDE} -c $< -o $@

# ${OBJECTS}: ${HEADERS}

#telegram: ${OBJECTS}
#	${CC} ${OBJECTS} ${LINK_FLAGS} -o $@

#
# Plugin Objects
#

$(PRPL_C_OBJS): $(PRPL_C_SRCS)
	$(CC) -c $(PRPL_INCLUDE) $(PRPL_CFLAGS) $(CFLAGS) $(CPPFLAGS) -ggdb -o $@ $<

$(PRPL_LIBNAME): $(OBJECTS) $(PRPL_C_OBJS) tg/libs/libtgl.a
	$(LD) $(PRPL_LDFLAGS) $(LDFLAGS) $(PRPL_INCLUDE) $(LIBS_PURPLE) $(EXTRA_LIBS) -o $@ $^

.PHONY: all
all: ${PRPL_LIBNAME}

plugin: $(PRPL_LIBNAME)


.PHONY: strip
strip: $(PRPL_LIBNAME)
	$(STRIP) --strip-unneeded $(PRPL_LIBNAME)

# TODO: Find a better place for server.pub
install: $(PRPL_LIBNAME)
	install -D $(PRPL_LIBNAME) $(DESTDIR)$(PLUGIN_DIR_PURPLE)/$(PRPL_LIBNAME)
	install -D tg-server.pub /etc/telegram-purple/server.pub
	install -D imgs/telegram16.png $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/16/telegram.png
	install -D imgs/telegram22.png $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/22/telegram.png
	install -D imgs/telegram48.png $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/48/telegram.png

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PLUGIN_DIR_PURPLE)/$(PRPL_LIBNAME)
	rm -f /etc/telegram-purple/server.pub
	rm -f $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/16/telegram.png
	rm -f $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/22/telegram.png
	rm -f $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/48/telegram.png

.PHONY: run
run: install
	pidgin -d | grep 'telegram\|plugin\|proxy'

.PHONY: purge
purge: uninstall
	rm -rf $(HOME)/.telegram-purple

.PHONY: debug
debug: install
	ddd pidgin

clean:
	rm -rf *.so *.a *.o telegram config.log config.status $(PRPL_C_OBJS) $(PRPL_LIBNAME) > /dev/null || echo "all clean"

