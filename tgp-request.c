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

#include "telegram-purple.h"

#include <glib.h>
#include <tgl.h>
#include <tgl-binlog.h>
#include <tgl-methods-in.h>
#include <tgl-queries.h>

#include "msglog.h"
#include "tgp-request.h"
#include "telegram-base.h"
#include "tgp-2prpl.h"
#include "tgp-utils.h"
#include "tgp-structs.h"

static void request_code_entered (struct request_values_data *data, const gchar *code) {
  char *stripped = g_strstrip (purple_markup_strip_html (code));
  debug ("sending code: '%s'\n", stripped);
  data->callback (data->TLS, (const char **)&stripped, data->arg);
  g_free (stripped);
}

static void request_code_canceled (struct request_values_data *data) {
  purple_connection_error_reason (tg_get_conn (data->TLS), PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED, "registration canceled");
  free (data);
}

static void request_code (struct tgl_state *TLS, void (*callback) (struct tgl_state *TLS, const char *string[], void *arg),
    void *arg) {
  debug ("client is not registered, registering...");

  struct request_values_data *data = malloc (sizeof(struct request_values_data));
  data->TLS = TLS;
  data->arg = arg;
  data->callback = callback;

  if (purple_account_get_bool (tg_get_acc (TLS), "compat-verification", 0) ||
      ! purple_request_input (tg_get_conn (TLS), _("Login code"), _("Enter login code"), _("Telegram wants to verify your "
        "identity. Please enter the code that you have received via SMS."), NULL, 0, 0, _("the code"), _("OK"),
        G_CALLBACK(request_code_entered), _("Cancel"), G_CALLBACK(request_code_canceled), tg_get_acc (TLS), NULL, NULL, data)) {
    
    // the purple request API is not supported, create a new conversation (the Telegram system account "Telegram") to
    // prompt the user for the code.
    tg_get_data (TLS)->request_code_data = data;
    purple_connection_set_state (tg_get_conn (TLS), PURPLE_CONNECTED);
    PurpleConversation *conv = purple_conversation_new (PURPLE_CONV_TYPE_IM, tg_get_acc (TLS), "Telegram");
    purple_conversation_write (conv, "Telegram", _("What is your SMS verification code?"),
        PURPLE_MESSAGE_RECV | PURPLE_MESSAGE_SYSTEM, 0);
  }
}

static void request_name_and_code (struct tgl_state *TLS, void (*callback) (struct tgl_state *TLS, const char *string[], void *arg), void *arg);
static void request_name_code_entered (struct request_values_data *data, PurpleRequestFields* fields) {
  char *names[] = {
    g_strstrip (g_strdup (purple_request_fields_get_string (fields, "first_name"))),
    g_strstrip (g_strdup (purple_request_fields_get_string (fields, "last_name"))),
    g_strstrip (g_strdup (purple_request_fields_get_string (fields, "code")))
  };

  if (str_not_empty (names[0]) && str_not_empty (names[1]) && str_not_empty (names[2])) {
    data->callback (data->TLS, (const char **) names, data->arg);
  } else {
    request_name_and_code (data->TLS, data->callback, data->arg);
  }

  int j;
  for (j = 0; j < 3; ++j) {
    g_free (names[j]);
  }
  free (data);
}

static void request_name_and_code (struct tgl_state *TLS, void (*callback) (struct tgl_state *TLS, const char *string[], void *arg), void *arg) {
  debug ("phone is not registered, registering...");

  struct request_values_data *data = talloc0 (sizeof (struct request_values_data));
  data->callback = callback;
  data->arg = arg;
  
  PurpleRequestFields* fields = purple_request_fields_new ();
  PurpleRequestField* field = 0;
  
  PurpleRequestFieldGroup* group = purple_request_field_group_new (_("Registration"));
  field = purple_request_field_string_new ("first_name", _("First name"), "", 0);
  purple_request_field_group_add_field (group, field);
  field = purple_request_field_string_new ("last_name", _("Last name"), "", 0);
  purple_request_field_group_add_field (group, field);
  purple_request_fields_add_group (fields, group);
  
  group = purple_request_field_group_new (_("Authorization"));
  field = purple_request_field_string_new ("code", _("Login code"), "", 0);
  purple_request_field_group_add_field (group, field);
  purple_request_fields_add_group (fields, group);
  
  if (!purple_request_fields (tg_get_conn (TLS), _("Register"), _("Please register your phone number."), NULL, fields,
      _("OK"), G_CALLBACK(request_name_code_entered), _("Cancel"), NULL, tg_get_acc (TLS), NULL, NULL, data)) {
    // purple_request API not available
    const char *error = _("Phone number is not registered. Please register your phone on a different client.");
    purple_connection_error_reason (tg_get_conn (data->TLS) , PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED, error);
    purple_notify_error (_telegram_protocol, _("Not Registered"), _("Not Registered"), error);
  }
}

static void request_password_entered (struct request_values_data *data, PurpleRequestFields* fields) {
  const char *pass = purple_request_fields_get_string (fields, "password");
  data->callback (data->TLS, &pass, data->arg);
  free (data);
}

void request_password (struct tgl_state *TLS, void (*callback) (struct tgl_state *TLS, const char *string[], void *arg), void *arg) {
  connection_data *conn = TLS->ev_base;
  
  struct request_values_data *data = malloc (sizeof(struct request_values_data));
  data->TLS = TLS;
  data->arg = arg;
  data->callback = callback;
  
  PurpleRequestFields* fields = purple_request_fields_new();
  PurpleRequestField* field = NULL;
  
  PurpleRequestFieldGroup* group = purple_request_field_group_new ("");
  field = purple_request_field_string_new ("password", _("Password"), "", 0);
  purple_request_field_string_set_masked (field, TRUE);
  purple_request_field_group_add_field (group, field);
  purple_request_fields_add_group (fields, group);
  
  if (! purple_request_fields (conn->gc, _("Enter password"), _("Enter password for two factor authentication"),
      NULL, fields, "Ok", G_CALLBACK(request_password_entered), "Cancel", NULL, conn->pa,
      NULL, NULL, data)) {
    const char *error = _("No password set for two factor authentication. Please enter it in the extended settings.");
    purple_connection_error_reason (conn->gc, PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED, error);
    purple_notify_error (_telegram_protocol, _("Password invalid"), _("Password invalid"), error);
  }
}

static void accept_secret_chat_cb (struct accept_secret_chat_data *data, const gchar *code) {
  tgl_do_accept_encr_chat_request (data->TLS, data->U, write_secret_chat_gw, 0);
  g_free (data);
}

static void decline_secret_chat_cb (gpointer _data, const gchar *code) {
  struct accept_secret_chat_data *data = _data;
  
  bl_do_peer_delete (data->TLS, data->U->id);
  purple_blist_remove_buddy (tgp_blist_buddy_find (data->TLS, data->U->id));
  
  g_free (data);
}

void request_accept_secret_chat (struct tgl_state *TLS, struct tgl_secret_chat *U) {
  connection_data *conn = TLS->ev_base;
  PurpleBuddy *who = tgp_blist_buddy_find (TLS, TGL_MK_USER (U->user_id));
  struct accept_secret_chat_data *data = g_new (struct accept_secret_chat_data, 1);
  data->TLS = TLS;
  data->U = U;
  
  gchar *message = g_strdup_printf (_("Accept Secret Chat '%s'?"), U->print_name);
  purple_request_accept_cancel (conn->gc, _("Secret Chat"), message, _("Secret chats can only have one "
      "end point. If you accept a secret chat on this device, its messages will not be available anywhere "
      "else. If you decline, you can accept the chat on other devices."), 0, conn->pa, who->name, NULL, data,
      G_CALLBACK(accept_secret_chat_cb), G_CALLBACK(decline_secret_chat_cb));
  g_free (message);
}

static void create_group_chat_cb (struct accept_create_chat_data *data, PurpleRequestFields* fields) {
  // FIXME: Oh god.
  const char *users[3] = {
    purple_request_fields_get_string (fields, "user1"),
    purple_request_fields_get_string (fields, "user2"),
    purple_request_fields_get_string (fields, "user3")
  };
  
  tgp_create_group_chat_by_usernames (data->TLS, data->title, users, 3, FALSE);
  g_free (data->title);
  free (data);
}

static void cancel_group_chat_cb (gpointer data) {
  struct accept_create_chat_data *d = data;
  g_free (d->title);
  free (d);
}

void request_choose_user (struct accept_create_chat_data *data) {
  // Telegram doesn't allow to create chats with only one user, so we need to force
  // the user to specify at least one other one.
  PurpleRequestFields* fields = purple_request_fields_new();
  PurpleRequestFieldGroup* group = purple_request_field_group_new (
      _("Invite at least one additional user (Autocompletion available).\nYou can add more users once"
      " the chat was created."));
  
  PurpleRequestField *field = purple_request_field_string_new ("user1", _("Username"), NULL, FALSE);
  purple_request_field_set_type_hint (field, "screenname");
  purple_request_field_group_add_field (group, field);
  
  field = purple_request_field_string_new ("user2", _("Username"), NULL, FALSE);
  purple_request_field_set_type_hint (field, "screenname");
  purple_request_field_group_add_field (group, field);
  
  field = purple_request_field_string_new ("user3", _("Username"), NULL, FALSE);
  purple_request_field_set_type_hint (field, "screenname");
  purple_request_field_group_add_field (group, field);
  
  purple_request_fields_add_group (fields, group);
  purple_request_fields (tg_get_conn (data->TLS), _("Create group chat"), _("Invite users"), NULL, fields, _("OK"),
      G_CALLBACK(create_group_chat_cb), _("Cancel"), G_CALLBACK(cancel_group_chat_cb), tg_get_acc (data->TLS), NULL, NULL, data);
}

void request_create_chat (struct tgl_state *TLS, const char *subject) {
  struct accept_create_chat_data *data = malloc (sizeof (struct accept_create_chat_data));
  data->title = g_strdup (subject);
  data->TLS = TLS;
  char *title = _("Chat doesn't exist (yet)");
  char *secondary = g_strdup_printf (_("Do you want to create a new group chat named '%s'?"), subject);
  
  // FIXME: This dialog is pointless. It only asks whether the user wants to create a new chat.
  // This should be merged with the next dialog.
  // TODO: This still displays "Cancel" and "Accept", no matter $LANG
  purple_request_accept_cancel (tg_get_conn (TLS), _("Create group chat"), title, secondary, 1, tg_get_acc (TLS), NULL,
      NULL, data, G_CALLBACK(request_choose_user), G_CALLBACK(cancel_group_chat_cb));
  g_free (secondary);
}

void request_value (struct tgl_state *TLS, enum tgl_value_type type, const char *prompt, int num_values,
    void (*callback) (struct tgl_state *TLS, const char *string[], void *arg), void *arg) {

  debug ("tgl requests user input, tgl_value_type: %d, prompt: %s, count: %d", type, prompt, num_values);
  switch (type) {
    case tgl_cur_password: {
      const char *P = purple_account_get_string (tg_get_acc (TLS), TGP_KEY_PASSWORD_TWO_FACTOR, NULL);
      if (str_not_empty (P)) {
        if (tg_get_data (TLS)->password_retries ++ < 1) {
          callback (TLS, &P, arg);
          return;
        }
      }
      request_password (TLS, callback, arg);
      break;
    }

    case tgl_register_info:
      request_name_and_code (TLS, callback, arg);
      break;

    case tgl_code:
      request_code (TLS, callback, arg);
      break;

    case tgl_phone_number: {
      // if we arrive here for the second time the specified phone number is not valid. We do not
      // ask for the phone number directly, cause in that case the account would still be created
      // named with the invalid phone number, even though the login will work
      tgp_error_if_false (TLS, tg_get_data(TLS)->login_retries++ < 1, _("Invalid phone number"),
          _("Please enter only numbers in the international phone number format, "
              "a leading + following by the country prefix and the phone number.\n"
              "Do not use any other special chars."));

      const char *username = purple_account_get_username (tg_get_acc (TLS));
      callback (TLS, &username, arg);
      break;
    }

    default:
      assert (FALSE);
      break;
  }
}
