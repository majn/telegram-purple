/**
 * @file yahoo_doodle.h The Yahoo! protocol plugin Doodle IMVironment object
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

#ifndef _YAHOO_DOODLE_H_
#define _YAHOO_DOODLE_H_

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "whiteboard.h"
#include "cmds.h"

#define DOODLE_IMV_KEY "doodle;106"

/******************************************************************************
 * Defines
 *****************************************************************************/
/* Doodle communication commands */
/* TODO: Should be an enum. */
#define DOODLE_CMD_REQUEST	0
#define DOODLE_CMD_CLEAR	1
#define DOODLE_CMD_DRAW		2
#define DOODLE_CMD_EXTRA	3
#define DOODLE_CMD_READY	4
#define DOODLE_CMD_CONFIRM	5
/* Doodle communication command for shutting down (also 0) */
#define DOODLE_CMD_SHUTDOWN 0

#define DOODLE_EXTRA_NONE      "\"1\""
#define DOODLE_EXTRA_TICTACTOE "\"3\""
#define DOODLE_EXTRA_DOTS      "\"2\""

/* Doodle session states */
/* TODO: Should be an enum. */
#define DOODLE_STATE_REQUESTING  0
#define DOODLE_STATE_REQUESTED   1
#define DOODLE_STATE_ESTABLISHED 2
#define DOODLE_STATE_CANCELLED    3

/* Doodle canvas dimensions */
#define DOODLE_CANVAS_WIDTH  368
#define DOODLE_CANVAS_HEIGHT 256

/* Doodle color codes (most likely RGB) */
/* TODO: Should be an enum and sorted by color name. */
#define	DOODLE_COLOR_RED    13369344
#define	DOODLE_COLOR_ORANGE 16737792
#define	DOODLE_COLOR_YELLOW 15658496
#define	DOODLE_COLOR_GREEN     52224
#define	DOODLE_COLOR_CYAN      52428
#define	DOODLE_COLOR_BLUE        204
#define	DOODLE_COLOR_VIOLET  5381277
#define	DOODLE_COLOR_PURPLE 13369548
#define	DOODLE_COLOR_TAN    12093547
#define	DOODLE_COLOR_BROWN   5256485
#define	DOODLE_COLOR_BLACK         0
#define	DOODLE_COLOR_GREY   11184810
#define	DOODLE_COLOR_WHITE  16777215

#define PALETTE_NUM_OF_COLORS 12

/* Doodle brush sizes (most likely variable) */
#define DOODLE_BRUSH_SMALL   2
#define DOODLE_BRUSH_MEDIUM  5
#define DOODLE_BRUSH_LARGE  10

#define DOODLE_MAX_BRUSH_MOTIONS 100

/******************************************************************************
 * Datatypes
 *****************************************************************************/
typedef struct _doodle_session
{
	int brush_size;  /* Size of drawing brush */
	int brush_color; /* Color of drawing brush */
	gchar *imv_key;
} doodle_session;

/******************************************************************************
 * API
 *****************************************************************************/

PurpleCmdRet yahoo_doodle_purple_cmd_start(PurpleConversation *conv, const char *cmd, char **args,
									   char **error, void *data);

void yahoo_doodle_process(PurpleConnection *gc, const char *me, const char *from,
						  const char *command, const char *message, const char *imv_key);
void yahoo_doodle_initiate(PurpleConnection *gc, const char *to);

void yahoo_doodle_command_got_shutdown(PurpleConnection *gc, const char *from);

void yahoo_doodle_command_send_request(PurpleConnection *gc, const char *to, const char *imv_key);
void yahoo_doodle_command_send_ready(PurpleConnection *gc, const char *to, const char *imv_key);
void yahoo_doodle_command_send_draw(PurpleConnection *gc, const char *to, const char *message, const char *imv_key);
void yahoo_doodle_command_send_clear(PurpleConnection *gc, const char *to, const char *imv_key);
void yahoo_doodle_command_send_extra(PurpleConnection *gc, const char *to, const char *message, const char *imv_key);
void yahoo_doodle_command_send_confirm(PurpleConnection *gc, const char *to, const char *imv_key);
void yahoo_doodle_command_send_shutdown(PurpleConnection *gc, const char *to);

void yahoo_doodle_start(PurpleWhiteboard *wb);
void yahoo_doodle_end(PurpleWhiteboard *wb);
void yahoo_doodle_get_dimensions(const PurpleWhiteboard *wb, int *width, int *height);
void yahoo_doodle_send_draw_list(PurpleWhiteboard *wb, GList *draw_list);
void yahoo_doodle_clear(PurpleWhiteboard *wb);

void yahoo_doodle_draw_stroke(PurpleWhiteboard *wb, GList *draw_list);
void yahoo_doodle_get_brush(const PurpleWhiteboard *wb, int *size, int *color);
void yahoo_doodle_set_brush(PurpleWhiteboard *wb, int size, int color);

#endif /* _YAHOO_DOODLE_H_ */
