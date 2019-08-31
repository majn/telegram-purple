# Contributing

This is Open Source Software. We realized that we didn't pass the Patch Test

## What to contribute

Here's a few places to start:

- Report any bugs you find.
- No, seriously! Most of the bugs don't even get reported, or the reporter only writes "Doesn't work for me" and then vanishes.
- ["help wanted" issues](https://github.com/majn/telegram-purple/issues?utf8=%E2%9C%93&q=is%3Aopen+is%3Aissue+label%3A%22help+wanted%22+-label%3A%22in-pipeline%22), especially automated testing
- ["secret chat" issues](https://github.com/majn/telegram-purple/issues?utf8=%E2%9C%93&q=is%3Aopen+is%3Aissue+label%3A%22secret+chats%22+-label%3A%22in-pipeline%22+)
- Portability (especially endianness) of tgl (external project)
- [Issues of tgl](https://github.com/vysheng/tgl/issues) -- lots and lots of false or obsolete reports
- Slim down the size of the generated files in `tgl/auto`, as this currently dominates the build time. Yuk!
- Make sure that the libpurple-devs don't break libpurple any further.

## How to contribute

Write [an issue](https://github.com/majn/telegram-purple/issues/new), shoot us a mail (scrape the address from the git history), or contact us in dev chat (changing URL, see README for that).

## How to release

#### Translations

Make sure that the string freeze has already happened.

Go to the [translation site](https://www.transifex.com/telegram-purple-developers/telegram-purple/content/),
and close the current resource for new translations.  Rename and re-prioritize it appropriately.
Download the zip-file, extract it, remove the annoying prefix.  Move all files to `po/`, except `en.po`.

While waiting for the download link, create the new resource with the new potfile,
then re-upload all the old translations there.

Run `make po/*.po` to fix the filenames.  Do not delete any files.  Rationale see `Makefile`.

Adjust `po/LINGUAS*` as you see fit.

Also import the translations for `AppStream/po/`.

#### Increment version

Attempt to use semver.

Adjust `configure.ac`, in macro `AC_INIT`.  Remember to run `autoreconf`.

Adjust `telegram-adium/telegram-adium/telegram-adium-Info.plist`.

Adjust `rpm/purple-telegram.spec`, in version at the top and "changelog" below.

Don't update debian: It's abandoned anyway.

AppStream: Nothing to do.

Update `CHANGELOG.md`.

#### Run some tests

First locally, then on Travis.
Also, install it locally.  Don't forget to `./configure` first.

#### Prepare release files

Merge into `master`.

Make a tag: `git tag v1.something`
Remember to push the tag to github.

Make a dist-bundle: `make dist`
Keep the resulting `telegram-purple_1.something.orig.tar.gz` safe.

Make the windows installer: `./mkwindows.sh`
Keep the resulting `telegram-purple-1.something+gcafebabe5.exe` safe.

#### Create release on github

See the [release page](https://github.com/majn/telegram-purple/releases/).
Feel free to be inspired by the previous release-messages.

Remember to upload the origtar *and* Windows installer.

#### Close associated issues

All relevant issues are marked with `in-pipeline`, but sadly too many are marked this way.

#### Organize branches

Create a new branch `dev-1.nextsomething`, delete the old branch.

#### Create new translation goal on transifex

Be pessimistic in the slug-name.

Run `./autogen.sh` to make all string changes available.

Also, create an announcement that yes there is a new resource,
but this resource may change significantly until the next string freeze.
