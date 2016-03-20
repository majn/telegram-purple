Changelog
---------

##### 1.2.6

- Include libtgl bugfix "use-after-not-created for photos"
- Fix Adium bundle version incompatibillity for newest version

##### 1.2.5

- Include libtgl fix to prevent crashes on certain channel messages
- Improve error handling to be more graceful

##### 1.2.4

- Fix crash on unknown media type
- Fix crash on logout after cancelling a file transfer

##### 1.2.3

- Build: Allow compilation on Windows #52 Thanks Eion!
- Build: Drop dependency on LodePNG, Thanks Ben!
- Build: Gettext is now optional

- Fix issue that prevented to send messages to deleted users in certain cases (#174)
- Fix own user being added to the buddy list in certain cases
- Fix that read recipes of own messages are being displayed (#139)
- Fix encoding inconsistencies with Unicode characters (#177)
- Fix auto-joining for chats (#179)
- Fix client not reconnecting anymore under certain circumstances (#173)
- Fix crash on compat-verification (PullRequest #183)

- Remove pointless "create chat" confirmation dialogue
- Improve logging messages
- Always send read recipes when the user is typing or sending a message
- Improve translation and user messages (#139)
- Use native password prompts (Adium)


##### 1.2.2

    - fix some licensing issues by not depending on OpenSSL any longer (thanks Ben!)
    - add tons of translations (thanks to anyone helping!)
    - fix issue that caused empty files
    - fix pidgin not reconnecting after hibernation
    - fix adium not reconnecting after hibernation
    - fix secret chat fingerprint not being displayed after restart
    - fix secret chat name not being displayed after restart

##### 1.2.1

##### Bugfixes

    - Fix buggy group chat handling
      * Assure that foreign users are always listed by their full name instead of the user ID
      * Don't automatically add group chats to buddy list to prevent incompatibillities with some libpurple clients
      * Fix issue that caused the buddy list to not be correctly updated on joining / leaving participants
      * Fix an issue that caused the user list to be printed on every group chat message in Finch

    - Fetch chat information lazily to reduce traffic and prevent flood errors
    - Improve input validation for phone numbers

###### Features

    - Support leaving chats
    - Implement the roomlist-API to allow listing of all active chats


##### Version 1.2.0 (Beta 3)

##### Bugfixes

    - Fix error in incoming message handling that caused segmentation faults
    - Fix failing audio and video transfers
    - Mark messages read on other devices as 'deferred' to avoid needless notifications
    - Improved buddy list handling (should avoid "unknown" contacts popping up)
    - Many small issues (see commit history)
    - Fix issues that caused audio and video downloads to not have a proper file ending
    - Fix issue that caused very old unused chats to not be available

###### Features

    - Allow creation of new group chats
    - Allow joining chats by invite link
    - Allow exporting chats by invite link
    - Support displaying and sending GIFs 
    - Support joining chats by invite link

##### Version 1.1.0 (Beta 2)

    - update to libtgl 2
    - support two-factor authentication
    - improve sticker support
    - add option to control message read recipes
    - many bugfixes


##### Version 1.0.0 (Beta 1)

    - Move state files into .purple/telegram-purple directory

    - Add extended account settings for controlling history and read notifications

    - Improved support for Adium
        * fix chat bookmarking
        * add custom views for account settings and chat joining


##### Version 0.7

    - Support uploads and downloads (with some caveats though):
         * Uploads wont contain the current file name but a generic one
         * Every file is uploaded as a document or picture, so unfortunately no embedded videos and audio (right now)
         * Group chats only support picture uploads since nothing else is supported by the UI

    - Fix HTML escaping issues

##### Version 0.6.1

    - Fix many stability issues
    - Do not readd left chats on login

##### Version 0.6

    - Support for secret chats 
    - Receiving geo messages

##### Version 0.5

    - Display incoming photos
    - Respect received user and chat property updates
    - Support changing own profile picture
    - Support adding new contacts
    - Display service messages
    - Works with libpurple proxy settings

##### TODO:

    - Picture Uploads
    - Audio, Video and File Transfers

