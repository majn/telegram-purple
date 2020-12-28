Changelog
---------

##### 1.4.6

IMPORTANT: This repository barely sees any updates anymore.
You should switch to [tdlib-purple](https://github.com/ars3niy/tdlib-purple/),
which will hopefully become telegram-purple's successor.
Note that only the following features are missing:
- Retrieve history on first login (not everyone needs this)
- Add options to ignore certain groups (why use a messenger if you don't want to ever see those messeges?!)
- Turn off sending read receipts: https://github.com/ars3niy/tdlib-purple/issues/22
So it's a good replacement in many cases already.
For more details, see https://github.com/ars3niy/tdlib-purple/milestone/1

The actual changes in this version are only minor stuff:
- Fixed minor details required to build telegram-adium (developmentRegion and no arm64)
- Updated dependencies for newer MinGW versions
- Updated dependencies for telegram-adium
- Updated documentation and links

##### 1.4.5

IMPORTANT: This repository barely sees any updates anymore.
You should switch to [tdlib-purple](https://github.com/ars3niy/tdlib-purple/),
which will hopefully become telegram-purple's successor.
Note that only the following features are missing:
- Retrieve history on first login (not everyone needs this)
- Add options to ignore certain groups (why use a messenger if you don't want to ever see those messeges?!)
- Turn off sending read receipts: https://github.com/ars3niy/tdlib-purple/issues/22
So it's a good replacement in many cases already.
For more details, see https://github.com/ars3niy/tdlib-purple/milestone/1

The actual changes in this version are only minor stuff:
- New feature: Enable setting the download URI base; this seems to be interesting for bitlbee users.
- Improvement: Update and improve searching for zlib (ax_check_zlib.m4)
- Fix: `sed` invocation on macOS
- Fix: Cross-reference to telepathy-morse.
- Improvement: Automatically build on ppc64le
  Caveat emptor! We only *build* there. I don't have access to any such hardware,
  telegram-purple has no real tests, and I know nothing about ppc64le.
  Bugfixes and improvements are welcome.

##### 1.4.4

IMPORTANT: This repository barely sees any updates anymore.
You may want to switch to [tdlib-purple](https://github.com/ars3niy/tdlib-purple/),
which will hopefully become telegram-purple's successor.
Note that some features like secret chats are missing,
so it's not a good replacement yet.

- Always display the system account 'Telegram' as online.
- Adium: Lots of work, thanks to Kent Sutherland!
- Fix some typos and build issues
- AppStream: use new destination directory (which means we're not compatible with old AppStream versions!)

##### 1.4.3

- Bitlbee: Write files to /tmp instead (See #513)
- Windows: Automatically adapt installer filename
- Binaries should be "somewhat" reproducible now
- Update translations.  Thanks to the following people:
    * oɹʇuʞ <kntro@msn.com> (es_AR)
    * Yngve Spjeld-Landro <l10n@landro.net> (nn)
    * Piotr Drąg <piotrdrag@gmail.com> (pl_PL)
    * Denis Brandl <denisbr@gmail.com> (pt_BR)
    * Besnik <besnik@programeshqip.org> (sq)
    * Самохвалов Антон <samant.ua@mail.ru> (uk)
- Fix: Don't crash on immediately-finished transfers (See #530)
    Thanks Vladimir Panteleev <git@thecybershadow.net>!
- Fix: Don't crash on photos with geolocation (See https://github.com/majn/tgl/pull/30 )
    Thanks to himselfv <me@boku.ru> !

##### 1.4.2

- Still no support for Adium.  We just don't know how!  Help wanted!  PLEASE!
- Update Windows dependencies
- Support ancient glib (See #501)
- Update translations.  Thanks to the following people:
    * Eduardo Trápani <etrapani@gmail.com> (es)
    * Olesya Gerasimenko <gammaray@basealt.ru> (ru)
- Fix: Handle forwarded messages and captioned images better
- Fix: Handle replies better
- Fix: Images in own messages
- Fix: Remove *some* of the duplicate messages seen (Maybe fixes #258?)

##### 1.4.1

- *REMOVED* active support for Adium.  We just don't know how!  Help wanted!
- Had to skip 1.4.0 as someone distributed an intermediate version as "1.4.0"
- Import awesomeness from kenorb's mega-merge:
    * Support for pinned and other action types.
    * Stability improvements in general
- Better build support:
    * No `-Werror`
    * Compatibility with LibreSSL
    * Compatibility with C++
- Simplify: Only one level of submodules
- Add support for replies/forwarded messages
- Add support for IPv6-only
- Add windows cross-compilation script

##### 1.3.1

- Stability improvements
- Support auto-loading for documents
- Add support for "typing" in group chats (using group-typing-notifications plugin)
- Add support for /kick command in group chats

##### 1.3.0

- channel/supergroup support
- support sending code tags in markdown "backtick" format (see README for example)
- reduce amount of file-transfer popups in Pidgin, auto-load media in the background
- fix stability issues for the win32 build
- fix multiple crashes in libtgl

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

