/**
 * @file nat-pmp.h NAT-PMP Implementation
 * @ingroup core
 */

/* purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * Most code in nat-pmp.h copyright (C) 2007, R. Tyler Ballance, bleep, LLC.
 * This file is distributed under the 3-clause (modified) BSD license:
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * the following disclaimer.
 * Neither the name of the bleep. LLC nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#ifndef _PURPLE_NAT_PMP_H
#define _PURPLE_NAT_PMP_H

#include <glib.h>

#define PURPLE_PMP_LIFETIME	3600	/* 3600 seconds */

typedef enum {
	PURPLE_PMP_TYPE_UDP,
	PURPLE_PMP_TYPE_TCP
} PurplePmpType;

/**
 * Initialize nat-pmp
 */
void purple_pmp_init(void);

/**
 *
 */
char *purple_pmp_get_public_ip(void);

/**
 * Remove the NAT-PMP mapping for a specified type on a specified port
 *
 * @param type The PurplePmpType
 * @param privateport The private port on which we are listening locally
 * @param publicport The public port on which we are expecting a response
 * @param lifetime The lifetime of the mapping. It is recommended that this be PURPLE_PMP_LIFETIME.
 *
 * @returns TRUE if succesful; FALSE if unsuccessful
 */
gboolean purple_pmp_create_map(PurplePmpType type, unsigned short privateport, unsigned short publicport, int lifetime);

/**
 * Remove the NAT-PMP mapping for a specified type on a specified port
 *
 * @param type The PurplePmpType
 * @param privateport The private port on which the mapping was previously made
 *
 * @returns TRUE if succesful; FALSE if unsuccessful
 */
gboolean purple_pmp_destroy_map(PurplePmpType type, unsigned short privateport);

#endif

