/**
 * @file ntlm.h
 */

/* purple
 *
 * Copyright (C) 2005, Thomas Butter <butter@uni-mannheim.de>
 *
 * ntlm structs are taken from NTLM description on
 * http://www.innovation.ch/java/ntlm.html
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

#ifndef _PURPLE_NTLM_H
#define _PURPLE_NTLM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generates the base64 encoded type 1 message needed for NTLM authentication
 *
 * @param hostname Your hostname
 * @param domain The domain to authenticate to
 * @return base64 encoded string to send to the server.  This should
 *         be g_free'd by the caller.
 */
gchar *purple_ntlm_gen_type1(const gchar *hostname, const gchar *domain);

/**
 * Parses the ntlm type 2 message
 *
 * @param type2 String containing the base64 encoded type2 message
 * @param flags If not @c NULL, this will store the flags for the message
 *
 * @return The nonce for use in message type3.  This is a statically
 *         allocated 8 byte binary string.
 */
guint8 *purple_ntlm_parse_type2(const gchar *type2, guint32 *flags);

/**
 * Generates a type3 message
 *
 * @param username The username
 * @param passw The password
 * @param hostname The hostname
 * @param domain The domain to authenticate against
 * @param nonce The nonce returned by purple_ntlm_parse_type2
 * @param flags Pointer to the flags returned by purple_ntlm_parse_type2
 * @return A base64 encoded type3 message.  This should be g_free'd by
 *         the caller.
 */
gchar *purple_ntlm_gen_type3(const gchar *username, const gchar *passw, const gchar *hostname, const gchar *domain, const guint8 *nonce, guint32 *flags);

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_NTLM_H */
