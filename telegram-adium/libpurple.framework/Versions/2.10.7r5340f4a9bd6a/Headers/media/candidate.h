/**
 * @file candidate.h Candidate for Media API
 * @ingroup core
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

#ifndef _PURPLE_MEDIA_CANDIDATE_H_
#define _PURPLE_MEDIA_CANDIDATE_H_

#include "enum-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define PURPLE_TYPE_MEDIA_CANDIDATE            (purple_media_candidate_get_type())
#define PURPLE_IS_MEDIA_CANDIDATE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), PURPLE_TYPE_MEDIA_CANDIDATE))
#define PURPLE_IS_MEDIA_CANDIDATE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PURPLE_TYPE_MEDIA_CANDIDATE))
#define PURPLE_MEDIA_CANDIDATE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), PURPLE_TYPE_MEDIA_CANDIDATE, PurpleMediaCandidate))
#define PURPLE_MEDIA_CANDIDATE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), PURPLE_TYPE_MEDIA_CANDIDATE, PurpleMediaCandidate))
#define PURPLE_MEDIA_CANDIDATE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PURPLE_TYPE_MEDIA_CANDIDATE, PurpleMediaCandidate))

/** An opaque structure representing a network candidate (IP Address and port pair). */
typedef struct _PurpleMediaCandidate PurpleMediaCandidate;

/**
 * Gets the type of the media candidate structure.
 *
 * @return The media canditate's GType
 *
 * @since 2.6.0
 */
GType purple_media_candidate_get_type(void);

/**
 * Creates a PurpleMediaCandidate instance.
 *
 * @param foundation The foundation of the candidate.
 * @param component_id The component this candidate is for.
 * @param type The type of candidate.
 * @param proto The protocol this component is for.
 * @param ip The IP address of this component.
 * @param port The network port.
 *
 * @return The newly created PurpleMediaCandidate instance.
 *
 * @since 2.6.0
 */
PurpleMediaCandidate *purple_media_candidate_new(
		const gchar *foundation, guint component_id,
		PurpleMediaCandidateType type,
		PurpleMediaNetworkProtocol proto,
		const gchar *ip, guint port);

/**
 * Copies a PurpleMediaCandidate.
 *
 * @param candidate The candidate to copy.
 *
 * @return The copy of the PurpleMediaCandidate.
 *
 * @since 2.7.0
 */
PurpleMediaCandidate *purple_media_candidate_copy(
		PurpleMediaCandidate *candidate);

/**
 * Copies a GList of PurpleMediaCandidate and its contents.
 *
 * @param candidates The list of candidates to be copied.
 *
 * @return The copy of the GList.
 *
 * @since 2.6.0
 */
GList *purple_media_candidate_list_copy(GList *candidates);

/**
 * Frees a GList of PurpleMediaCandidate and its contents.
 *
 * @param candidates The list of candidates to be freed.
 *
 * @since 2.6.0
 */
void purple_media_candidate_list_free(GList *candidates);

/**
 * Gets the foundation (identifier) from the candidate.
 *
 * @param candidate The candidate to get the foundation from.
 *
 * @return The foundation.
 *
 * @since 2.6.0
 */
gchar *purple_media_candidate_get_foundation(PurpleMediaCandidate *candidate);

/**
 * Gets the component id (rtp or rtcp)
 *
 * @param candidate The candidate to get the compnent id from.
 *
 * @return The component id.
 *
 * @since 2.6.0
 */
guint purple_media_candidate_get_component_id(PurpleMediaCandidate *candidate);

/**
 * Gets the IP address.
 *
 * @param candidate The candidate to get the IP address from.
 *
 * @return The IP address.
 *
 * @since 2.6.0
 */
gchar *purple_media_candidate_get_ip(PurpleMediaCandidate *candidate);

/**
 * Gets the port.
 *
 * @param candidate The candidate to get the port from.
 *
 * @return The port.
 *
 * @since 2.6.0
 */
guint16 purple_media_candidate_get_port(PurpleMediaCandidate *candidate);

/**
 * Gets the base (internal) IP address.
 *
 * This can be NULL.
 *
 * @param candidate The candidate to get the base IP address from.
 *
 * @return The base IP address.
 *
 * @since 2.6.0
 */
gchar *purple_media_candidate_get_base_ip(PurpleMediaCandidate *candidate);

/**
 * Gets the base (internal) port.
 *
 * Invalid if the base IP is NULL.
 *
 * @param candidate The candidate to get the base port.
 *
 * @return The base port.
 *
 * @since 2.6.0
 */
guint16 purple_media_candidate_get_base_port(PurpleMediaCandidate *candidate);

/**
 * Gets the protocol (TCP or UDP).
 *
 * @param candidate The candidate to get the protocol from.
 *
 * @return The protocol.
 *
 * @since 2.6.0
 */
PurpleMediaNetworkProtocol purple_media_candidate_get_protocol(
		PurpleMediaCandidate *candidate);

/**
 * Gets the priority.
 *
 * @param candidate The candidate to get the priority from.
 *
 * @return The priority.
 *
 * @since 2.6.0
 */
guint32 purple_media_candidate_get_priority(PurpleMediaCandidate *candidate);

/**
 * Gets the candidate type.
 *
 * @param candidate The candidate to get the candidate type from.
 *
 * @return The candidate type.
 *
 * @since 2.6.0
 */
PurpleMediaCandidateType purple_media_candidate_get_candidate_type(
		PurpleMediaCandidate *candidate);

/**
 * Gets the username.
 *
 * This can be NULL. It depends on the transmission type.
 *
 * @param The candidate to get the username from.
 *
 * @return The username.
 *
 * @since 2.6.0
 */
gchar *purple_media_candidate_get_username(PurpleMediaCandidate *candidate);

/**
 * Gets the password.
 *
 * This can be NULL. It depends on the transmission type.
 *
 * @param The candidate to get the password from.
 *
 * @return The password.
 *
 * @since 2.6.0
 */
gchar *purple_media_candidate_get_password(PurpleMediaCandidate *candidate);

/**
 * Gets the TTL.
 *
 * @param The candidate to get the TTL from.
 *
 * @return The TTL.
 *
 * @since 2.6.0
 */
guint purple_media_candidate_get_ttl(PurpleMediaCandidate *candidate);

G_END_DECLS

#endif  /* _PURPLE_MEDIA_CANDIDATE_H_ */

