telegram-purple
===============

Telegram-purple is a Libpurple plugin that adds support for the Telegram messenger. Its still in an early (pre-alpha) development stage, but already provides basic chat functionality and group chats.

This plugin is based on the great [Telegram-cli](http://github.com/vysheng/tg), a full-featured terminal-based client for Telegram created by [Vysheng](http://github.com/vysheng).

# Features

## Already Implemented

    - Group chats
    - Multiple accounts at once
    - Support for libpurple proxy settings

## Planned

We are trying to create a full-featured protocol plugin for Telegram, that can make use of all major features of 
the Messenger. The following features are currently under active development (or planned) and will probably be 
added in the future:

    - encrypted chats
    - picture, audio and video messages
    - file transfers
    - geo-locations
    - emojis 


# Installation Instructions

Unfortunately there are currently no packages, so you need to compile it yourself:

1. Get this repository either from Bitbucket or from the Github mirror.

        git clone https://bitbucket.org/telegrampurple/telegram-purple



2. Fetch all needed dependencies

This plugin depends on a working libpurple client and the following packages:

        - glib-2.0
        - libcrypto
        - libpurple
        - libzlib


On Fedora you can install all dependencies with:

        sudo yum install gcc openssl-devel glib2-devel libpurple-devel


And on Debian/Ubuntu you can use:

        sudo apt-get install libssl-dev libglib2.0-dev libpurple-dev


3. Compile and install

        ./configure
        make
        sudo make install


# Usage

After succesfully completing all steps mentioned in the installation instructions, you should restart Pidgin to ensure that the plugin is loaded. When everything went well, Telegram should show up in the account manager:

![Create a new Telegram account](lauschgift.org/telegram-purple/res/install-1.png)

The username is your current phone number, including your full country prefix instead of a leading '0'. For Germany, this would be '+49' for example. Telegram will verify your phone number by sending you a verification code via sms. You will be prompted for this code, once that happens.

![Provide a phone number](lauschgift.org/telegram-purple/res/install-2.png)
 
Now you should be able to see all your contacts and chats in your buddy list and send/receive messages.


# A Word of Warning

This is a very early (pre-alpha) version of the plugin, mainly used for development and testing and NOT for productive use. 
Even though it already provides basic features, you should stil expect bugs and crashes when running it.

When encountering a crash, please report it to us, preferably together with a backtrace [https://developer.pidgin.im/wiki/GetABacktrace]

