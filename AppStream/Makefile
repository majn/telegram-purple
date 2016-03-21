# This file is part of telegram-purple
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
#
# Copyright Ben Wiederhake 2016

# This file is NOT meant for inclusion into the main Makefile.

LOCALES=$(shell cat po/LINGUAS)
LOCALE_MOS=$(addprefix po/,$(addsuffix .mo,$(LOCALES)))
# Hardcoded, since this should only be run on very few machines anyway,
# which are expected to have msgfmt on their $PATH
MSGFMT_PATH=msgfmt

po/%.mo: po/%.po
	${MSGFMT_PATH} -cf -o $@ $<

po/telegram-purple.metainfo.xml.pot: telegram-purple.metainfo.untranslated.xml as-metainfo.its
	itstool -i as-metainfo.its -o $@ $<

# CAREFUL WHEN EDITING!
# "-j raw.xml *.mo" must be the last options!
telegram-purple.metainfo.xml: as-metainfo.its telegram-purple.metainfo.untranslated.xml $(LOCALE_MOS) po/LINGUAS
	itstool -i as-metainfo.its -o $@ \
	        -j telegram-purple.metainfo.untranslated.xml $(LOCALE_MOS)

.PHONY: potfile
potfile: po/telegram-purple.metainfo.xml.pot

.PHONY: translate
translate: telegram-purple.metainfo.xml

all: translate
