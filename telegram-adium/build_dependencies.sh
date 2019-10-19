#!/usr/bin/env sh
# Build libgpg-error and libgcrypt dependencies for telegram-adium

set -ex

LIBGPG_ERROR_VERSION="1.36"
LIBGCRYPT_VERSION="1.8.5"

# Start relative to the location of build_dependencies.sh
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
cd "$SCRIPTPATH"

mkdir -p Frameworks/dependencies
cd Frameworks/dependencies

export CFLAGS="-mmacosx-version-min=10.11"

if [[ ! -f "libgpg-error-$LIBGPG_ERROR_VERSION.tar.bz2" ]]; then
    curl -O "https://gnupg.org/ftp/gcrypt/libgpg-error/libgpg-error-$LIBGPG_ERROR_VERSION.tar.bz2"
fi

if [[ ! -f "libgcrypt-$LIBGCRYPT_VERSION.tar.bz2" ]]; then
    curl -O "https://gnupg.org/ftp/gcrypt/libgcrypt/libgcrypt-$LIBGCRYPT_VERSION.tar.bz2"
fi

tar -xjf "libgpg-error-$LIBGPG_ERROR_VERSION.tar.bz2"
tar -xjf "libgcrypt-$LIBGCRYPT_VERSION.tar.bz2"

# Build libgpg-error
pushd "libgpg-error-$LIBGPG_ERROR_VERSION"
./configure --disable-dependency-tracking --disable-silent-rules --prefix="$(pwd)/../libgpg-error-install"
make install -j9
popd

# Build libgcrypt
pushd "libgcrypt-$LIBGCRYPT_VERSION"
./configure --disable-dependency-tracking --disable-silent-rules --disable-asm --disable-jent-support --with-libgpg-error-prefix="$(pwd)/../libgpg-error-install" --prefix="$(pwd)/../libgcrypt-install"
make install -j9
popd

# Copy libraries into correct location and set loader paths
mkdir -p ../Adium
cp libgpg-error-install/lib/libgpg-error.0.dylib ../Adium
cp libgcrypt-install/lib/libgcrypt.20.dylib ../Adium

install_name_tool -id "@loader_path/../Resources/libgpg-error.0.dylib" ../Adium/libgpg-error.0.dylib
install_name_tool -id "@loader_path/../Resources/libgcrypt.20.dylib" ../Adium/libgcrypt.20.dylib
install_name_tool -change "$(pwd)/libgpg-error-$LIBGPG_ERROR_VERSION/../libgpg-error-install/lib/libgpg-error.0.dylib" "@loader_path/../Resources/libgpg-error.0.dylib" ../Adium/libgcrypt.20.dylib

