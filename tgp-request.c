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

static struct request_values_data *request_values_data_init (struct tgl_state *TLS, void *callback, void *arg, int num_values) {
  struct request_values_data *data = talloc0 (sizeof (struct request_values_data));
  data->TLS = TLS;
  data->callback = callback;
  data->arg = arg;
  data->num_values = num_values;
  return data;
}

static void request_code_entered (struct request_values_data *data, const gchar *code) {
  char *stripped = g_strstrip (purple_markup_strip_html (code));
  debug ("sending code: '%s'\n", stripped);
  data->callback (data->TLS, (const char **)&stripped, data->arg);
  g_free (stripped);
}

static void request_canceled_disconnect (struct request_values_data *data) {
  purple_connection_error_reason (tls_get_conn (data->TLS), PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED, "registration canceled");
  free (data);
}

static void request_canceled (struct request_values_data *data) {
  free (data);
}

static void request_code (struct tgl_state *TLS, void (*callback) (struct tgl_state *TLS, const char *string[], void *arg),
    void *arg) {
  debug ("client is not registered, registering...");
  char *explanation = _("Telegram wants to verify your "
      "identity. Please enter the login code that you have received via SMS.");
  if (purple_account_get_bool (tls_get_pa (TLS), "compat-verification", 0) ||
      !purple_request_input (tls_get_conn (TLS), _("Login code"), _("Enter login code"), explanation, NULL, 0, 0, _("the code"), _("OK"),
          G_CALLBACK(request_code_entered), _("Cancel"), G_CALLBACK(request_canceled_disconnect), tls_get_pa (TLS),
          NULL, NULL, request_values_data_init (TLS, callback, arg, 0))) {
    
    // the purple request API is not supported, create a new conversation (the Telegram system account "Telegram") to
    // prompt the user for the code.
    tls_get_data (TLS)->request_code_data = request_values_data_init (TLS, callback, arg, 0);
    purple_connection_set_state (tls_get_conn (TLS), PURPLE_CONNECTED);
    PurpleConversation *conv = purple_conversation_new (PURPLE_CONV_TYPE_IM, tls_get_pa (TLS), "Telegram");
    purple_conversation_write (conv, "Telegram", explanation,
        PURPLE_MESSAGE_RECV | PURPLE_MESSAGE_SYSTEM, 0);
  }
}

static void request_name (struct tgl_state *TLS, void (*callback) (struct tgl_state *TLS, const char *string[],
    void *arg), void *arg);
static void request_name_code_entered (struct request_values_data *data, PurpleRequestFields* fields) {
  char *names[] = {
    g_strdup ("y"), // input line is a y/n choice
    g_strstrip (g_strdup (purple_request_fields_get_string (fields, "first_name"))),
    g_strstrip (g_strdup (purple_request_fields_get_string (fields, "last_name")))
  };
  if (str_not_empty (names[1]) && str_not_empty (names[2])) {
    data->callback (data->TLS, (const char **) names, data->arg);
  } else {
    request_name (data->TLS, data->callback, data->arg);
  }
  int j;
  for (j = 0; j < 3; ++j) {
    g_free (names[j]);
  }
  free (data);
}

static void request_name (struct tgl_state *TLS,
    void (*callback) (struct tgl_state *TLS, const char *string[], void *arg), void *arg) {
  debug("Phone is not registered, registering...");

  PurpleRequestFields *fields = purple_request_fields_new ();
  PurpleRequestField *field = 0;

  PurpleRequestFieldGroup *group = purple_request_field_group_new (_("Registration"));
  field = purple_request_field_string_new ("first_name", _("First name"), "", 0);
  purple_request_field_group_add_field (group, field);

  field = purple_request_field_string_new ("last_name", _("Last name"), "", 0);
  purple_request_field_group_add_field (group, field);
  purple_request_fields_add_group (fields, group);

  if (!purple_request_fields (tls_get_conn (TLS), _("Register"), _("Please register your phone number."), NULL, fields,
      _("OK"), G_CALLBACK(request_name_code_entered), _("Cancel"), G_CALLBACK(request_canceled_disconnect),
      tls_get_pa (TLS), NULL, NULL, request_values_data_init(TLS, callback, arg, 0))) {

    // purple_request API not available
    const char *error = _("Phone number is not registered. Please register your phone on a different client.");
    purple_connection_error_reason (tls_get_conn (TLS), PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED, error);
    purple_notify_error (_telegram_protocol, _("Not registered"), _("Not registered"), error);
  }
}

static void request_password_entered (struct request_values_data *data, char *password) {
  data->callback (data->TLS, (const char **) &password, data->arg);
  free (data);
}

void request_password (struct tgl_state *TLS,
    void (*callback) (struct tgl_state *TLS, const char *string[], void *arg), void *arg) {

  if (! purple_request_input (tls_get_conn (TLS), _("Password needed"), _("Password needed"), _("Enter password for two factor authentication"),
      NULL, FALSE, TRUE, NULL, _("Ok"), G_CALLBACK(request_password_entered), _("Cancel"), G_CALLBACK(request_canceled_disconnect),
      tls_get_pa (TLS), NULL, NULL, request_values_data_init (TLS, callback, arg, 0))) {
    const char *error = _("No password set for two factor authentication. Please enter it in the extended settings.");
    purple_connection_error_reason (tls_get_conn (TLS), PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED, error);
    purple_notify_error (_telegram_protocol, _("Password invalid"), _("Password invalid"), error);
  }
}

static void accept_secret_chat_cb (struct accept_secret_chat_data *data) {
  tgl_do_accept_encr_chat_request (data->TLS, data->U, write_secret_chat_gw, 0);
  free (data);
}

static void decline_secret_chat_cb (struct accept_secret_chat_data *data) {
  bl_do_peer_delete (data->TLS, data->U->id);
  purple_blist_remove_buddy (tgp_blist_buddy_find (data->TLS, data->U->id));
  free (data);
}

void request_accept_secret_chat (struct tgl_state *TLS, struct tgl_secret_chat *U) {
  tgl_peer_t *P = tgl_peer_get (TLS, TGL_MK_USER(U->user_id));
  g_return_if_fail (P);
  
  struct accept_secret_chat_data *data = talloc0 (sizeof (struct accept_secret_chat_data));
  data->TLS = TLS;
  data->U = U;

  gchar *message = g_strdup_printf (_("Accept secret chat '%s' on this device?"), U->print_name);
  purple_request_accept_cancel (tls_get_conn (TLS), _("Secret chat"), message, _("Secret chats can only have one "
      "end point. If you accept a secret chat on this device, its messages will not be available anywhere "
      "else. If you decline, you can still accept the chat on other devices."), 0, tls_get_pa (TLS), P->print_name,
      NULL, data, G_CALLBACK(accept_secret_chat_cb), G_CALLBACK(decline_secret_chat_cb));
  g_free (message);
}

static void create_group_chat_cb (struct request_values_data *data, PurpleRequestFields* fields) {
  // FIXME: Oh god.
  const char *users[3] = {
    purple_request_fields_get_string (fields, "user1"),
    purple_request_fields_get_string (fields, "user2"),
    purple_request_fields_get_string (fields, "user3")
  };

  tgp_create_group_chat_by_usernames (data->TLS, data->arg, users, 3, FALSE);
  g_free (data->arg);
  free (data);
}

static void cancel_group_chat_cb (struct request_values_data *data) {
  g_free (data->arg);
  free (data);
}

void request_create_chat (struct tgl_state *TLS, const char *subject) {

  // Telegram doesn't allow to create chats with only one user, so we need to force
  // the user to specify at least one other one.
  PurpleRequestFields* fields = purple_request_fields_new ();
  PurpleRequestFieldGroup* group = purple_request_field_group_new (
      _("Invite at least one additional user by specifying\n their full name (autocompletion available).\nYou can add more users once"
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
  purple_request_fields (tls_get_conn (TLS), _("Create group chat"), _("Invite users"), NULL, fields, _("OK"),
      G_CALLBACK(create_group_chat_cb), _("Cancel"), G_CALLBACK(cancel_group_chat_cb), tls_get_pa (TLS), NULL, NULL,
      request_values_data_init (TLS, NULL, (void *) g_strdup (subject), 0));
}

static void request_cur_and_new_password_ok (struct request_values_data *data, PurpleRequestFields* fields) {
  const char *names[3] = {
      purple_request_fields_get_string (fields, "current"),
      purple_request_fields_get_string (fields, "new1"),
      purple_request_fields_get_string (fields, "new2")
  };
  data->callback(data->TLS, names, data->arg);
  free (data);
}

void request_cur_and_new_password (struct tgl_state *TLS,
    void (*callback) (struct tgl_state *TLS, const char *string[], void *arg), void *arg) {

  PurpleRequestFields* fields = purple_request_fields_new ();
  PurpleRequestFieldGroup* group = purple_request_field_group_new (_("Change password"));

  PurpleRequestField *field = purple_request_field_string_new ("current", _("Current"), NULL, FALSE);
  purple_request_field_string_set_masked (field, TRUE);
  purple_request_field_group_add_field (group, field);

  field = purple_request_field_string_new ("new1", _("Password"), NULL, FALSE);
  purple_request_field_string_set_masked (field, TRUE);
  purple_request_field_group_add_field (group, field);

  field = purple_request_field_string_new ("new2", _("Confirm"), NULL, FALSE);
  purple_request_field_string_set_masked (field, TRUE);
  purple_request_field_group_add_field (group, field);

  purple_request_fields_add_group (fields, group);
  purple_request_fields (tls_get_conn (TLS), _("Change password"), _("Change password"), NULL, fields,
      _("OK"), G_CALLBACK(request_cur_and_new_password_ok),
      _("Cancel"), G_CALLBACK(request_canceled), tls_get_pa (TLS), NULL, NULL,
      request_values_data_init (TLS, callback, arg, 0));
}

static void request_new_password_ok (struct request_values_data *data, PurpleRequestFields* fields) {
  const char *names[2] = {
      purple_request_fields_get_string (fields, "new1"),
      purple_request_fields_get_string (fields, "new2")
  };
  data->callback(data->TLS, names, data->arg);
  free (data);
}

void request_new_password (struct tgl_state *TLS,
    void (*callback) (struct tgl_state *TLS, const char *string[], void *arg), void *arg) {

  PurpleRequestFields* fields = purple_request_fields_new ();
  PurpleRequestFieldGroup* group = purple_request_field_group_new (_("New password"));

  PurpleRequestField * field = purple_request_field_string_new ("new1", _("Password"), NULL, FALSE);
  purple_request_field_string_set_masked (field, TRUE);
  purple_request_field_group_add_field (group, field);

  field = purple_request_field_string_new ("new2", _("Confirm"), NULL, FALSE);
  purple_request_field_string_set_masked (field, TRUE);
  purple_request_field_group_add_field (group, field);

  purple_request_fields_add_group (fields, group);
  purple_request_fields (tls_get_conn (TLS), _("New password"), _("New password"), NULL, fields,
      _("OK"), G_CALLBACK(request_new_password_ok),
      _("Cancel"), G_CALLBACK(request_canceled), tls_get_pa (TLS), NULL, NULL,
      request_values_data_init (TLS, callback, arg, 0));
}

void request_value (struct tgl_state *TLS, enum tgl_value_type type, const char *prompt, int num_values,
    void (*callback) (struct tgl_state *TLS, const char *string[], void *arg), void *arg) {

  debug ("tgl requests user input, tgl_value_type: %d, prompt: %s, count: %d", type, prompt, num_values);
  switch (type) {
    case tgl_cur_password: {
      const char *P = purple_account_get_string (tls_get_pa(TLS), TGP_KEY_PASSWORD_TWO_FACTOR, NULL);
      if (str_not_empty (P)) {
        if (tls_get_data (TLS)->password_retries ++ < 1) {
          callback (TLS, &P, arg);
          return;
        }
      }
      request_password (TLS, callback, arg);
      break;
    }
    case tgl_cur_and_new_password:
      request_cur_and_new_password (TLS, callback, arg);
      break;
    case tgl_new_password:
      request_new_password (TLS, callback, arg);
      break;
    case tgl_register_info:
      request_name (TLS, callback, arg);
      break;
    case tgl_code:
      request_code (TLS, callback, arg);
      break;
    case tgl_phone_number: {
      // if we arrive here for the second time the specified phone number is not valid. We do not
      // ask for the phone number directly, cause in that case the account would still be created
      // named with the invalid phone number, even though the login will work
      tgp_error_if_false (TLS, tls_get_data (TLS)->login_retries++ < 1, _("Invalid phone number"),
          _("Please enter only numbers in the international phone number format, "
              "a leading + following by the country prefix and the phone number.\n"
              "Do not use any other special chars."));
      const char *username = purple_account_get_username (tls_get_pa (TLS));
      callback (TLS, &username, arg);
      break;
    }
    case tgl_bot_hash:
      assert (FALSE && "we are not a bot");
      break;
  }
}
