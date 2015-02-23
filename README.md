Telegram-Purple
===============

Telegram-purple is a Libpurple plugin that adds support for the Telegram messenger. Its still alpha and not ready for production use.

Build
-----

Below, you will find instructions of how to build the libpurple protocol plugin. If you just want to use the plugin in Adium, [download precompiled packages here.](https://github.com/majn/telegram-purple/releases)

#### 1. Clone

This repository has submodules, so you need to clone recursively


        git clone --recursive https://github.com/majn/telegram-purple


#### 2. Fetch all dependencies

##### Fedora

        sudo yum install gcc openssl-devel glib2-devel libpurple-devel


###### Debian

        sudo apt-get install libssl-dev libglib2.0-dev libpurple-dev


###### OpenSUSE

        sudo zypper install gcc glib glib-devel libpurple libpurple-devel zlib-devel openssl libopenssl-devel


#### 3. Compile and install

        ./configure
        make
        sudo make install


Usage
-----

#### First Login

The username is your current phone number, including your full country prefix. For Germany, this would be '+49', resulting in a user name like '+49151123456'. Telegram will verify your phone number by sending you a code via sms. You will be prompted for this code, once that happens.

#### Using secret chats

You can use Telegram secret chats with this plugin, they will show up as a new buddy with a '!' in front of the buddy name.

One caveat of secret chats in Telegram is that they can only have one endpoint, this is a limitation of the protocol. This means that if you create a secret chat in Pidgin you will not be able to use that chat on your phone. You will be asked whether to accept each secret chat, so you can always choose to accept the chat on a different device if you want. You can set a default behavior for dealing with secret chats (Accept or Decline) in the account settings if you don't want that prompt to appear every time.

Self destructive messages will be ignored, since I don't know any way to delete them from the conversation and the history.

##### Confirming the key authenticity

Click on the buddy in the buddy list and click on "Show Info" to visualize the key fingerprint.  

![Confirm key authenticity](http://h2079792.stratoserver.net/telegram-purple/res/key.png)

##### Initiate secret chats

To initiate a secret chat from Pidgin, click on a Buddy in the Buddy List and hit ``Start Secret Chat''

##### Deleting secret chats

If you delete a secret chat from the buddy list, it will be terminated and no longer be usable.


#### Unicode Emojis for Pidgin

The Telegram phone applications for iOS and Android make use of standardized Unicode smileys (called [Emojis](https://en.wikipedia.org/wiki/Emoji)). Pidgin
does not display those smileys natively, but you can install a custom smiley theme like (https://github.com/stv0g/unicode-emoji) or (https://github.com/VxJasonxV/emoji-for-pidgin) and activate it under Settings > Themes > Smiley Theme.


Troubleshooting
---------------

If you encounter problems running this plugin and you have updated from an older version,
deleting your old user-data might be helpful. WARNING: This will require you to enter a new authentication
code and delete all your secret chat keys.

To clean all your user files run:


        make purge



Building the Adium Plugin
-------------------------

Compiling with XCode is a little bit problematic, since it requries you to compile Adium first to get the necessary framework files. My advice is to just use the [prebuild bundle](https://github.com/majn/telegram-purple/releases), but if you really want to do it, follow those steps.

1. Get the Adium source, compile it with XCode and copy the build output into telegram-adium/Frameworks/Adium. It should contain at least Adium.framework, AdiumLibpurple.framework and AIUitilies.framework
2. Open the Adium source code, go to ./Frameworks and copy libglib.framework and libpurple.framework into telegram-adium/Frameworks/Adium
3. Build the tgl submodule and delete libtgl.so from libs/ (it should only contain libtgl.a)
4. Build the XCode-Project and execute the created bundle

Authors
-------

Telegram-Purple was written by:

    - Matthias Jentsch <mtthsjntsch@gmail.com>
    - Vitaly Valtman
    - Christopher Althaus <althaus.christopher@gmail.com>
    - Markus Endres <endresma45241@th-nuernberg.de>


Acknowledgements
----------------

This software is based on the library [Libtgl](https://github.com/vysheng/tgl), which was written by Vitaly Valtman <mail@vysheng.ru> and others, see (http://github.com/vysheng/tgl)

For PNG rendering, it includes the [lodepng library](http://lodev.org/lodepng/).

This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit. (http://www.openssl.org/)

This product includes cryptographic software written by Eric Young (eay@cryptsoft.com)

