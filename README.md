telegram-purple
===============


# Installieren

1. Sicherstellen dass Pidgin installiert ist.
2. Dieses Git-Repository klonen.


        git clone https://bitbucket.org/telegrampurple/telegram-purple


3. Im Ordner von **telegram-purple** make ausführen:


        cd telgram-purple
        ./configure --disable-liblua --disable-libconfig
        make objects
        cd purple-plugin
        make install


Das Protokoll Telegram sollte dann beim nächsten Start in der Accountverwaltung von Pidgin automatisch auftauchen.


# Testen und Debuggen

Um **telegram-purple** während der Entwicklung zu testen, ist es sinnvoll vorher die Berechtigungen des Plugin-Ordners zu ändern, damit beim Kopieren nicht jedes Mal Root-Rechte benötigt werden:


        sudo chmod 777 -R `pkg-config --variable=plugindir purple`
        sudo chmod 777 -R `pkg-config --variable=datarootdir purple`pixmaps/pidgin/protocol


## Testen


Zum Compilen, Testen und Ausgeben aller Debugnachrichten folgenden Befehl ausführen:


        make run


Falls die Lognachrichten nach kurzer Zeit nicht mehr angezeigt werden, und die Meldung "Wird geschlossen, da bereits ein andere libpurple-Client läuft" erscheint, die laufende Instanz von Pidgin mit folgendem Befehl beenden:


        sudo killall pidgin



### Filtern der Lognachrichten

Wenn Pidgin einfach mit **make run** ausgeführt wird, werden alle Debugnachrichten des gesamten Programms ausgegegeben. Libpurple verwendet interne Loggingfunktionen die den Lognachrichten automatisch einen Prefix hinzufügen, z.B. "plugins:" für Nachrichten des Pluginsloaders und "prpl-telegram:" für Nachrichten dieses Plugins.

Wenn man nur Ausgaben des Plugin-Loaders und von telegram-purple haben möchte kann man unnötige Nachrichten mit **grep** herausfiltern.


        make run | grep -i 'prpl-telegram:\|plugins:'


# Deinstallieren

Mit folgendem Befehl werden alle installierten Dateien wieder aus den Plugin-Ordnern gelöscht:


        sudo make uninstall



# Coding-Guidelines

## Coding Style

Wir wollen wenn möglichen den typischen Linux-C-Coding-Style verwenden. Bei Funktionen werden die Klammern auf der nächsten Zeile geschrieben, bei allem anderen auf der gleichen Zeile. Bei Funktionsargumenten zwischen Komma und dem nächsten Argument und nach Bedingungen von if/else-Blöcken immer ein Leerzeichen lassen.

        unsigned int some_function(int a, int b)
        {
            if (true) {
                // ...
            } else {
                // ...
            }
        }


## Logging

Wenn irgendeine Ausgabe gemacht wird, sollte das ausschließlich über die Libpurple Debugging-Funktionen passieren (Siehe die Anleitung zum Debuggen im [Libpurple-Howto](https://developer.pidgin.im/wiki/CHowTo/DebugAPIHowTo "Libpurple-HowTo")).


        #include "debug.h"
        #define PLUGIN_ID "prpl-telegram"

        // ...

         purple_debug_info(PLUGIN_ID, "Debugnachricht");


## GIT
git pull 	-> Stand aktualisieren
git add -A 	-> Files hinzufügen
git push 	-> Stand hochladen
git commit 	-> Commiten



## Troubleshooting

Zum löschen der angelegten Accounts
rm /home/USER/.purple/accounts.xml

Die lib muss eventuell nach /usr/lib/purple-2/telegram-purple.so kopiert werden


