Telegram-Purple
===============

Telegram-purple is a Libpurple plugin that adds support for the Telegram messenger. Its still in an early (pre-alpha) development stage, but already provides basic chat functionality and group chats.

This plugin is based on [Telegram-cli](http://github.com/vysheng/tg), a full-featured terminal-based client for Telegram created by [Vysheng](http://github.com/vysheng).

# Changelog

Warning, this version is mainly for development and testing and NOT for productive use. Even though it already provides basic features, you should still expect bugs and crashes when running it.

When encountering a crash or some other bugs, please report it to us, preferably together with a backtrace of the crashed application [https://developer.pidgin.im/wiki/GetABacktrace]

## Version 0.5

    - Display incoming photos
    - Respect received user and chat property updates
    - Support changing own profile picture
    - Support adding new contacts
    - Display service messages


## Version 0.4
    
    - Use latest version of libtgl


## Version 0.3.3

    - multiple bugfixes


## Version 0.3.2

    - Add compatibility for OSX 10.8


## Version 0.3.1

    - Improve error handling
    - Fix some installation problems


## Version 0.3.0

    - Provide Adium Plugin


# Features

## Already Implemented:

    - Chats/Group-Chats
        * Send/receive messages
        * Discover buddies/chats
        * Discover buddy state and info
    - Profile Pictures
        * Download and use profile pictures
    - Adium Plugin

## TODO:

The following features are currently planned and will probably be added in the future:

    - Encrypted chats
    - Picture, audio and video messages
    - File transfers
    - Geo-locations
    - Multiple accounts on one client
    - Respect libpurple proxy settings (implemented but untested)


## Platform Support

We only provide an installation guide for Linux right now, even though it should be possible to compile this plugin on other platforms. If someone manages to create a working Windows or BSD-build please let us know.


# Installation Instructions

Unfortunately there are no packages right now, so you need to compile it yourself:

## 1. Get this repository


        git clone --recursive https://github.com/majn/telegram-purple

## 2. Fetch all needed dependencies

This plugin depends on a working libpurple client (like Pidgin or Finch) and the following packages:

        - glib-2.0
        - libssl
        - libpurple
        - libzlib

### Fedora

On Fedora you can install all dependencies with:

        sudo yum install gcc openssl-devel glib2-devel libpurple-devel

### Debian

On Debian-based systems you can use:

        sudo apt-get install libssl-dev libglib2.0-dev libpurple-dev



### OpenSUSE

        
        sudo zypper install gcc glib glib-devel libpurple libpurple-devel zlib-devel openssl libopenssl-devel


## 3. Compile and install


        make
        sudo make install


# Usage

After succesfully completing all steps mentioned in the installation instructions, you should restart Pidgin to ensure that the plugin is loaded. When everything went well, Telegram should show up in the account manager:

![Create a new Telegram account](http://h2079792.stratoserver.net/telegram-purple/res/install-1.png)

The username is your current phone number, including your full country prefix instead of a leading '0'. For Germany, this would be '+49' for example. Telegram will verify your phone number by sending you a verification code via sms. You will be prompted for this code, once that happens.

![Provide a phone number](http://lauschgift.org/telegram-purple/res/install-2.png)
 
Now you should be able to see all your contacts and chats in your buddy list and send/receive messages.


# Troubleshooting

If you encounter problems running this plugin and you have updated from an older version,
deleting your old user-data might be helpful. WARNING: This will require you to enter a new authentication
code and delete all your secret chat keys.

To clean all your user files run:


        sudo make purge



# Adium Plugin

## Prebuilt Bundle

This bundle was tested to work under OSX 10.9 and 10.8. If it doesn't work on your installation
please send your Adium crash log (which you can find in ~/Library/Logs/Adium 2/).

[Version 0.3.2](http://h2079792.stratoserver.net/telegram-purple/telegram-adium-0.3.2.zip)

[Version 0.3.3](http://h2079792.stratoserver.net/telegram-purple/telegram-adium-0.3.3.zip)

## Build with XCode

1. Compile the source of your current Adium version and add the created frameworks to the Adium-Telegram build path.
2. Get zlib and libcrypto.a and provide it somewhere in your build path.
3. Build the XCode-Project and execute the created bundle


# Unicode Emojis for Pidgin

The Telegram phone applications for iOS and Android make use of standardized Unicode smileys (called [Emojis](https://en.wikipedia.org/wiki/Emoji)). Pidgin
does not display those smileys natively, but you can install a custom smiley theme like (https://github.com/stv0g/unicode-emoji) or (https://github.com/VxJasonxV/emoji-for-pidgin) and activate it under Settings > Themes > Smiley Theme.


# Authors

Telegram-Purple and Telegram-Adium were written by:

    - Matthias Jentsch <mtthsjntsch@gmail.com>
    - Christopher Althaus <althaus.christopher@gmail.com>
    - Markus Endres <endresma45241@th-nuernberg.de>
    - Vitaly Valtman


The plugin is based on Telegram-CLI, which was written by Vitaly Valtman <mail@vysheng.ru> and others, see (http://github.com/vysheng/tg)

