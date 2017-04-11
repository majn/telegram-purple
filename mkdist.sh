#!/bin/bash

do_fail()
{
    echo "Failure: $*"
    exit 1
}

test -d .git || do_fail "This is not a valid git tree"

TARBALL_NAME=`basename $(pwd)`
THIS=`basename ${0}`

git submodule init || do_fail "Cannot init submodules"
git submodule update --init --recursive || do_fail "Cannot update submodules"

GIT_TAG=`git describe --tags --abbrev=0|sed 's/^v//'`
if [[ $? -ne 0 || -z "${GIT_TAG}" ]]; then
    do_fail "Cannot find current tag"
fi

echo "Creating tarball for ${GIT_TAG}"

TGT_DIR="./${TARBALL_NAME}-${GIT_TAG}"

if [[ -d "${TGT_DIR}" ]]; then
    rm -rvf "${TGT_DIR}" || do_fail "Unable to remove ${TGT_DIR}"
fi

mkdir "${TGT_DIR}" || do_fail "Cannot create required directory"

# Copy dir structure excluding top git
find . -maxdepth 1 -type d -not -iname .git -not -iname .|xargs -I{} cp -Rpv "{}" "${TGT_DIR}/."
# Copy all files in here excluding this script
find . -maxdepth 1 -type f -not -iname "${THIS}" |xargs -I{} cp -pv "{}" "${TGT_DIR}/."

# Remove the unnecessary submodule gits inside the tarball tree
find "${TGT_DIR}" -name .git |xargs -I{} rm -rfv {}

XZ_OPT="-9" tar cJf "${TARBALL_NAME}-${GIT_TAG}.tar.xz" "${TGT_DIR}"
