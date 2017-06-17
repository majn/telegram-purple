#!/bin/sh

# This file is part of telegram-purple
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
#
# Copyright Ben Wiederhake 2017

set -e


# === Ensure that all tools are available. ===

# Commands
for cmd in file make makensis i686-w64-mingw32-gcc realpath sed tar unzip ; do
    if ! command -v $cmd >/dev/null 2>&1 ; then
        echo "No '$cmd' available.  Are you sure you know what you're doing?"
        exit 1
    fi
done
WIN_CC=i686-w64-mingw32-gcc

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


# === Build libwebp ===

# Sadly, we cannot use the pre-built binaries from Google,
# as they are built with MVSC and that seems to be incompatible with MinGW.
# Specifically, reading their libwebp.lib (requires special linker flags to
# find that file) fails with the error "corrupt .drectve at end of def file".
# GCC version 6.3.0 20170516 (GCC)
echo "===== 1b: Cross-compile webp"
WEBP_DIR="objs/webp"
mkdir -p ${WEBP_DIR}
WEBP_DIR_FULL="`realpath ${WEBP_DIR}`"
WEBP_BUILD_DIR="objs/webp-build"
# -a to (hopefully) preserve some timestamps
cp -ar win32/libwebp-0.6.0 "${WEBP_BUILD_DIR}"
cd "${WEBP_BUILD_DIR}"
    # ./configure && make
    #
    # So libtool.m4 (line 10043 in configure) thinks that:
    # - if the default configuration of the compiler contains ' -lc' (note the space), then do nothing
    # - else, append ' -lc' to the config.
    # I'm not sure why this is a good idea, but '-lc' is definitely wrong here.
    sed -i -re 's%lt_cv_archive_cmds_need_lc=yes%lt_cv_archive_cmds_need_lc=no%' configure
    #
    # Disable linking against PNG, JPEG, TIFF, GIF, WIC,
    # as those would either need cross-compilation, too, or some other magic.
    ./configure -q --host x86_64-linux-gnu --target i686-w64-mingw32 \
        --disable-dependency-tracking --prefix="${WEBP_DIR_FULL}/" \
        --disable-static --enable-shared \
        --enable-swap-16bit-csp --enable-experimental \
        --disable-libwebpmux --disable-libwebpdemux \
        --disable-libwebpdecoder --disable-libwebpextras \
        --disable-png --disable-jpeg --disable-tiff --disable-gif --disable-wic \
        CC=i686-w64-mingw32-gcc
    # The warning
    # "configure: WARNING: using cross tools not prefixed with host triplet"
    # stems from the fact that 'mt' (magnetic tape control) is not available
    # for i686-w64-mingw32, so the x86_64 version is used.  We can ignore that.
    #
    # Try to avoid too extreme autotools overhead:
    touch Makefile.in
    #
    # Finally.
    make --quiet -j4 install
    #
    # Fix bad naming by rebuilding that one file.  With blackjack and hookers!
    rm -rf ../webp/lib
    mkdir ../webp/lib
    i686-w64-mingw32-gcc -shared -fPIC -DPIC  -Wl,--whole-archive src/dec/.libs/libwebpdecode.a src/dsp/.libs/libwebpdsp.a src/enc/.libs/libwebpencode.a src/utils/.libs/libwebputils.a -Wl,--no-whole-archive -lm -O2 -pthread -o ../webp/lib/libwebp.dll
cd ../..
if ! [ -r ${WEBP_DIR}/include/webp/decode.h -a -r ${WEBP_DIR}/lib/libwebp.a ] ; then
    # I expect that cross-compiling webp is going to be very fragile,
    # so print a nice error in case this happens.
    echo "Cross-compilation apparently failed:"
    echo "Some files in ${WEBP_DIR} are missing."
    echo "Please submit a bugreport."
fi


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
export WIN32_WEBP_DIR="${WEBP_DIR_FULL}"
./configure -q --host x86_64-linux-gnu --target i686-w64-mingw32 \
    --includedir="/usr/share/mingw-w64/include" \
    --oldincludedir="/usr/share/mingw-w64/include" \
    --libdir="/usr/i686-w64-mingw32" \
    --libexecdir="/usr/i686-w64-mingw32" \
    --with-zlib="/usr/i686-w64-mingw32" \
    CC=i686-w64-mingw32-gcc CFLAGS="-g -O2" \
    PURPLE_CFLAGS="\${WIN32_INC}" \
    PURPLE_LIBS="-lpurple -lglib-2.0" \
    LDFLAGS="-Wl,--export-all-symbols -L${WIN32_GTK_DEV_DIR}/lib -L${WIN32_PIDGIN_DIR}-win32bin -L${WIN32_WEBP_DIR}/lib" \
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
