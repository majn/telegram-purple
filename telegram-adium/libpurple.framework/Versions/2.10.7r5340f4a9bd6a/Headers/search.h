/**
 * @file search.h
 *
 * purple
 *
 * Copyright (C) 2005  Bartosz Oler <bartosz@bzimage.us>
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


#ifndef _PURPLE_GG_SEARCH_H
#define _PURPLE_GG_SEARCH_H

#include "connection.h"

#include <libgadu.h>
#include "gg.h"


typedef enum {
	GGP_SEARCH_TYPE_INFO,
	GGP_SEARCH_TYPE_FULL

} GGPSearchType;

typedef struct {

	char *uin;
	char *lastname;
	char *firstname;
	char *nickname;
	char *city;
	char *birthyear;
	char *gender;
	char *active;

	GGPSearchType search_type;
	guint32 seq;
	guint16 page_number;
	guint16 page_size; /* how many contacts fits into one page of results */

	void *user_data;
	void *window;
} GGPSearchForm;

typedef GHashTable GGPSearches;


/**
 * Create a new GGPSearchForm structure, and set the fields
 * to the sane defaults.
 *
 * @return Newly allocated GGPSearchForm.
 */
GGPSearchForm *
ggp_search_form_new(GGPSearchType st);

/**
 * Destroy a Search Form.
 *
 * @param form Search Form to destroy.
 */
void
ggp_search_form_destroy(GGPSearchForm *form);

/**
 * Add a search to the list of searches.
 *
 * @param searches The list of searches.
 * @param seq      Search (form) ID number.
 * @param form 	   The search form to add.
 */
void
ggp_search_add(GGPSearches *searches, guint32 seq, GGPSearchForm *form);

/**
 * Remove a search from the list.
 *
 * If you want to destory the search completely also call:
 * ggp_search_form_destroy().
 *
 * @param searches The list of searches.
 * @param seq      ID number of the search.
 */
void
ggp_search_remove(GGPSearches *searches, guint32 seq);

/**
 * Return the search with the specified ID.
 *
 * @param searches The list of searches.
 * @param seq      ID number of the search.
 */
GGPSearchForm *
ggp_search_get(GGPSearches *searches, guint32 seq);

/**
 * Create a new GGPSearches structure.
 *
 * @return GGPSearches instance.
 */
GGPSearches *
ggp_search_new(void);

/**
 * Destroy GGPSearches instance.
 *
 * @param searches GGPSearches instance.
 */
void
ggp_search_destroy(GGPSearches *searches);

/**
 * Initiate a search in the public directory.
 *
 * @param gc   PurpleConnection.
 * @param form Filled in GGPSearchForm.
 *
 * @return Sequence number of a search or 0 if an error occurred.
 */
guint32
ggp_search_start(PurpleConnection *gc, GGPSearchForm *form);

/*
 * Return converted to the UTF-8 value of the specified field.
 *
 * @param res    Public directory look-up result.
 * @param num    Id of the record.
 * @param fileld Name of the field.
 *
 * @return UTF-8 encoded value of the field.
 */
char *
ggp_search_get_result(gg_pubdir50_t res, int num, const char *field);


#endif /* _PURPLE_GG_SEARCH_H */

/* vim: set ts=8 sts=0 sw=8 noet: */
