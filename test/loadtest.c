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

 Copyright Ben Wiederhake 2016, Purple developers 2007-2015

 Derived from the nullclient.c example at
 https://github.com/Tasssadar/libpurple/blob/master/example/nullclient.c
 */

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <glib.h>
#include <purple.h>
#include <signal.h>
#include <string.h>

#include "../commit.h"
#include "../telegram-purple.h"

// The following eventloop functions are used in both pidgin and purple-text. If your application uses glib mainloop, you can safely use this verbatim.
#define PURPLE_GLIB_READ_COND  (G_IO_IN | G_IO_HUP | G_IO_ERR)
#define PURPLE_GLIB_WRITE_COND (G_IO_OUT | G_IO_HUP | G_IO_ERR | G_IO_NVAL)

typedef struct TdfGLibIOClosure {
  PurpleInputFunction function;
  guint result;
  gpointer data;
} TdfGLibIOClosure;

typedef struct TdfAccountRequestInfo {
  PurpleAccountRequestType type;
  PurpleAccount *account;
  void *ui_handle;
  char *user;
  gpointer userdata;
  PurpleAccountRequestAuthorizationCb auth_cb;
  PurpleAccountRequestAuthorizationCb deny_cb;
  guint ref;
} TdfAccountRequestInfo;

static void tdf_glib_io_destroy (gpointer data) {
  g_free (data);
}

static gboolean tdf_glib_io_invoke (GIOChannel *source, GIOCondition condition, gpointer data) {
  TdfGLibIOClosure *closure = data;
  PurpleInputCondition purple_cond = 0;

  if (condition & PURPLE_GLIB_READ_COND)
    purple_cond |= PURPLE_INPUT_READ;
  if (condition & PURPLE_GLIB_WRITE_COND)
    purple_cond |= PURPLE_INPUT_WRITE;

  closure->function (closure->data, g_io_channel_unix_get_fd (source), purple_cond);

  return TRUE;
}

static guint tdf_glib_input_add (gint fd, PurpleInputCondition condition, PurpleInputFunction function, gpointer data) {
  TdfGLibIOClosure *closure = g_new0(TdfGLibIOClosure, 1);
  GIOChannel *channel;
  GIOCondition cond = 0;

  closure->function = function;
  closure->data = data;

  if (condition & PURPLE_INPUT_READ)
    cond |= PURPLE_GLIB_READ_COND;
  if (condition & PURPLE_INPUT_WRITE)
    cond |= PURPLE_GLIB_WRITE_COND;

  channel = g_io_channel_unix_new (fd);
  closure->result = g_io_add_watch_full (channel, G_PRIORITY_DEFAULT, cond, tdf_glib_io_invoke, closure, tdf_glib_io_destroy);

  g_io_channel_unref (channel);
  return closure->result;
}

static PurpleEventLoopUiOps tdf_glib_eventloop_ops = {
  g_timeout_add,
  g_source_remove,
  tdf_glib_input_add,
  g_source_remove,
  NULL,
#if GLIB_CHECK_VERSION(2,14,0)
  g_timeout_add_seconds,
#else
  NULL,
#endif
  // padding
  NULL,
  NULL,
  NULL
};
// End of the eventloop functions.

static PurpleConnectionUiOps connection_uiops = {
    NULL,                      // connect_progress
    NULL,                      // connected
    NULL,                      // disconnected
    NULL,                      // notice
    NULL,                      // report_disconnect
    NULL,                      // network_connected
    NULL,                      // network_disconnected
    NULL,                      // report_disconnect_reason
    // padding
    NULL,
    NULL,
    NULL
};

static void tdf_init (void) {
  // This should initialize the UI components for all the modules.
  purple_connections_set_ui_ops (&connection_uiops);
}

static PurpleCoreUiOps tdf_core_uiops = {
  NULL,
  NULL,
  tdf_init,
  NULL,
  // padding
  NULL,
  NULL,
  NULL,
  NULL
};

PurplePlugin *tgp = NULL;

static void display_search_paths (GList *elem) {
  printf ("Search paths:");
  for (; elem; elem = elem->next) {
    printf (" %s", (char *)elem->data);
  }
  printf ("\n");
}

static void tdf_inject_plugin (void) {
  printf ("Injecting our module into purple_plugins_* ...\n");
  purple_plugins_init ();
  GList *search_paths = purple_plugins_get_search_paths ();
  assert (!search_paths->prev && !search_paths->next && search_paths->data);
  GList *new_paths = g_list_append (search_paths, g_strdup ("bin/"));
  assert (new_paths == search_paths);
  // Load "my" path before the default.
  assert (search_paths->next && search_paths->next->data);
  {
    // swap(search_paths->data, search_paths->next->data);
    gchar* tmp = search_paths->data;
    search_paths->data = search_paths->next->data;
    search_paths->next->data = tmp;
  }
}

gboolean tdf_is_in_ci (void) {
  gchar* envvar = getenv ("CONTINUOUS_INTEGRATION");
  return envvar && g_str_equal ("true", envvar);
}

static void init_libpurple (void) {
  // Set a custom user directory (optional)
  purple_util_set_user_dir ("test/tmp/user");

  // We *do* want debugging. However, this is just too much noise.
  // Then again, this doesn't hurt if Travis does it. Query the environment to check whether we're in CI or not.
  purple_debug_set_enabled (tdf_is_in_ci ());

  /* Set the core-uiops, which is used to
   * - initialize the ui specific preferences.
   * - initialize the debug ui.
   * - initialize the ui components for all the modules.
   * - uninitialize the ui components for all the modules when the core terminates.
   */
  purple_core_set_ui_ops (&tdf_core_uiops);

  // Set the uiops for the eventloop. Since our client is glib-based, you can safely copy this verbatim. */
  purple_eventloop_set_ui_ops (&tdf_glib_eventloop_ops);

  tdf_inject_plugin ();
  display_search_paths (purple_plugins_get_search_paths ());

  // Now that all the essential stuff has been set, let's try to init the core.
  printf ("Enter purple_core_init ...\n");
  if (!purple_core_init ("tgp-dummy")) {
    // Initializing the core failed. Terminate.
    fprintf (stderr, "libpurple initialization failed. Abort.\n");
    abort ();
  }
  printf ("Core initialized.\n");

  // Create and load the buddylist.
  purple_set_blist (purple_blist_new ());

  // Find our plugin
  printf ("Checking version:\n");
  tgp = purple_plugins_find_with_id (PLUGIN_ID);
  if (!tgp) {
    printf ("Huh! Can't find plugin! Expected ID \"" PLUGIN_ID "\"\n");
    abort ();
  }

  // Make sure we loaded the correct plugin
  printf ("This is what we loaded:\n\t\t\tversion: %s\n", tgp->info->version);
  gchar *pos = g_strrstr (tgp->info->version, "commit: " GIT_COMMIT);
  if (pos == NULL) {
    printf ("This is the wrong version, we built for commit %s. Abort!\n", GIT_COMMIT);
    abort ();
  } else {
    printf ("Looks like the right version, because we also expected commit %s.\n", GIT_COMMIT);
  }
}

//static void signed_on (PurpleConnection *gc) {
//  PurpleAccount *account = purple_connection_get_account (gc);
//  printf("Account connected: \"%s\" (%s)\n", purple_account_get_username(account), purple_account_get_protocol_id(account));
//}
//
//static void received_im_msg (PurpleAccount *account, char *sender, char *message, PurpleConversation *conv, PurpleMessageFlags flags) {
//  if (conv==NULL) {
//      conv = purple_conversation_new(PURPLE_CONV_TYPE_IM, account, sender);
//  }
//
//  printf("(%s) %s (%s): %s\n", purple_utf8_strftime("%H:%M:%S", NULL), sender, purple_conversation_get_name(conv), message);
//}
//
//static void connect_to_signals(void)
//{
//
//        static int handle;
//
//        purple_signal_connect(purple_connections_get_handle(), "signed-on", &handle,
//                                PURPLE_CALLBACK(signed_on), NULL);
//
//        purple_signal_connect(purple_conversations_get_handle(), "received-im-msg", &handle,
//                                PURPLE_CALLBACK(received_im_msg), NULL);
//
//}

int main(int argc, char *argv[])
{
  printf ("Running loadtest on %s.\n", argv[1]);
  GMainLoop *loop = g_main_loop_new(NULL, FALSE);

  /* libpurple's built-in DNS resolution forks processes to perform
   * blocking lookups without blocking the main process.  It does not
   * handle SIGCHLD itself, so if the UI does not you quickly get an army
   * of zombie subprocesses marching around.
   */
  signal(SIGCHLD, SIG_IGN);

  printf ("Enter init_libpurple\n");
  init_libpurple ();
  printf ("libpurple %s initialized.\n", purple_core_get_version());

  // Since we do not yet do anything useful, simply terminate after a few seconds.
  purple_timeout_add (2000, purple_core_quit_cb, NULL);
  g_timeout_add (4000, (GSourceFunc)g_main_loop_quit, loop);

//  connect_to_signals();

//  PurpleAccount *account = purple_account_new("YOUR_IM_ACCOUNTS_USERNAME_HERE", "prpl-IM_NETWORK_HERE"); //this could be prpl-aim, prpl-yahoo, prpl-msn, prpl-icq, etc.
//  purple_account_set_password(account, "YOUR_IM_ACCOUNTS_PASSWORD_HERE");
//  purple_accounts_add(account);
//  purple_account_set_enabled(account, UI_ID, TRUE);

  g_main_loop_run (loop);

  return 0;
}
