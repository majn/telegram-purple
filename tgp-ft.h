/*
 This file is part of telegram-purple
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 
 Copyright Matthias Jentsch 2014-2015
 */

#ifndef __telegram_adium__tgp_ft__
#define __telegram_adium__tgp_ft__

#include <stdio.h>
#include <tgl.h>

#include "tgp-structs.h"

PurpleXfer *tgprpl_new_xfer (PurpleConnection * gc, const char *who);
void tgprpl_send_file (PurpleConnection * gc, const char *who, const char *file);
void tgprpl_recv_file (PurpleConnection * gc, const char *who, struct tgl_message *M);
void tgprpl_recv_encr_file (PurpleConnection * gc, const char *who, struct tgl_message *M);
void tgprpl_xfer_free_all (connection_data *conn);

#endif
