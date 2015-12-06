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
 
    Copyright Matthias Jentsch, Vitaly Valtman, Ben Wiederhake, Christopher Althaus 2014-2015
*/

#include "telegram-purple.h"
#include "commit.h"

#ifdef WIN32
#include "win32/win32dep.h"
#endif

static void update_message_handler (struct tgl_state *TLS, struct tgl_message *M);
static void update_marked_read (struct tgl_state *TLS, int num, struct tgl_message *list[]);
static void update_on_logged_in (struct tgl_state *TLS);
static void update_on_ready (struct tgl_state *TLS);
static void update_user_typing (struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status);
static void update_user_status_handler (struct tgl_state *TLS, struct tgl_user *U);
static void update_user_handler (struct tgl_state *TLS, struct tgl_user *U, unsigned flags);
static void update_secret_chat_handler (struct tgl_state *TLS, struct tgl_secret_chat *C, unsigned flags);
static void update_chat_handler (struct tgl_state *TLS, struct tgl_chat *C, unsigned flags);
static void on_user_get_info (struct tgl_state *TLS, void *info_data, int success, struct tgl_user *U);

const char *config_dir = "telegram-purple";
const char *user_pk_filename = "server.tglpub";
#ifdef WIN32
const char *pk_path = "server.tglpub";
#else
const char *pk_path = "/etc/telegram-purple/server.tglpub";
#endif

struct tgl_update_callback tgp_callback = {
  .new_msg = update_message_handler,
  .marked_read = update_marked_read,
  .logprintf = debug,
  .get_values = request_value,
  .logged_in = update_on_logged_in,
  .started = update_on_ready,
  .type_notification = update_user_typing,
    // FIXME: what about type_in_secret_chat_notification, user_registred, user_activated, new_authorization, our_id ?
  .chat_update = update_chat_handler,
  .user_update = update_user_handler,
  .secret_chat_update = update_secret_chat_handler,
  .msg_receive = update_message_handler,
  .user_status_update = update_user_status_handler,
  .create_print_name = tgp_blist_create_print_name
    // FIXME: on_failed_login ?
};

static void _update_buddy (struct tgl_state *TLS, tgl_peer_t *user, unsigned flags) {
  PurpleBuddy *buddy = tgp_blist_buddy_find (TLS, user->id);
  if (buddy) {
    if (flags & TGL_UPDATE_DELETED) {
      purple_blist_remove_buddy (buddy);
    } else {
      if (flags & TGL_UPDATE_CONTACT) {
        purple_blist_alias_buddy (buddy, user->print_name);
      }
      if (flags & TGL_UPDATE_PHOTO) {
        tgl_do_get_user_info (TLS, user->id, 0, on_user_get_info, get_user_info_data_new (0, user->id));
      }
    }
  }
}

static void update_user_handler (struct tgl_state *TLS, struct tgl_user *user, unsigned flags) {
  debug ("update_user_handler() flags: %s", print_flags_update (flags));
  
  if (tgl_get_peer_id (TLS->our_id) == tgl_get_peer_id (user->id) && (flags & (TGL_UPDATE_NAME | TGL_UPDATE_CONTACT))) {
    // own user object, do not add that user to the buddy list but make the ID known to the name lookup and
    // set the print name as the name to be displayed in IM chats instead of the login name (the phone number)
    purple_connection_set_display_name (tls_get_conn (TLS), user->print_name);
    tgp_blist_peer_add_purple_name (TLS, user->id, user->print_name);
    return;
  }
  
  if (flags & TGL_UPDATE_CREATED) {
    // new user was allocated, fetch the corresponding buddy from the buddy list
    PurpleBuddy *buddy = tgp_blist_buddy_find (TLS, user->id);
    debug ("new user %s allocated flags: %s", user->print_name, print_flags_peer (flags));
    
    // the buddy doesn't exist, if it is stored in the legacy naming format find and migrate it. This
    // should only happen when making the switch from a version < 1.2.2 to a version >= 1.2.2
    if (! buddy) {
      char *id = g_strdup_printf ("%d", tgl_get_peer_id (user->id));
      buddy = purple_find_buddy (tls_get_pa (TLS), id);
      g_free (id);
      if (buddy) {
        debug ("migrating buddy from old name %s to %s", purple_buddy_get_name (buddy), user->print_name);
        buddy = tgp_blist_buddy_migrate (TLS, buddy, user);
        if (user->photo_id) {
          tgl_do_get_user_info (TLS, user->id, 0, on_user_get_info, get_user_info_data_new (0, user->id));
        }
      }
    }
    
    // changing the username would mean dropping the history. The first print name that is known to the blist
    // will remain the permanent name for this user, all future name changes will just change the user alias.
    if (buddy) {
      tgp_blist_peer_add_purple_name (TLS, user->id, purple_buddy_get_name (buddy));
      if (user->print_name && strcmp (user->print_name, purple_buddy_get_name (buddy))) {
        purple_blist_alias_buddy (buddy, user->print_name);
      }
      p2tgl_prpl_got_user_status (TLS, user->id, &user->status);
    } else {
      tgp_blist_peer_add_purple_name (TLS, user->id, user->print_name);
    }
  } else {
    // peer was altered in some way
    _update_buddy (TLS, (tgl_peer_t *)user, flags);
  }
}

static void update_message_handler (struct tgl_state *TLS, struct tgl_message *M) {
  write_files_schedule (TLS);
  tgp_msg_recv (TLS, M);
}

static void update_user_status_handler (struct tgl_state *TLS, struct tgl_user *U) {
  p2tgl_prpl_got_user_status (TLS, U->id, &U->status);
}

static void update_secret_chat_handler (struct tgl_state *TLS, struct tgl_secret_chat *U, unsigned flags) {
  debug ("update_secret_chat_handler() flags: %s", print_flags_update (flags));
  PurpleBuddy *buddy = tgp_blist_buddy_find (TLS, U->id);

  if (flags & TGL_UPDATE_CREATED) {
    tgp_blist_peer_add_purple_name (TLS, U->id, U->print_name);
  } else {
    if (flags & TGL_UPDATE_WORKING) {
      write_secret_chat_file (TLS);
      if (U->state == sc_ok) {
        tgp_msg_sys_out (TLS, _("Secret chat ready."), U->id, TRUE);
      }
    }
    if (buddy) {
      if (flags & TGL_UPDATE_DELETED) {
        U->state = sc_deleted;
        write_secret_chat_file (TLS);
        tgp_msg_sys_out (TLS, _("Secret chat terminated."), U->id, FALSE);
        purple_prpl_got_user_status (tls_get_pa (TLS), tgp_blist_peer_get_purple_name (TLS, U->id), "offline", NULL);
      } else {
        _update_buddy (TLS, (tgl_peer_t *)U, flags);
      }
    }
  }
  
  if (!(flags & TGL_UPDATE_DELETED)) {
    if (! buddy) {
      buddy = tgp_blist_buddy_new (TLS, (tgl_peer_t *)U);
      purple_blist_add_buddy (buddy, NULL, tgp_blist_group_init (_("Telegram")), NULL);
      purple_blist_alias_buddy (buddy, U->print_name);
    }
    purple_prpl_got_user_status (tls_get_pa (TLS), tgp_blist_peer_get_purple_name (TLS, U->id), "mobile", NULL);
  }

  if (flags & TGL_UPDATE_REQUESTED) {
    const char* choice = purple_account_get_string (tls_get_pa (TLS), "accept-secret-chats", "ask");
    if (! strcmp (choice, "always")) {
      tgl_do_accept_encr_chat_request (TLS, U, write_secret_chat_gw, 0);
    } else if (! strcmp (choice, "ask")) {
      request_accept_secret_chat (TLS, U);
    }
  }
}

static void update_chat_handler (struct tgl_state *TLS, struct tgl_chat *chat, unsigned flags) {
  debug ("update_chat_handler() flags: %s", print_flags_update(flags));
  
  if (flags & TGL_UPDATE_CREATED) {
    debug ("new chat '%s' allocated flags: %s", chat->title, print_flags_peer (chat->flags));
    tgp_blist_peer_add_purple_name (TLS, chat->id, chat->print_title);
  }
  if (! (flags & TGL_UPDATE_CREATED)) {
    PurpleChat *ch = tgp_blist_chat_find (TLS, chat->id);
    
    if (flags & TGL_UPDATE_TITLE && ch) {
      purple_blist_alias_chat (ch, chat->print_title);
    }
    if (flags & TGL_UPDATE_DELETED && ch) {
      purple_blist_remove_chat (ch);
    }
  }
}

static void update_user_typing (struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status) {
  g_return_if_fail (tgp_blist_peer_get_purple_name (TLS, U->id));
  if (status == tgl_typing_typing) {
    serv_got_typing (tls_get_conn (TLS), tgp_blist_peer_get_purple_name (TLS, U->id), 2, PURPLE_TYPING);
  }
}

static void update_marked_read (struct tgl_state *TLS, int num, struct tgl_message *list[]) {
  if (! purple_account_get_bool (tls_get_pa (TLS), TGP_KEY_DISPLAY_READ_NOTIFICATIONS, FALSE)) {
    return;
  }
  int i;
  for (i = 0; i < num; i++) {
    if (list[i]) {
      tgl_peer_id_t to_id;
      if (tgl_get_peer_type (list[i]->to_id) == TGL_PEER_USER &&
          tgl_get_peer_id (list[i]->to_id) == tgl_get_peer_id (TLS->our_id)) {
        to_id = list[i]->from_id;
      } else {
        to_id = list[i]->to_id;
      }
      tgp_msg_sys_out (TLS , _("Message marked as read.") , to_id , TRUE);
    }
  }
}

static void on_userpic_loaded (struct tgl_state *TLS, void *extra, int success, const char *filename) {
  struct download_desc *dld = extra;
  struct tgl_user *U = dld->data;
  tgl_peer_t *P = tgl_peer_get (TLS, dld->get_user_info_data->peer);
  
  if (!success || !P) {
    warning ("Can not load userpic for user %s %s", U->first_name, U->last_name);
    tgp_notify_on_error_gw (TLS, NULL, success);
    free (dld->get_user_info_data);
    free (dld);
    return;
  }
  
  int imgStoreId = p2tgl_imgstore_add_with_id (filename);
  if (imgStoreId > 0) {
    used_images_add (tls_get_data (TLS), imgStoreId);

    p2tgl_buddy_icons_set_for_user (tls_get_pa (TLS), P->id, filename);
    if (dld->get_user_info_data->show_info == 1) {
      purple_notify_userinfo (tls_get_conn (TLS), tgp_blist_peer_get_purple_name (TLS, P->id),
          p2tgl_notify_peer_info_new (TLS, P), NULL, NULL);
    }
  }
  free (dld->get_user_info_data);
  free (dld);
}

static void on_get_dialog_list_done (struct tgl_state *TLS, void *callback_extra, int success, int size,
    tgl_peer_id_t peers[], tgl_message_id_t *last_msg_id[], int unread_count[]) {
  info ("Fetched dialogue list of size: %d", size);
  int i;
  for (i = size - 1; i >= 0; i--) {
    tgl_peer_t *UC = tgl_peer_get (TLS, peers[i]);
    if (! UC) {
      g_warn_if_reached ();
      continue;
    }
    // our own contact shouldn't show up in our buddy list
    if (tgl_get_peer_id (UC->id) == tgl_get_peer_id (TLS->our_id)) {
      continue;
    }
    if (tgl_get_peer_type (UC->id) == TGL_PEER_USER) {
      if (! (UC->user.flags & TGLUF_DELETED)) {
        PurpleBuddy *buddy = tgp_blist_buddy_find (TLS, UC->id);
        if (! buddy) {
          info ("%s is in the dialogue list but not in the buddy list, add the user",
              tgp_blist_peer_get_purple_name (TLS, UC->id));
          buddy = tgp_blist_buddy_new (TLS, UC);
          purple_blist_add_buddy (buddy, NULL, tgp_blist_group_init (_("Telegram")), NULL);
          if (UC->user.photo_id) {
            info ("%s has the photo %lld, fetch it.", buddy->name, UC->user.photo_id);
            tgl_do_get_user_info (TLS, UC->id, 0, on_user_get_info, get_user_info_data_new (0, UC->id));
          }
        }
        p2tgl_prpl_got_user_status (TLS, UC->id, &UC->user.status);
      }
    } else if (tgl_get_peer_type (UC->id) == TGL_PEER_CHAT) {
      if (UC->chat.users_num > 0 &&
          purple_account_get_bool (tls_get_data (TLS)->pa, TGP_KEY_JOIN_GROUP_CHATS, TGP_DEFAULT_JOIN_GROUP_CHATS)) {
        PurpleChat *PC = tgp_blist_chat_find (TLS, UC->id);
        if (!PC) {
          PC = p2tgl_chat_new (TLS, &UC->chat);
          purple_blist_add_chat (PC, NULL, NULL);
        }
      }
    }
  }
}

void on_user_get_info (struct tgl_state *TLS, void *info_data, int success, struct tgl_user *U) {
  get_user_info_data *user_info_data = (get_user_info_data *)info_data;
  tgl_peer_t *P = tgl_peer_get (TLS, user_info_data->peer);
  g_return_if_fail (P);
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  if (!U->photo || U->photo->sizes_num == 0) {
    // No profile pic to load, display it right away
    if (user_info_data->show_info) {
      purple_notify_userinfo (tls_get_conn (TLS), tgp_blist_peer_get_purple_name (TLS, P->id),
          p2tgl_notify_peer_info_new (TLS, P), NULL, NULL);
    }
    g_free (user_info_data);
  } else {
    struct download_desc *dld = malloc (sizeof (struct download_desc));
    dld->data = U;
    dld->get_user_info_data = info_data;
    tgl_do_load_photo (TLS, U->photo, on_userpic_loaded, dld);
  }
}

static const char *tgprpl_list_icon (PurpleAccount *acct, PurpleBuddy *buddy) {
  return "telegram";
}

static void tgprpl_tooltip_text (PurpleBuddy *buddy, PurpleNotifyUserInfo *info, gboolean full) {
  
  // buddy in old format that didn't migrate
  if (! tgp_blist_buddy_has_id (buddy)) {
    return;
  }
  
  tgl_peer_t *P = tgl_peer_get (pbn_get_data (&buddy->node)->TLS, tgp_blist_buddy_get_id (buddy));
  g_return_if_fail (P);

  gchar *status = tgp_format_user_status (&P->user.status);
  purple_notify_user_info_add_pair (info, "last online: ", status);
  g_free (status);
}

static GList *tgprpl_status_types (PurpleAccount *acct) {
  debug ("tgprpl_status_types()");
  GList *types = NULL;
  PurpleStatusType *type;
  
  type = purple_status_type_new_full (PURPLE_STATUS_AVAILABLE, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);
  
  type = purple_status_type_new_full (PURPLE_STATUS_MOBILE, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);

  type = purple_status_type_new_full (PURPLE_STATUS_OFFLINE, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);
  
  /*
    The states below are only registered internally so that we get notified about 
    state changes to away and unavailable. This is useful for deciding when to send 
    No other peer should ever have those states.
   */
  type = purple_status_type_new_full (PURPLE_STATUS_AWAY, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);
  
  type = purple_status_type_new_full (PURPLE_STATUS_UNAVAILABLE, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);
  
  return g_list_reverse (types);
}

static void create_secret_chat_done (struct tgl_state *TLS, void *callback_extra, int success,
    struct tgl_secret_chat *E) {
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  write_secret_chat_file (TLS);
}

static void start_secret_chat (PurpleBlistNode *node, gpointer data) {
  tgl_do_create_secret_chat (pbn_get_data (node)->TLS, tgp_blist_buddy_get_id ((PurpleBuddy *)node),
      create_secret_chat_done, NULL);
}

static void create_chat_link_done (struct tgl_state *TLS, void *extra, int success, const char *url) {
  tgl_peer_t *C = extra;
  
  if (success) {
    assert (tgl_get_peer_type (C->id) == TGL_PEER_CHAT);
    tgp_chat_show (TLS, &C->chat);
    
    char *msg = g_strdup_printf (_("Invite link: %s"), url);
    serv_got_chat_in (tls_get_conn (TLS), tgl_get_peer_id (C->id), "WebPage", PURPLE_MESSAGE_SYSTEM, msg, time(NULL));
    g_free (msg);
  } else {
    tgp_notify_on_error_gw (TLS, NULL, success);
  }
}

static void create_chat_link (PurpleBlistNode *node, gpointer data) {
  PurpleChat *chat = (PurpleChat*)node;
  export_chat_link_checked (pbn_get_data (node)->TLS, purple_chat_get_name (chat));
}

void export_chat_link_checked (struct tgl_state *TLS, const char *name) {
  tgl_peer_t *C = tgl_peer_get_by_name (TLS, name);
  if (! C) {
    failure ("Chat \"%s\" not found, not exporting link.", name);
    return;
  }
  if (C->chat.admin_id != tgl_get_peer_id (TLS->our_id)) {
    purple_notify_error (_telegram_protocol, _("Creating chat link failed"), _("Creating chat link failed"),
        _("You need to be admin of the group  to do that."));
    return;
  }
  tgl_do_export_chat_link (TLS, C->id, create_chat_link_done, C);
}

void leave_and_delete_chat (struct tgl_state *TLS, tgl_peer_t *P) {
  g_return_if_fail (P);
  if (P && P->chat.users_num) {
    tgl_do_del_user_from_chat (TLS, P->id, TLS->our_id, tgp_notify_on_error_gw, NULL);
  }
  serv_got_chat_left (tls_get_conn (TLS), tgl_get_peer_id (P->id));
  PurpleChat *PC = tgp_blist_chat_find (TLS, P->id);
  if (PC) {
    purple_blist_remove_chat (PC);
  }
}

static void import_chat_link_done (struct tgl_state *TLS, void *extra, int success) {
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  purple_notify_info (_telegram_protocol, _("Chat joined"), _("Chat joined"),
      _("Chat added to list of chat rooms."));
}

void import_chat_link_checked (struct tgl_state *TLS, const char *link) {
  tgl_do_import_chat_link (TLS, link, (int) strlen(link), import_chat_link_done, NULL);
}

static GList* tgprpl_blist_node_menu (PurpleBlistNode *node) {
  debug ("tgprpl_blist_node_menu()");

  // orphaned buddy in "old" format that didn't migrate
  if (! tgp_blist_buddy_has_id ((PurpleBuddy *)node)) {
    return NULL;
  }
  
  GList* menu = NULL;
  if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
      tgl_get_peer_type (tgp_blist_buddy_get_id ((PurpleBuddy *)node)) == TGL_PEER_USER) {
    // Add encrypted chat option to the right click menu of all buddies
    PurpleBuddy* buddy = (PurpleBuddy*)node;
    PurpleMenuAction* action = purple_menu_action_new (_("Start secret chat..."), PURPLE_CALLBACK(start_secret_chat),
        buddy, NULL);
    menu = g_list_append (menu, (gpointer)action);
  }
  if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
     // Generate Public Link
    PurpleMenuAction* action = purple_menu_action_new (_("Invite users by link..."), PURPLE_CALLBACK(create_chat_link),
        NULL, NULL);
    menu = g_list_append (menu, (gpointer)action);
  }
  if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
    // Delete self from chat
    PurpleMenuAction* action = purple_menu_action_new (_("Delete and exit..."), PURPLE_CALLBACK(leave_and_delete_chat),
        NULL, NULL);
    menu = g_list_append (menu, (gpointer)action);
  }
  return menu;
}

static void update_on_logged_in (struct tgl_state *TLS) {
  info ("update_on_logged_in(): The account is signed in");
  write_auth_file (TLS);
  
  debug ("state: seq = %d, pts = %d, date = %d", TLS->seq, TLS->pts, TLS->date);
  purple_connection_set_state (tls_get_conn (TLS), PURPLE_CONNECTED);
  purple_blist_add_account (tls_get_pa (TLS));
  
  // It is important to load secret chats exactly at this point during login, cause if it was done earlier,
  // the update function wouldn't find existing chats and create duplicate entries. If it was done later, eventual
  // updates for those secret chats wouldn't be able to be decrypted due to missing secret keys.
  read_secret_chat_file (TLS);
}

static void update_on_ready (struct tgl_state *TLS) {
  info ("update_on_ready(): The account is done fetching new history");
  purple_connection_set_display_name (tls_get_conn (TLS), purple_account_get_username (tls_get_pa (TLS)));
  tgl_do_get_dialog_list (TLS, 200, 0, on_get_dialog_list_done, NULL);
  tgl_do_update_contact_list (TLS, 0, 0);
}

static void tgprpl_login (PurpleAccount * acct) {
  info ("tgprpl_login(): Purple is telling the prpl to connect the account");
  
  PurpleConnection *gc = purple_account_get_connection (acct);
  
  struct tgl_state *TLS = tgl_state_alloc ();
  connection_data *conn = connection_data_init (TLS, gc, acct);
  purple_connection_set_protocol_data (gc, conn);

  TLS->base_path = get_config_dir (purple_account_get_username (acct));
  tgl_set_download_directory (TLS, get_download_dir(TLS));
  debug ("base configuration path: '%s'", TLS->base_path);
  
  struct rsa_pubkey pubkey;
#ifdef WIN32
  gchar *global_pk_path = g_strdup_printf("%s/%s", DATADIR, pk_path);
#else
  gchar *global_pk_path = g_strdup(pk_path);
#endif
  debug ("trying global pubkey at %s", global_pk_path);
  gboolean global_pk_loaded = read_pubkey_file (global_pk_path, &pubkey);
  g_free(global_pk_path);

  tgl_set_verbosity (TLS, 4);
  if (global_pk_loaded) {
    info ("using global pubkey");
    tgl_set_rsa_key_direct (TLS, pubkey.e, pubkey.n_len, pubkey.n_raw);
  } else {
    char *user_pk_path = get_user_pk_path ();
    debug ("trying local pubkey at %s", user_pk_path);
    gboolean user_pk_loaded = read_pubkey_file (user_pk_path, &pubkey);

    if (user_pk_loaded) {
      info ("using local pubkey");
      tgl_set_rsa_key_direct (TLS, pubkey.e, pubkey.n_len, pubkey.n_raw);
    } else {
      failure ("both didn't work. abort.");
      char *cause = g_strdup_printf (_("Unable to sign on as %s: pubkey not found."),
                      purple_account_get_username (acct));
      purple_connection_error_reason (gc, PURPLE_CONNECTION_ERROR_INVALID_SETTINGS, cause);
      char *long_hint = g_strdup_printf (
        _("Make sure telegram-purple is installed properly,\n"
          "including the .tglpub file.\n"
          "If you're running SELinux (e.g. when using Tails),\n"
          "try 'make local_install', or simply copy\n"
          "%1$s to %2$s."), pk_path, user_pk_path);
      purple_notify_message (_telegram_protocol, PURPLE_NOTIFY_MSG_ERROR, cause,
                             long_hint, NULL, NULL, NULL);
      g_free (cause);
      g_free (long_hint);
      return;
    }
  }

  tgl_set_ev_base (TLS, conn);
  tgl_set_net_methods (TLS, &tgp_conn_methods);
  tgl_set_timer_methods (TLS, &tgp_timers);
  tgl_set_callback (TLS, &tgp_callback);
  tgl_register_app_id (TLS, TGP_APP_ID, TGP_APP_HASH);
  tgl_init (TLS);

  if (! tgp_startswith (purple_account_get_username (acct), "+")) {
        char *cause = g_strdup_printf (_("Unable to sign on as %s: phone number lacks country prefix."
                                         " Numbers must start with the full international"
                                         " prefix code, e.g. +49 for Germany."),
                        purple_account_get_username (acct));
    purple_connection_error_reason (gc, PURPLE_CONNECTION_ERROR_INVALID_SETTINGS, cause);
    purple_notify_message (_telegram_protocol, PURPLE_NOTIFY_MSG_ERROR, _("Incomplete phone number"),
        cause, NULL, NULL, NULL);
    g_free (cause);
    return;
  }

  read_auth_file (TLS);
  read_state_file (TLS);

  purple_connection_set_state (conn->gc, PURPLE_CONNECTING);
  tgl_login (TLS);
}

static void tgprpl_close (PurpleConnection *gc) {
  debug ("tgprpl_close()");
  connection_data_free (purple_connection_get_protocol_data (gc));
}

static int tgprpl_send_im (PurpleConnection *gc, const char *who, const char *message, PurpleMessageFlags flags) {
  debug ("tgprpl_send_im()");

  // workaround to support clients without the request API (request.h), see tgp-request.c:request_code()
  if (gc_get_data (gc)->request_code_data) {

    // OTR plugins may try to insert messages that don't contain the code
    if (tgp_startswith (message, "?OTR")) {
      info ("Fallback SMS auth, skipping OTR message: '%s'", message);
      return -1;
    }
    struct request_values_data *data = gc_get_data (gc)->request_code_data;
    data->callback (gc_get_tls (gc), &message, data->arg);
    free (data);

    gc_get_data (gc)->request_code_data = NULL;
    return 1;
  }

  // Make sure that to only send messages to an existing peer by searching it in the peer tree, to give immediate
  // feedback by returning an error-code in case the peer doesn't exist.
  tgl_peer_t *peer = tgp_blist_peer_find (gc_get_tls (gc), who);
  if (peer) {
    if (tgl_get_peer_type (peer->id) == TGL_PEER_ENCR_CHAT && peer->encr_chat.state != sc_ok) {
      warning ("secret chat not ready for sending messages or deleted");
      return -1;
    }
    return tgp_msg_send (gc_get_tls (gc), message, peer->id);
  }
  
  warning ("peer not found");
  return -1;
}

static unsigned int tgprpl_send_typing (PurpleConnection *gc, const char *who, PurpleTypingState typing) {
  debug ("tgprpl_send_typing()");
  tgl_peer_t *peer = tgp_blist_peer_find (gc_get_tls (gc), who);
  if (peer) {
    tgl_do_send_typing (gc_get_tls (gc), peer->id, typing == PURPLE_TYPING ? tgl_typing_typing : tgl_typing_cancel,
        0, 0);
    
    // when the other peer sees that the current user is typing it is obvious that the previous messages were read
    pending_reads_send_user (gc_get_tls (gc), peer->id);
  }
  return 0;
}

static void tgprpl_get_info (PurpleConnection *gc, const char *who) {
  debug ("tgprpl_get_info()");

  tgl_peer_t *peer = tgp_blist_peer_find (gc_get_data (gc)->TLS, who);
  if (peer) {
    get_user_info_data* info_data = get_user_info_data_new (1, peer->id);
    if (tgl_get_peer_type (peer->id) == TGL_PEER_ENCR_CHAT) {
      tgl_peer_t *parent_peer = tgp_encr_chat_get_partner (gc_get_tls (gc), &peer->encr_chat);
      if (parent_peer) {
        tgl_do_get_user_info (gc_get_tls (gc), parent_peer->id, 0, on_user_get_info, info_data);
      }
    } else {
      tgl_do_get_user_info (gc_get_tls (gc), peer->id, 0, on_user_get_info, info_data);
    }
  }
}

static void tgprpl_set_status (PurpleAccount *acct, PurpleStatus *status) {
  debug ("tgprpl_set_status(%s)", purple_status_get_name (status));
  debug ("tgprpl_set_status(currstatus=%s)", purple_status_get_name (purple_account_get_active_status (acct)));
  
  pending_reads_send_all (pa_get_data (acct)->TLS);
}

static void tgprpl_add_buddy (PurpleConnection *gc, PurpleBuddy *buddy, PurpleGroup *group) {
  tgl_peer_t *peer = tgl_peer_get_by_name (gc_get_tls (gc), buddy->name);
  if (peer) {
    _update_buddy (gc_get_tls (gc), peer, TGL_UPDATE_PHOTO);
    tgp_blist_buddy_set_id (buddy, peer->id);
    p2tgl_prpl_got_user_status (gc_get_tls (gc), peer->id, &peer->user.status);
  }
}

static void tgprpl_remove_buddy (PurpleConnection *gc, PurpleBuddy *buddy, PurpleGroup *group) {
  debug ("tgprpl_remove_buddy()");
  if (! buddy) {
    return;
  }

  tgl_peer_t *peer = tgp_blist_buddy_get_peer (buddy);
  if (peer) {
    if (tgl_get_peer_type (peer->id) == TGL_PEER_ENCR_CHAT) {
      /* TODO: implement the api call cancel secret chats. Currently the chat will only be marked as
       deleted on our side so that it won't be added on startup
       (when the secret chat file is loaded) */
      bl_do_peer_delete (gc_get_tls (gc), peer->encr_chat.id);
    }
  }
}

static void tgprpl_chat_invite (PurpleConnection *gc, int id, const char *message, const char *who) {
  debug ("tgprpl_chat_invite()");
  tgl_peer_t *chat = tgl_peer_get (gc_get_tls (gc), TGL_MK_CHAT (id));
  tgl_peer_t *user = tgp_blist_peer_find (gc_get_tls (gc), who);
  
  if (! chat || ! user) {
    purple_notify_error (_telegram_protocol, _("Cannot invite buddy to chat"), _("Cannot invite buddy to chat"),
        _("Specified user does not exist."));
    return;
  }
  
  tgl_do_add_user_to_chat (gc_get_tls (gc), chat->id, user->id, 0, tgp_notify_on_error_gw, chat);
}

static int tgprpl_send_chat (PurpleConnection *gc, int id, const char *message, PurpleMessageFlags flags) {
  debug ("tgprpl_send_chat()");
  int ret = tgp_msg_send (gc_get_tls (gc), message, TGL_MK_CHAT(id));
  if (ret != 0) {
    p2tgl_got_chat_in (gc_get_tls (gc), TGL_MK_CHAT(id), gc_get_tls (gc)->our_id, message, PURPLE_MESSAGE_RECV,
        time (NULL));
  }
  return ret;
}

static gboolean tgprpl_can_receive_file (PurpleConnection *gc, const char *who) {
  return TRUE;
}

PurplePlugin *_telegram_protocol = NULL;

static PurplePluginProtocolInfo prpl_info = {
  OPT_PROTO_NO_PASSWORD | OPT_PROTO_IM_IMAGE,
  NULL,                    // user_¡splits, initialized in tgprpl_init()
  NULL,                    // protocol_options, initialized in tgprpl_init()
  {
    "png",
    1,                     // min_width
    1,                     // min_height
    512,                   // max_width
    512,                   // max_height
    64000,                 // max_filesize
    PURPLE_ICON_SCALE_SEND,
  },
  tgprpl_list_icon,
  NULL,
  NULL,
  tgprpl_tooltip_text,
  tgprpl_status_types,
  tgprpl_blist_node_menu,
  tgprpl_chat_join_info,
  tgprpl_chat_info_defaults,
  tgprpl_login,
  tgprpl_close,
  tgprpl_send_im,
  NULL,                    // set_info
  tgprpl_send_typing,
  tgprpl_get_info,
  tgprpl_set_status,
  NULL,                    // set_idle
  NULL,                    // change_passwd
  tgprpl_add_buddy,
  NULL,                    // add_buddies
  tgprpl_remove_buddy,
  NULL,                    // remove_buddies
  NULL,                    // add_permit
  NULL,                    // add_deny
  NULL,                    // rem_permit
  NULL,                    // rem_deny
  NULL,                    // set_permit_deny
  tgprpl_chat_join,
  NULL,                    // reject_chat
  tgprpl_get_chat_name,
  tgprpl_chat_invite,
  NULL,                    // chat_leave
  NULL,                    // chat_whisper
  tgprpl_send_chat,
  NULL,                    // keepalive
  NULL,                    // register_user
  NULL,                    // get_cb_info
  NULL,                    // get_cb_away
  NULL,                    // alias_buddy
  NULL,                    // group_buddy
  NULL,                    // rename_group
  NULL,                    // buddy_free
  NULL,                    // convo_closed
  NULL,                    // normalize
  NULL,                    // tgprpl_set_buddy_icon
  NULL,                    // remove_group
  NULL,
  NULL,                    // set_chat_topic
  NULL,                    // find_blist_chat
  tgprpl_roomlist_get_list,
  tgprpl_roomlist_cancel,
  NULL,                    // roomlist_expand_category
  tgprpl_can_receive_file,
  tgprpl_send_file,       
  tgprpl_new_xfer,        
  NULL,                    // offline_message
  NULL,                    // whiteboard_prpl_ops
  NULL,                    // send_raw
  NULL,                    // roomlist_room_serialize
  NULL,                    // unregister_user
  NULL,                    // send_attention
  NULL,                    // get_attention_types
  sizeof(PurplePluginProtocolInfo),
  NULL,           		     // get_account_text_table
  NULL,                    // initiate_media
  NULL,                    // get_media_caps
  NULL,                    // get_moods
  NULL,                    // set_public_alias
  NULL,                    // get_public_alias
  NULL,                    // add_buddy_with_invite
  NULL                     // add_buddies_with_invite
};

static void tgprpl_init (PurplePlugin *plugin) {
  PurpleAccountOption *opt;
  
  // set domain name that is used for translation
#if ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
  
#define ADD_VALUE(list, desc, v) { \
  PurpleKeyValuePair *kvp = g_new0(PurpleKeyValuePair, 1); \
  kvp->key = g_strdup((desc)); \
  kvp->value = g_strdup((v)); \
  list = g_list_prepend(list, kvp); \
}
  
  // Login
  opt = purple_account_option_string_new (_("Password (two factor authentication)"), TGP_KEY_PASSWORD_TWO_FACTOR, NULL);
  purple_account_option_set_masked (opt, TRUE);
  prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, opt);
  
  opt = purple_account_option_bool_new (
      _("Fallback SMS verification\n(Helps when not using Pidgin and you aren't being prompted for the code)"),
      "compat-verification", 0);
  prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, opt);

  // Messaging
  GList *verification_values = NULL;
  ADD_VALUE(verification_values, _("ask"), "ask");
  ADD_VALUE(verification_values, _("always"), "always");
  ADD_VALUE(verification_values, _("never"), "never");
  
  opt = purple_account_option_list_new (_("Accept secret chats"),
      TGP_KEY_ACCEPT_SECRET_CHATS, verification_values);
  prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, opt);
  
  opt = purple_account_option_int_new (_("Display buddies offline after (days)"),
      TGP_KEY_INACTIVE_DAYS_OFFLINE, TGP_DEFAULT_INACTIVE_DAYS_OFFLINE);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);

  opt = purple_account_option_int_new (_("Don't fetch history older than (days)\n(0 for unlimited)"),
      TGP_KEY_HISTORY_RETRIEVAL_THRESHOLD, TGP_DEFAULT_HISTORY_RETRIEVAL_THRESHOLD);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);
  
  // Chats
  opt = purple_account_option_bool_new (_("Add all group chats to buddy list"),
      TGP_KEY_JOIN_GROUP_CHATS, TGP_DEFAULT_JOIN_GROUP_CHATS);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);

  // Read notifications
  opt = purple_account_option_bool_new (_("Display notices of receipt"),
      TGP_KEY_DISPLAY_READ_NOTIFICATIONS, TGP_DEFAULT_DISPLAY_READ_NOTIFICATIONS);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);
  
  opt = purple_account_option_bool_new (_("Send notices of receipt when present"),
      TGP_KEY_SEND_READ_NOTIFICATIONS, TGP_DEFAULT_SEND_READ_NOTIFICATIONS);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);
  
  _telegram_protocol = plugin;
}

static GList *tgprpl_actions (PurplePlugin *plugin, gpointer context) {
  return (GList *)NULL;
}

static PurplePluginInfo plugin_info = {
  PURPLE_PLUGIN_MAGIC,
  PURPLE_MAJOR_VERSION,
  PURPLE_MINOR_VERSION,
  PURPLE_PLUGIN_PROTOCOL,
  NULL,
  0,
  NULL,
  PURPLE_PRIORITY_DEFAULT,
  PLUGIN_ID,
  "Telegram",
  PACKAGE_VERSION "\n\t\t\tcommit: " GIT_COMMIT "\n\t\t\tlibtgl: " TGL_VERSION,
  "Telegram",
  N_("Telegram Protocol Plugin."),
  PLUGIN_AUTHOR,
  "https://github.com/majn/telegram-purple",
  NULL,           // on load
  NULL,           // on unload
  NULL,           // on destroy
  NULL,           // ui specific struct
  &prpl_info,
  NULL,           // prefs info
  tgprpl_actions,
  NULL,           // reserved
  NULL,           // reserved
  NULL,           // reserved
  NULL            // reserved
};

PURPLE_INIT_PLUGIN (telegram, tgprpl_init, plugin_info)
