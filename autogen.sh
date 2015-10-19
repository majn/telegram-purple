#!/bin/sh

autoreconf

echo bootstrapping translation files ...
cd po
XGETTEXT='xgettext -kP_:1,2' intltool-update --pot
## Translations are managed at https://www.transifex.com/telegram-purple-developers/telegram-purple/
## To update the .po files, download it from there, since intltool, msginit, and transifex produce slightly different files, and I'd like to avoid gigantic git diffs that only change indentation or similar things.
#for lang in $(cat LINGUAS); do
#    if [ -e "$lang.po" ]
#    then
#        echo "updating language file $lang.po ..."
#        intltool-update "$lang"
#    else
#        echo "creating new language file $lang.po ..."
#        msginit --locale="$lang"
#    fi
#done
cd ..
