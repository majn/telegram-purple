#!/bin/sh

set -e


# === Ensure that all tools are available. ===

# Commands
for cmd in file make makensis i686-w64-mingw32-gcc realpath tar unzip ; do
    if ! command -v $cmd >/dev/null 2>&1 ; then
        echo "No '$cmd' available.  Are you sure you know what you're doing?"
        exit 1
    fi
done
WIN_CC=i686-w64-mingw32-gcc

# Folder-symlink
LINK_TYPE=`LC_ALL=C file /usr/share/mingw-w64/include/webp`
if [ "/usr/share/mingw-w64/include/webp: symbolic link to /usr/include/webp" != "${LINK_TYPE}" ] ; then
    echo "Need a way to directly link into libwebp.  Please run this as root:"
    echo "    ln -s /usr/include/webp /usr/share/mingw-w64/include/webp"
    exit 1
fi

# Fail-early for some obvious dependencies:
if [ ! -r /usr/i686-w64-mingw32/lib/zlib1.dll ] ; then
    echo "You're probably missing libz-mingw-w64 or so."
    echo "Expected file: /usr/i686-w64-mingw32/lib/zlib1.dll"
    echo "For non-Debian systems, this could be a false positive, though."
    exit 1
fi
if [ ! -r /usr/i686-w64-mingw32/include/zlib.h ] ; then
    echo "You're probably missing libz-mingw-w64-dev or so."
    echo "Expected file: /usr/i686-w64-mingw32/include/zlib.h"
    echo "For non-Debian systems, this could be a false positive, though."
    exit 1
fi
if [ ! -r /usr/i686-w64-mingw32/bin/libgcrypt-20.dll ] ; then
    echo "You're probably missing libgcrypt-mingw-w64-dev or so."
    echo "Expected file: /usr/i686-w64-mingw32/bin/libgcrypt-20.dll"
    echo "For non-Debian systems, this could be a false positive, though."
    exit 1
fi
if [ ! -r /usr/i686-w64-mingw32/include/gpg-error.h ] ; then
    echo "You're probably missing libgpg-error-mingw-w64-dev or so."
    echo "Expected file: /usr/i686-w64-mingw32/include/gpg-error.h"
    echo "For non-Debian systems, this could be a false positive, though."
    exit 1
fi


# === Download all sources ===

echo "===== 1: Prepare sources"
./configure -q
make clean download_win32_sources


# === Build auto-gen files ===

# Create and backup all auto/ files
echo "===== 2: Create auto/ files"
make tgl/Makefile
make -C tgl -j4 auto/auto-autocomplete.c auto/auto.c auto/auto-fetch.c auto/auto-fetch-ds.c auto/auto-free-ds.c
make -C tgl -j4 auto/auto-print-ds.c auto/auto-skip.c auto/auto-store.c auto/auto-store-ds.c auto/auto-types.c
make bin
tar cf bin/tgl-auto-code.tar tgl/auto/


# === Cross-compilation itself ===

# Clean tgl only (do not touch bin/ with the auto-gen files!)
echo "===== 3: Cleanup tgl"
make -C tgl clean

# Reconfigure
echo "===== 4: Configure for cross-compilation"
# Sadly, the library paths must be resolved *now* already.
# AND, they must be absolute.  Sigh.
export WIN32_GTK_DEV_DIR=`realpath win32/gtk_2_0-2.16`
export WIN32_PIDGIN_DIR=`realpath win32/pidgin-2.12.0`
./configure -q --host x86_64-linux-gnu --target i686-w64-mingw32 \
    --disable-libwebp --disable-translation \
    --includedir="/usr/share/mingw-w64/include" \
    --oldincludedir="/usr/share/mingw-w64/include" \
    --libdir="/usr/i686-w64-mingw32" \
    --libexecdir="/usr/i686-w64-mingw32" \
    --with-zlib="/usr/i686-w64-mingw32" \
    CC=i686-w64-mingw32-gcc CFLAGS="-g -O2" \
    PURPLE_CFLAGS="\${WIN32_INC}" \
    PURPLE_LIBS="-lpurple -lglib-2.0" \
    LDFLAGS="-Wl,--export-all-symbols -L${WIN32_GTK_DEV_DIR}/lib -L${WIN32_PIDGIN_DIR}-win32bin" \
    LIBS="-lssp -lintl -lz -lgcrypt -lws2_32"
cd tgl
./configure -q --host x86_64-linux-gnu --target i686-w64-mingw32 \
    --disable-openssl --disable-extf \
    --includedir="/usr/share/mingw-w64/include" \
    --oldincludedir="/usr/share/mingw-w64/include" \
    --libdir="/usr/i686-w64-mingw32" \
    --libexecdir="/usr/i686-w64-mingw32" \
    --with-zlib="/usr/i686-w64-mingw32" \
    CC=i686-w64-mingw32-gcc CFLAGS="-g -O2" LDFLAGS="-lssp"
cd ..

# Pretend we're building up the preliminaries for auto/
echo "===== 5: Compile tgl, pre-'generate' files"
# Need to override LOCAL_LDFLAGS to remove '-rshared'.
echo "    In case this fails mysteriously with some 'file stubs-32.h not found':"
echo "    You're probably missing libc6-dev-i386 or some other dev files."
make -C tgl -j4 LOCAL_LDFLAGS="-lz -lgcrypt -ggdb" bin/generate

# Surprise!  Don't actually call bin/generate to create the autogen files,
# And make sure that calling the binaries definitely would fail.
echo "===== 6: Inject auto/ files"
touch tgl/objs/tl-parser.o tgl/objs/tlc.o
cp /bin/false tgl/bin/tl-parser
cp /bin/false tgl/bin/generate
tar xf bin/tgl-auto-code.tar --touch
# Make sure that the timestamps look sane.
cd tgl/auto
    # First comes scheme.tl:
    touch scheme.tl
    # Then the tlo:
    touch scheme.tlo
    # Which generates the scheme2:
    touch scheme2.tl
    # Which is used to extract the constants:
    touch constants.h
    # With these, all the rest (but first the headers):
    touch auto-autocomplete.h auto-fetch-ds.h auto-fetch.h auto-free-ds.h
    touch auto-print-ds.h auto-skip.h auto-store-ds.h auto-store.h auto-types.h
    touch auto-autocomplete.c auto.c auto-fetch.c auto-fetch-ds.c auto-free-ds.c
    touch auto-print-ds.c auto-skip.c auto-store.c auto-store-ds.c auto-types.c
cd ../..

# Just DO IT!
echo "===== 7: Compile tgl, post-'generate' files"
make -C tgl -j4 LOCAL_LDFLAGS="-lz -lgcrypt -ggdb" libs/libtgl.a

# Don't let your DREAMS stay DREAMS!
echo "===== 8: Compile telegram-purple"
# CFLAGS: Remove LOCALEDIR definition.
# PRPL_NAME: Assign Windows-specific name.
#            (Baked into the file, so we can't just rename it.)
# LDFLAGS: Remove -rdynamic flag.
make -j4 bin/libtelegram.dll \
    CFLAGS_INTL=-DENABLE_NLS \
    PRPL_NAME=libtelegram.dll \
    LDFLAGS_EXTRA=-ggdb
