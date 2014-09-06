/**
 * @file page.h Paging functions
 *
 * purple
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
#ifndef MSN_PAGE_H
#define MSN_PAGE_H

typedef struct _MsnPage MsnPage;

#include "session.h"

/**
 * A page.
 */
struct _MsnPage
{
	char *from_location;
	char *from_phone;

	char *body;
};

/**
 * Creates a new, empty page.
 *
 * @return A new page.
 */
MsnPage *msn_page_new(void);

/**
 * Destroys a page.
 */
void msn_page_destroy(MsnPage *page);

/**
 * Generates the payload data of a page.
 *
 * @param page     The page.
 * @param ret_size The returned size of the payload.
 *
 * @return The payload data of a page.
 */
char *msn_page_gen_payload(const MsnPage *page, size_t *ret_size);

/**
 * Sets the body of a page.
 *
 * @param page  The page.
 * @param body The body of the page.
 */
void msn_page_set_body(MsnPage *page, const char *body);

/**
 * Returns the body of the page.
 *
 * @param page The page.
 *
 * @return The body of the page.
 */
const char *msn_page_get_body(const MsnPage *page);

#endif /* MSN_PAGE_H */
