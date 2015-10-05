#!/bin/sh

# git-buildpackage doesn't understand submodules, and is unusable for this
# purpose. Specifically, we needed a different way of re-creating a .orig.tar.gz
# reproducibly, given only the SHA1 of the base commit.
# GitHub's functionality for that is nice, but not reproducible:
# I don't want to depend on GitHub internals, ad I don't know how they do it.
# "git-archive-all.sh" seems to fulfill all those needs, even though it is
# highly broken. But it works for now, and will be replaced in the future.

set -e

# Let's hope I didn't introduce any bashisms.
# TODO: This should probably be rewritten as a perl script.

# == Check state ==
DEBIAN_VERSION=`dpkg-parsechangelog --show-field=Version | sed -r s/-[0-9]+\$//`
RAW_TAG=`echo v${DEBIAN_VERSION} | sed 's/~/-/'`
echo "Debian version seems to be ${DEBIAN_VERSION}. Expecting a tag ${RAW_TAG}"
PARSED_REV=`git rev-parse "${RAW_TAG}" 2> /dev/null || true`
if [ "${RAW_TAG}" = "${PARSED_REV}" ]
then
  echo "That tag doesn't exist."
  exit 1
fi
echo "Resolved to ${PARSED_REV}"

# == Do it! ==
# Cleanup (if the last run didn't already)
rm -f /tmp/telegram-purple.tar.gz /tmp/tgl.tar.gz /tmp/tgl.tl-parser.tar.gz
# Actual archiving
debian/rawtar --format tar.gz --prefix telegram-purple/ --tree-ish ${PARSED_REV} \
 -- ../telegram-purple_${DEBIAN_VERSION}.orig.tar.gz
# Remove gzip timetamp:
dd if=/dev/zero of=../telegram-purple_${DEBIAN_VERSION}.orig.tar.gz \
 bs=1 seek=4 count=4 status=none conv=notrunc
# Note that 'faketime' is somehow unreliable(?): It produces sometimes, although
# rarely, a gzip with "last modified: Thu Jan  1 00:00:01 1970", even though
# time should be frozen, even though 'LC_ALL=C faketime "2015-01-01 00:00:00"'
# should *freeze* (not shift) time.
