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
# Copyright Ben Wiederhake 2017-2018

set -e


# === Set up environment, ===

# If you want to run parts by hand, these might be helpful.
# These are *all* variables used anywhere in the script.

# Sigh.
HOST_MACHINE="$(uname -m)"
HOST="${HOST_MACHINE}-linux-gnu"

# MinGW
MINGW_MACHINE="i686"
MINGW_TARGET="${MINGW_MACHINE}-w64-mingw32"
MINGW_BASE=/usr/${MINGW_TARGET}
MINGW_INCLUDEDIR=/usr/share/mingw-w64/include

# Win32 references
# Sadly, the library paths must be resolved *now* already.
# AND, they must be absolute.  Sigh.
mkdir -p win32
WIN32_GTK_DEV_DIR=`realpath win32/gtk+-bundle_2.24.10-20120208_win32`
WIN32_PIDGIN_DIR=`realpath win32/pidgin-2.12.0`

# Versioning information
./configure -q
make commit.h
VERSION=`grep -E 'PACKAGE_VERSION' config.h | sed -re 's/^.*"(.*)".*$/\1/'`
COMMIT=`grep -E 'define' commit.h | sed -re 's/^.*"(.*)".*$/\1/'`


# === Ensure that all tools are available. ===

# Commands
for cmd in file make makensis "${MINGW_TARGET}-gcc" realpath sed tar unzip ; do
    if ! command -v "$cmd" >/dev/null 2>&1 ; then
        echo "No '$cmd' available.  Are you sure you know what you're doing?"
        exit 1
    fi
done

# Fail-early for some obvious dependencies:
if [ ! -r "${MINGW_BASE}/bin/libgcrypt-20.dll" ] ; then
    echo "You're probably missing libgcrypt-mingw-w64-dev or so."
    echo "Expected file: ${MINGW_BASE}/bin/libgcrypt-20.dll"
    echo "For non-Debian systems, this could be a false positive, though."
    exit 1
fi
if [ ! -r "${MINGW_BASE}/include/gpg-error.h" ] ; then
    echo "You're probably missing libgpg-error-mingw-w64-dev or so."
    echo "Expected file: ${MINGW_BASE}/include/gpg-error.h"
    echo "For non-Debian systems, this could be a false positive, though."
    exit 1
fi
if [ ! -r "${MINGW_BASE}/lib/zlib1.dll" ] ; then
    echo "You're probably missing libz-mingw-w64 or so."
    echo "Expected file: ${MINGW_BASE}/lib/zlib1.dll"
    echo "For non-Debian systems, this could be a false positive, though."
    exit 1
fi
if [ ! -r "${MINGW_BASE}/include/zlib.h" ] ; then
    echo "You're probably missing libz-mingw-w64-dev or so."
    echo "Expected file: ${MINGW_BASE}/include/zlib.h"
    echo "For non-Debian systems, this could be a false positive, though."
    exit 1
fi


# === Download all sources ===

echo "===== 01: Prepare sources"
make clean download_win32_sources


# === Build libpng and libwebp ===

echo "===== 02: Cross-compile libpng, libwebp"
echo "    SKIPPED"


# === Build auto-gen files ===

# Create and backup all auto/ files
echo "===== 03: Create those precious executables"
make tgl/Makefile
cd tgl && git checkout tl-parser/tl-parser.c tl-parser/tlc.c && cd ..
make -C tgl -j4 bin/generate bin/tl-parser
make bin
tar cf bin/tgl-bin.tar tgl/bin/


# === Cross-compilation itself ===

# Clean tgl only (do not touch bin/ with the auto-gen files!)
echo "===== 04: Cleanup tgl"
make -C tgl clean

# Reconfigure
echo "===== 05: Configure for cross-compilation"
# Must first configure telegram-purple, otherwise it thinks tgl/Makefile is outdated.
./configure -q --build ${HOST} --host ${MINGW_TARGET} --target ${MINGW_TARGET} \
    --disable-libwebp --disable-libpng \
    --with-zlib="${MINGW_BASE}" \
    PURPLE_CFLAGS="\${WIN32_INC}" \
    PURPLE_LIBS="-lpurple -lglib-2.0" \
    LDFLAGS="-L${WIN32_GTK_DEV_DIR}/lib -L${WIN32_PIDGIN_DIR}-win32bin" \
    LIBS="-lssp -lintl -lws2_32"
cd tgl
./configure -q --build ${HOST} --host ${MINGW_TARGET} --target ${MINGW_TARGET} \
    --disable-openssl --disable-extf \
    --with-zlib="${MINGW_BASE}" \
    LIBS="-lssp"
cd ..

# Pretend we're building up the preliminaries for auto/
echo "===== 06: Compile tgl, pre-'generate' files"
# Need to override LOCAL_LDFLAGS to remove '-rdynamic'.
echo "    In case this fails mysteriously with some 'file stubs-32.h not found':"
echo "    You're probably missing libc6-dev-i386 or some other dev files."
# Don't care about unportability of tl-parser
echo 'int main(){return 1;}' > tgl/tl-parser/tl-parser.c
echo ' ' > tgl/tl-parser/tlc.c
make -C tgl -j4 LOCAL_LDFLAGS="-lz -lgcrypt -lssp -ggdb" bin/generate bin/tl-parser

# Surprise!  Don't use the host bin/generate and bin/tl-parser to create the autogen files,
# but instead use the *build* versions.  The advantage: we can actually run them!
echo "===== 07: Inject binaries"
tar xf bin/tgl-bin.tar --touch

# Just DO IT!
echo "===== 08: Compile tgl, post-'generate' files"
make -C tgl -j4 LOCAL_LDFLAGS="-lz -lgcrypt -lssp -ggdb" libs/libtgl.a

# Don't let your DREAMS stay DREAMS!
echo "===== 09: Compile telegram-purple"
# CFLAGS: Remove LOCALEDIR definition.
# PRPL_NAME: Assign Windows-specific name.
#            (Baked into the file, so we can't just rename it.)
# LDFLAGS: Remove -rdynamic flag.
make -j4 bin/libtelegram.dll \
    CFLAGS_INTL=-DENABLE_NLS \
    PRPL_NAME=libtelegram.dll \
    LDFLAGS_EXTRA=-ggdb

# There's no monster under your bed, I swear.
echo "===== 10: Unspoof files"
# Stealth cleanup
cd tgl && git checkout tl-parser/tl-parser.c tl-parser/tlc.c && cd ..
