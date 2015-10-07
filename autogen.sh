autoconf
autoheader

echo bootstrapping translation files ...
cd po
intltool-update --pot
for lang in $(cat LINGUAS); do
    if [ -e "$lang.po" ]
    then
        echo "updating language file $lang.po ..."
        intltool-update "$lang"
    else
        echo "creating new language file $lang.po ..."
        msginit --locale="$lang"
    fi
done
cd ..

