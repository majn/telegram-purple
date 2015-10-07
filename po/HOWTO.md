Translation Howto
=================

To add the new language, append the language code LL to po/LINGUAS, and run autogen.sh. You 
rill be prompted for a contact email. The .po files for the language should then show up in ./po/LL.po


        echo LL >> po/LINGUAS
        ./autogen.sh


To update the translatable strings:


        ./autogen.sh


