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
else
    echo "Note: webp disabled"
fi
if [ -z "${USE_PNG}" ]
then
    USE_PNG=y
    # Otherwise:
    # USE_PNG=n
else
    echo "Note: png disabled"
fi
if [ -z "${USE_VERSIONINFO}" ]
then
    USE_VERSIONINFO=y
    # Otherwise:
    # USE_VERSIONINFO=n
fi
if [ -z "${USE_REPRODUCIBLE}" ]
then
    USE_REPRODUCIBLE=y
    # Otherwise:
    # USE_REPRODUCIBLE=n
    FAKETIME="with_faketime"
    SOURCE_DATE_EPOCH="$(git log -1 --date='format:%s' --format=%cd)"

    FAKETIME_DATETIME="$(git log -1 --date=iso --format=%cd)"
    with_faketime () {
        showargs faketime -f "${FAKETIME_DATETIME}" "$@"
        faketime -f "${FAKETIME_DATETIME}" "$@"
    }
else
    FAKETIME=""
    SOURCE_DATE_EPOCH=""
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

# WebP compilation
WEBP_INSTALL_DIR="objs/webp/install/"
mkdir -p ${WEBP_INSTALL_DIR}  # Needed for realpath
WEBP_INSTALL_DIR_FULL="$(realpath ${WEBP_INSTALL_DIR})"
WEBP_BUILD_DIR="objs/webp/build/"

# Win32 references
# Sadly, the library paths must be resolved *now* already.
# AND, they must be absolute.  Sigh.
mkdir -p win32
WIN32_GTK_DEV_DIR=$(realpath win32/gtk+-bundle_2.24.10-20120208_win32)
WIN32_PIDGIN_DIR=$(realpath win32/pidgin-2.13.0)
WIN32_WEBP_PRISTINE="win32/libwebp-1.0.2/"

# Versioning information
./configure -q
make commit.h
VERSION=$(grep -E 'PACKAGE_VERSION' config.h | sed -re 's/^.*"(.*)".*$/\1/')
COMMIT=$(grep -E 'define' commit.h | sed -re 's/^.*"(.*)".*$/\1/')


# === Ensure that all tools are available. ===

# Commands
for cmd in faketime file make makensis "${MINGW_TARGET}-gcc" realpath sed tar unzip ; do
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
    (
        cd "${WEBP_BUILD_DIR}"
        # ./autogen.sh && ./configure && make

        ./autogen.sh

        WEBP_LDFLAGS=""
        WEBP_CFLAGS="-g -O2 -pthread"  # Default CFLAGS.  Why can't I just add some?
        if [ "y" = "${USE_REPRODUCIBLE}" ]
        then
            # Check if -fstack-protector-strong is supported before enabling it
            WEBP_CC=i686-w64-mingw32-gcc
            if ${WEBP_CC} -fstack-protector-strong 2>&1 | grep -q 'stack-protector-strong'
            then
                true  # Don't add flag: not supported.
            else
                WEBP_CFLAGS="${WEBP_CFLAGS} -fstack-protector-strong"
            fi
            # Check if -frandom-seed is supported before enabling it
            if ${WEBP_CC} -frandom-seed=77e0418a98676b76729b50fe91cc1f250c14fd8f664f8430649487a6f918926d 2>&1 | grep -q 'random-seed'
            then
                true  # Don't add flag: not supported.
            else
                WEBP_CFLAGS="${WEBP_CFLAGS} "'-frandom-seed=0x$$(sha256sum $< | cut -f1 -d" ")'
            fi
            # Check if -ffile-prefix-map is supported before enabling it
            if ${WEBP_CC} -ffile-prefix-map=/foo/bar/baz=/quux 2>&1 | grep -q 'file-prefix-map'
            then
                true  # Don't add flag: not supported.
            else
                WEBP_CFLAGS="${WEBP_CFLAGS} -ffile-prefix-map=$(realpath .)=webp"
            fi
        fi

        # Disable linking against PNG, JPEG, TIFF, GIF, WIC,
        # as those would either need cross-compilation, too, or some other magic.
        # libtoolize (called by autoreconf, called by autogen.sh) must not see
        # `install-sh` because it would become confused by it.
        # That's why the hierarchy is so deep.
        ./configure -q --build ${HOST} --host ${MINGW_TARGET} --target ${MINGW_TARGET} \
            --disable-dependency-tracking --prefix="${WEBP_INSTALL_DIR_FULL}/" \
            --disable-static --enable-shared \
            --enable-swap-16bit-csp \
            --disable-libwebpmux --disable-libwebpdemux \
            --disable-libwebpdecoder --disable-libwebpextras \
            --disable-png --disable-jpeg --disable-tiff --disable-gif --disable-wic
        # The warning
        # "configure: WARNING: using cross tools not prefixed with host triplet"
        # stems from the fact that 'mt' (magnetic tape control) is not available
        # for i686-w64-mingw32, so the x86_64 version is used.  We can ignore that.

        # For some reason, "--no-insert-timestamp" doesn't seem to work.
        # To make the timestamp in the final DLL reproducible, use the time of
        # the last commit instead.
        ${FAKETIME} make --quiet -j4 CFLAGS="${WEBP_CFLAGS}" LDFLAGS="${WEBP_LDFLAGS}" install
    )
    if [ ! -r ${WEBP_INSTALL_DIR}/include/webp/decode.h ] || [ ! -r ${WEBP_INSTALL_DIR}/bin/libwebp-7.dll ] ; then
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
if [ "y" = "${USE_PNG}" ]
then
    # Library should already be present in win32/gtk+-bundle_2.24.10-20120208_win32/lib
    CONFFLAGS_PNG="--enable-libpng"
fi
# pkg-config is used to find the right directories for icons, library, etc on linux.
# For Windows, this is already hardcoded in telegram-purple.nsi, so pkg-config isn't needed.
PKG_CONFIG=/bin/false ./configure -q --build ${HOST} --host ${MINGW_TARGET} --target ${MINGW_TARGET} \
    --disable-libpng --disable-libwebp \
    --with-zlib="${MINGW_BASE}" \
    PURPLE_CFLAGS="\${WIN32_INC}" \
    PURPLE_LIBS="-lpurple -lglib-2.0" \
    LDFLAGS="-L${WIN32_GTK_DEV_DIR}/lib -L${WIN32_PIDGIN_DIR}-win32bin ${LDFLAGS_WEBP}" \
    LIBS="-lssp -lintl -lws2_32" \
    ${CONFFLAGS_WEBP} ${CONFFLAGS_PNG}
(
    cd tgl
    ./configure -q --build ${HOST} --host ${MINGW_TARGET} --target ${MINGW_TARGET} \
        --disable-openssl --disable-extf \
        --with-zlib="${MINGW_BASE}" \
        LIBS="-lssp"
)

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
    COMMA_VERSION=$(grep -E 'PACKAGE_VERSION' config.h | sed -re 's/^.*"(.*)".*$/\1/' -e 's/\./,/g')
    sed -i -re "s/MAGIC_SED_VERSION/${COMMA_VERSION}/" objs/info.rc
    ${MINGW_TARGET}-windres objs/info.rc -O coff -o objs/info.res
    VERSIONINFO_OBJECTS="objs/info.res"
fi
# CFLAGS: Remove LOCALEDIR definition.
# PRPL_NAME: Assign Windows-specific name.
#            (Baked into the file, so we can't just rename it.)
# LDFLAGS: Remove -rdynamic flag.

${FAKETIME} make -j4 bin/libtelegram.dll \
    CFLAGS_INTL="-DENABLE_NLS -DSOURCE_DATE_EPOCH=${SOURCE_DATE_EPOCH}" \
    PRPL_NAME=libtelegram.dll \
    EXTRA_OBJECTS="${VERSIONINFO_OBJECTS}" \
    LDFLAGS_EXTRA=-ggdb

# Package it up
echo "===== 10: Create installer"
make PRPL_NAME=libtelegram.dll win-installer-deps
VARIANT_SUFFIX=""
if [ "y" != "${USE_PNG}" ]
then
    VARIANT_SUFFIX="${VARIANT_SUFFIX}_nopng"
fi
if [ "y" != "${USE_WEBP}" ]
then
    VARIANT_SUFFIX="${VARIANT_SUFFIX}_nowebp"
fi
if [ "y" != "${USE_REPRODUCIBLE}" ]
then
    VARIANT_SUFFIX="${VARIANT_SUFFIX}_norepro"

    # makensis packages the file mtimes.  I don't think this can be changed,
    # and the project seems too slow to ever introduce a feature like that.
    # Therefore, we need to "fix" the mtimes.
    # The following files cannot be modified:
    #     ${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico
    #     ${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico
    # But thankfully, Debian's package system includes mtime,
    # so the time stamps are set by the pyckage version of nsis.
    touch --no-create --date FAKETIME_DATETIME \
        bin/libtelegram.dll contrib/libgcc_s_sjlj-1.dll contrib/libgpg-error-0.dll \
        contrib/libgcrypt-20.dll contrib/libwebp-7.dll po/*.mo COPYING imgs/*
fi
if [ "i686" != "${MINGW_MACHINE}" ]
then
    VARIANT_SUFFIX="${VARIANT_SUFFIX}_${MINGW_MACHINE}"
fi

${FAKETIME} makensis -DPLUGIN_VERSION="${VERSION}+g${COMMIT}${VARIANT_SUFFIX}" -DPRPL_NAME=libtelegram.dll \
    -DWIN32_DEV_TOP=contrib telegram-purple.nsi

# There's no monster under your bed, I swear.
echo "===== 11: Unspoof files"
# Stealth cleanup
( cd tgl && git checkout tl-parser/tl-parser.c tl-parser/tlc.c )

echo "===== COMPLETE: All done.  Installer executable is in top directory."
