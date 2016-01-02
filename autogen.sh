#!/bin/sh

set -e

autoreconf

(
cd po
export XGETTEXT="xgettext -kP_:1,2"
intltool-update --pot
## Translations are managed at https://www.transifex.com/telegram-purple-developers/telegram-purple/
## To update the .po files, download it from there, since intltool, msginit, and transifex produce slightly different files, and I'd like to avoid gigantic git diffs that only change indentation or similar things.
)

test -r Makefile || (echo "Autoreconf didn't create Makefile?!"; exit 1)
make --quiet build-nsi
