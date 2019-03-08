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


# === What flavor? ===

if [ -z "${USE_WEBP}" ]
then
    USE_WEBP=y
    # Otherwise:
    # USE_WEBP=n
fi
if [ -z "${USE_VERSIONINFO}" ]
then
    USE_VERSIONINFO=y
    # Otherwise:
    # USE_VERSIONINFO=n
fi

# Other flags go here, i.e., libpng


# === Set up environment. ===

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

# WebP compilation
WEBP_INSTALL_DIR="objs/webp-install/"
mkdir -p ${WEBP_INSTALL_DIR}  # Needed for realpath
WEBP_INSTALL_DIR_FULL="$(realpath ${WEBP_INSTALL_DIR})"
WEBP_BUILD_DIR="objs/webp-build/"

# Win32 references
# Sadly, the library paths must be resolved *now* already.
# AND, they must be absolute.  Sigh.
mkdir -p win32
WIN32_GTK_DEV_DIR=`realpath win32/gtk+-bundle_2.24.10-20120208_win32`
WIN32_PIDGIN_DIR=`realpath win32/pidgin-2.12.0`
WIN32_WEBP_PRISTINE="win32/libwebp-0.6.1/"

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

# Sadly, we cannot use the pre-built binaries from Google,
# as they are built with MVSC and that seems to be incompatible with MinGW.
# Specifically, reading their libwebp.lib (requires special linker flags to
# find that file) fails with the error "corrupt .drectve at end of def file".
# GCC version 6.3.0 20170516 (GCC)
echo "===== 02: Cross-compile libpng, libwebp"
if [ "n" = "${USE_WEBP}" ]
then
    echo "    Skipping webp"
else
    mkdir -p "${WEBP_INSTALL_DIR}"
    # -a to (hopefully) preserve some timestamps
    cp -ar "${WIN32_WEBP_PRISTINE}" "${WEBP_BUILD_DIR}"
    cd "${WEBP_BUILD_DIR}"
        # ./configure && make

        # Disable linking against PNG, JPEG, TIFF, GIF, WIC,
        # as those would either need cross-compilation, too, or some other magic.
        ./configure -q --build ${HOST} --host ${MINGW_TARGET} --target ${MINGW_TARGET} \
            --disable-dependency-tracking --prefix="${WEBP_INSTALL_DIR_FULL}/" \
            --disable-static --enable-shared \
            --enable-swap-16bit-csp --enable-experimental \
            --disable-libwebpmux --disable-libwebpdemux \
            --disable-libwebpdecoder --disable-libwebpextras \
            --disable-png --disable-jpeg --disable-tiff --disable-gif --disable-wic
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
    cd ../..
    if ! [ -r ${WEBP_INSTALL_DIR}/include/webp/decode.h -a -r ${WEBP_INSTALL_DIR}/bin/libwebp-7.dll ] ; then
        # I expect that cross-compiling webp is going to be very fragile,
        # so print a nice error in case this happens.
        echo "Cross-compilation apparently failed:"
        echo "Some files in ${WEBP_INSTALL_DIR} are missing."
        echo "Please submit a bugreport."
        exit 1
    fi
    mkdir -p contrib
    cp ${WEBP_INSTALL_DIR}/bin/libwebp-7.dll contrib
fi


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
if [ "y" = "${USE_WEBP}" ]
then
    LDFLAGS_WEBP="-L${WEBP_INSTALL_DIR_FULL}/bin"
    CONFFLAGS_WEBP="--enable-libwebp"
fi
./configure -q --build ${HOST} --host ${MINGW_TARGET} --target ${MINGW_TARGET} \
    --disable-libpng --disable-libwebp \
    --with-zlib="${MINGW_BASE}" \
    PURPLE_CFLAGS="\${WIN32_INC}" \
    PURPLE_LIBS="-lpurple -lglib-2.0" \
    LDFLAGS="-L${WIN32_GTK_DEV_DIR}/lib -L${WIN32_PIDGIN_DIR}-win32bin ${LDFLAGS_WEBP}" \
    LIBS="-lssp -lintl -lws2_32" \
    ${CONFFLAGS_WEBP}
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
# Optionally, prepare and add the "resources" file.
VERSIONINFO_OBJECTS=""
if [ "y" = "${USE_VERSIONINFO}" ]
then
    make objs commit.h
    cat <<EOFRC > objs/info.rc
#include "../commit.h"
1 VERSIONINFO
FILEVERSION MAGIC_SED_VERSION
BEGIN
  BLOCK "StringFileInfo"
  BEGIN
    BLOCK "0409"
    BEGIN
      VALUE "FileDescription", "Telegram protocol plug-in for libpurple"
      VALUE "FileVersion", GIT_COMMIT
    END
  END
END
EOFRC
    COMMA_VERSION=`grep -E 'PACKAGE_VERSION' config.h | sed -re 's/^.*"(.*)".*$/\1/' -e 's/\./,/g'`
    sed -i -re "s/MAGIC_SED_VERSION/${COMMA_VERSION}/" objs/info.rc
    ${MINGW_TARGET}-windres objs/info.rc -O coff -o objs/info.res
    VERSIONINFO_OBJECTS="objs/info.res"
fi
# CFLAGS: Remove LOCALEDIR definition.
# PRPL_NAME: Assign Windows-specific name.
#            (Baked into the file, so we can't just rename it.)
# LDFLAGS: Remove -rdynamic flag.
make -j4 bin/libtelegram.dll \
    CFLAGS_INTL=-DENABLE_NLS \
    PRPL_NAME=libtelegram.dll \
    EXTRA_OBJECTS="${VERSIONINFO_OBJECTS}" \
    LDFLAGS_EXTRA=-ggdb

# Package it up
echo "===== 10: Create installer"
make PRPL_NAME=libtelegram.dll win-installer-deps
makensis -DPLUGIN_VERSION="${VERSION}+g${COMMIT}" -DPRPL_NAME=libtelegram.dll \
    -DWIN32_DEV_TOP=contrib telegram-purple.nsi

# There's no monster under your bed, I swear.
echo "===== 11: Unspoof files"
# Stealth cleanup
cd tgl && git checkout tl-parser/tl-parser.c tl-parser/tlc.c && cd ..

echo "===== COMPLETE: All done.  Installer executable is in top directory."
