/**
 * @file purple.h  Header files and defines
 * This file contains all the necessary preprocessor directives to include
 * libpurple's headers and other preprocessor directives required for plugins
 * or UIs to build.  Including this file eliminates the need to directly
 * include any other libpurple files.
 *
 * @ingroup core libpurple
 * @since 2.3.0
 */

/* purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#ifndef _PURPLE_PURPLE_H_
#define _PURPLE_PURPLE_H_

#include <glib.h>

#ifndef G_GNUC_NULL_TERMINATED
#	if     __GNUC__ >= 4
#		define G_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
#	else
#		define G_GNUC_NULL_TERMINATED
#	endif
#endif

#undef PURPLE_PLUGINS

#include <account.h>
#include <accountopt.h>
#include <blist.h>
#include <buddyicon.h>
#include <certificate.h>
#include <cipher.h>
#include <circbuffer.h>
#include <cmds.h>
#include <connection.h>
#include <conversation.h>
#include <core.h>
#include <debug.h>
#include <desktopitem.h>
#include <dnsquery.h>
#include <dnssrv.h>
#include <eventloop.h>
#include <ft.h>
#include <idle.h>
#include <imgstore.h>
#include <log.h>
#include <media.h>
#include <mediamanager.h>
#include <mime.h>
#include <nat-pmp.h>
#include <network.h>
#include <notify.h>
#include <ntlm.h>
#include <plugin.h>
#include <pluginpref.h>
#include <pounce.h>
#include <prefs.h>
#include <privacy.h>
#include <proxy.h>
#include <prpl.h>
#include <request.h>
#include <roomlist.h>
#include <savedstatuses.h>
#include <server.h>
#include <signals.h>
#include <smiley.h>
#include <sound.h>
#include <sound-theme.h>
#include <sound-theme-loader.h>
#include <sslconn.h>
#include <status.h>
#include <stringref.h>
#include <stun.h>
#include <theme.h>
#include <theme-loader.h>
#include <theme-manager.h>
#include <upnp.h>
#include <util.h>
#include <value.h>
#include <version.h>
#include <whiteboard.h>
#include <xmlnode.h>

#endif
