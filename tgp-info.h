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
 
 Copyright Matthias Jentsch 2016
 */

#ifndef tgp_info_h
#define tgp_info_h

#include "telegram-purple.h"

#define TGP_INFO_PHOTO_ID "tgp_photo_id"
void tgp_info_update_photo (PurpleBlistNode *node, tgl_peer_t *P);
void tgprpl_info_show (PurpleConnection *gc, const char *who);
#endif
