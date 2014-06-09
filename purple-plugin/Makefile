# Include libs from tg (--disable-liblua --disable-lib)

# TODO: These flags should depend on the used configuration flags
EXTRA_LIBS = -lcrypto -lz -lm -lreadline
LDFLAGSTG = -L/usr/local/lib
CPPFLAGSTG = -I/usr/local/include
OBJECTSTG = ../main.o ../loop.o ../interface.o ../net.o ../mtproto-common.o ../mtproto-client.o ../queries.o ../structures.o ../binlog.o ../tools.o ../lua-tg.o ../msglog.o
INCLUDE = -I. -I..
#

LIBNAME = telegram-purple.so
ARCH = ""

ifeq ($(ARCH),i686)
	ARCHFLAGS = -m32
else ifeq ($(ARCH),x86_64)
	ARCHFLAGS = -m64
else
	ARCHFLAGS =
endif

.PHONY: all
all: $(LIBNAME)

C_SRCS = telegram-purple.c
C_OBJS = $(C_SRCS:.c=.o)

STRIP = strip
CC = gcc
LD = $(CC)
CFLAGS_PURPLE = $(shell pkg-config --cflags purple)
CFLAGS = \
	$(ARCHFLAGS) \
	-fPIC \
	-DPURPLE_PLUGINS \
	-DPIC \
	$(CFLAGS_PURPLE)

LIBS_PURPLE = $(shell pkg-config --libs purple)
LDFLAGS =  $(ARCHFLAGS) -shared

%.o: %.c
	$(CC) -c $(INCLUDE) $(CFLAGS) -o $@ $<

$(LIBNAME): $(C_OBJS)
	cd ..; make objects; cd ./purple-plugin;
	$(LD) $(LDFLAGS) $(INCLUDE) $(LIBS_PURPLE) $(EXTRA_LIBS) -o $@ $^ $(OBJECTSTG)

.PHONY: strip
strip: $(LIBNAME)
	$(STRIP) --strip-unneeded $(LIBNAME)

.PHONY: debug install
debug: CFLAGS += -g -DDEBUG
debug: $(LIBNAME)
	gdb -tui pidgin


PLUGIN_DIR_PURPLE:=$(shell pkg-config --variable=plugindir purple)
DATA_ROOT_DIR_PURPLE:=$(shell pkg-config --variable=datarootdir purple)

# TODO: Find a better place for server.pub
.PHONY: install
install: $(LIBNAME)
	install -D $(LIBNAME) $(DESTDIR)$(PLUGIN_DIR_PURPLE)/$(LIBNAME)
	install -D ../tg-server.pub /etc/telegram/server.pub
	install -D telegram16.png $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/16/telegram.png
	install -D telegram22.png $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/22/telegram.png
	install -D telegram48.png $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/48/telegram.png

.PHONY: uninstall
uninstall: $(LIBNAME)
	rm -f $(DESTDIR)$(PLUGIN_DIR_PURPLE)/$(LIBNAME)
	rm -f /etc/telegram/server.pub
	rm -f $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/16/telegram.png
	rm -f $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/22/telegram.png
	rm -f $(DESTDIR)$(DATA_ROOT_DIR_PURPLE)/pixmaps/pidgin/protocols/48/telegram.png

.PHONY: clean
clean:
	-rm -f *.o *.a
	-rm -f $(LIBNAME)

.PHONY: run
run: all install
	pidgin -d
