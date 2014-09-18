/**
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

#include <glib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

// Libpurple Plugin Includes
#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "account.h"
#include "accountopt.h"
#include "blist.h"
#include "cmds.h"
#include "conversation.h"
#include "connection.h"
#include "debug.h"
#include "privacy.h"
#include "prpl.h"
#include "roomlist.h"
#include "status.h"
#include "util.h"
#include "prpl.h"
#include "prefs.h"
#include "util.h"
#include "eventloop.h"
#include "request.h"

// telegram-purple includes
#include "telegram.h"
#include "telegram-purple.h"
#include "structures.h"
#include "net.h"

#define BUDDYNAME_MAX_LENGTH 128

static PurplePlugin *_telegram_protocol = NULL;
PurpleGroup *tggroup;

void tgprpl_login_on_connected();

/**
 * Formats the given format string with the given arguments and writes 
 * it to the libpurple log
 */
void tg_cli_log_cb(const char* format, va_list ap)
{
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, ap);
    purple_debug_info(PLUGIN_ID, "%s", buffer);
}

void message_allocated_handler (struct telegram *instance, struct message *M);
void peer_allocated_handler (struct telegram *instance, void *user);
void telegram_on_phone_registration (struct telegram *instance);
void telegram_on_client_registration (struct telegram *instance);
void on_new_user_status(struct telegram *instance, void *user);
void on_user_typing(struct telegram *instance, void *user);
void on_chat_joined (struct telegram *instance, peer_id_t chatid);
static PurpleChat *blist_find_chat_by_id(PurpleConnection *gc, const char *id);
static void tgprpl_has_output(void *handle);

static const char *chat_id_get_comp_val (PurpleConnection *gc, int id, char *value)
{
    gchar *name = g_strdup_printf ("%d", id);
    PurpleChat *ch = blist_find_chat_by_id(gc, name);
    g_free (name);
    GHashTable *table = purple_chat_get_components(ch);
    return g_hash_table_lookup(table, value);
}

/**
 * Assure that the given chat is opened
 */
static PurpleConversation *chat_show (PurpleConnection *gc, int id)
{
    logprintf ("show chat");
    PurpleConversation *convo = purple_find_chat(gc, id);
    if (convo) {
        if (purple_conv_chat_has_left(PURPLE_CONV_CHAT(convo)))
        {
            serv_got_joined_chat(gc, id, chat_id_get_comp_val(gc, id, "subject"));
        }
    } else {
        // join chat first
        logprintf ("joining chat first...\n");
        telegram_conn *conn = purple_connection_get_protocol_data(gc);
        do_get_chat_info (conn->tg, MK_CHAT(id));
        telegram_flush (conn->tg);
    }
    return convo;
}

static PurpleChat *get_chat_by_id (PurpleConnection *gc, int id)
{
    gchar *name = g_strdup_printf ("%d", id);
    PurpleChat *chat = blist_find_chat_by_id (gc, name);
    g_free (name);
    return chat;
}

/**
 * Returns the base icon name for the given buddy and account.
 * If buddy is NULL and the account is non-NULL, it will return the
 * name to use for the account's icon. If both are NULL, it will
 * return the name to use for the protocol's icon.
 *
 * This must be implemented.
 */
static const char *tgprpl_list_icon(PurpleAccount * acct, PurpleBuddy * buddy)
{
    //purple_debug_info(PLUGIN_ID, "tgrpl_list_icon()\n");
    return "telegram";
}

static telegram_conn *get_conn_from_buddy (PurpleBuddy *buddy)
{
     telegram_conn *c = purple_connection_get_protocol_data (
        purple_account_get_connection (purple_buddy_get_account (buddy)));
     return c;
}

/**
 * Allows the prpl to add text to a buddy's tooltip.
 */
static void tgprpl_tooltip_text(PurpleBuddy * buddy, PurpleNotifyUserInfo * info, gboolean full)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_tooltip_text()\n");

    logprintf ("purple_buddy_get_protocol_data: %s\n", buddy->name);
	peer_id_t *peer = purple_buddy_get_protocol_data(buddy);
	if(peer == NULL)
	{
		purple_notify_user_info_add_pair_plaintext(info, "Status", "Offline");
		return;
	}
	peer_t *P = user_chat_get (get_conn_from_buddy(buddy)->tg->bl, *peer);
	purple_notify_user_info_add_pair_plaintext(info, "Status", P->user.status.online == 1 ? "Online" : "Offline");
	struct tm *tm = localtime ((void *)&P->user.status.when);
	char buffer [21];
	sprintf  (buffer, "[%04d/%02d/%02d %02d:%02d:%02d]", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	purple_notify_user_info_add_pair_plaintext(info, "Last seen: ", buffer);
}


/**
 * Handle a failed verification by removing the invalid sms code and notifying the user
 */
static void login_verification_fail(PurpleAccount *acct)
{
    // remove invalid sms code, so we won't try to register again
    purple_account_set_string(acct, "verification_key", "");
    purple_notify_message(_telegram_protocol, PURPLE_NOTIFY_MSG_INFO, "Verification Failed", 
        "Please make sure you entered the correct verification code.", NULL, NULL, NULL);
}

/*  OUTPUT  */

/**
 * Libpurple announced that new output should be written to the write-handle
 */
static void tgprpl_output_cb(gpointer data, gint source, PurpleInputCondition cond)
{
    mtproto_handle *conn = data;
    if (!conn->mtp) {
        logprintf ("connection no loner existing, do nothing. \n");
        return;
    }
    if (mtp_write_output(conn->mtp) == 0) {
        logprintf("no output, removing output...\n");
        purple_input_remove(conn->wh);
        conn->wh = 0;
    }
}

/**
 * Telegram announced new output in its buffers
 */
static void tgprpl_has_output(void *handle)
{
    logprintf("tgprpl_has_output(%p)\n", handle);
    mtproto_handle *conn = handle;
    if (! conn->wh) {
        conn->wh = purple_input_add(conn->fd, PURPLE_INPUT_WRITE, tgprpl_output_cb, handle);
    }
}

/*
 * Libpurple announced that new input should be read from the read-handle
 */
static void tgprpl_input_cb(gpointer data, gint source, PurpleInputCondition cond)
{
    mtproto_handle *conn = data;
    logprintf("tgprpl_input_cb()\n");
    if (!conn->fd) {
        logprintf("conn for handle no longer existing, not reading input\n");
        return;
    }
    mtp_read_input(conn->mtp);

    if (conn->mtp) {
        // processing of the answer may have inserted new queries
        telegram_flush (conn->mtp->instance);

        // free all mtproto_connections that may have errored
        mtproto_free_closed(conn->mtp->instance);
    }
}

/** 
 * Telegram announced that it awaits new input from the read-handle
 * TODO: this is currently unused, evaluate wether its needed at all
 */
static void tgprpl_has_input(void *handle)
{
    logprintf("tgprpl_has_input()\n");
    mtproto_handle *conn = handle;
    if (! conn->rh) {
        conn->rh = purple_input_add(conn->fd, PURPLE_INPUT_READ, tgprpl_input_cb, handle);
        logprintf("Attached read handle: %u ", conn->rh);
    }
}

static void init_dc_settings(PurpleAccount *acc, struct dc *DC)
{
    DC->port = purple_account_get_int(acc, "port", TELEGRAM_DEFAULT_PORT);
    DC->ip = g_strdup(purple_account_get_string(acc, "server", TELEGRAM_TEST_SERVER));
    DC->id = 0;
}

/**
 * Telegram requests a new connectino to our configured proxy
 */
void telegram_on_proxy_request(struct telegram *tg, const char *ip, int port)
{
    telegram_conn *conn = tg->extra;
    purple_proxy_connect (conn->gc, conn->pa, ip, port, tgprpl_login_on_connected, tg);
}

/**
 * Handle a proxy-close of telegram
 * 
 * Remove all open inputs added to purple
 */
void telegram_on_proxy_close(void *handle)
{
    mtproto_handle *conn = handle; 
    logprintf ("Closing proxy-handles - fd: %d, write-handle: %d, read-handle: %d\n", 
        conn->fd, conn->wh, conn->rh);
    if (conn->rh) {
        purple_input_remove (conn->rh);
    }
    if (conn->wh) {
        purple_input_remove (conn->wh);
    }
    conn->rh = conn->wh = 0;
    conn->mtp = 0;
    tfree (conn, sizeof(mtproto_handle));
}

void telegram_on_phone_registration (struct telegram *instance)
{
    telegram_conn *conn = instance->extra;

    // TODO: Request first and last name
    purple_debug_info(PLUGIN_ID, "Phone is not registered, registering...\n");
    const char *first_name = purple_account_get_string(conn->pa, "first_name", NULL);
    const char *last_name  = purple_account_get_string(conn->pa, "last_name", NULL);
    const char *code = purple_account_get_string(conn->pa, "verification_key", NULL);

    if (!first_name || !last_name || !strlen(first_name) > 0 || !strlen(last_name) > 0) {
        purple_notify_message(_telegram_protocol, PURPLE_NOTIFY_MSG_INFO, "Registration Needed", 
            "Enter your first and last name to register this phone number with the telegram network.", 
            NULL, NULL, NULL);
        return;
    }

    do_send_code_result_auth (instance, code, first_name, last_name);
}

void client_registration_entered (gpointer data, const gchar *code)
{
    struct telegram *tg = data;
    do_send_code_result (tg, code);
    telegram_flush (tg);
}

void client_registration_canceled (gpointer data)
{
    struct telegram *tg = data;
    // TODO: disconnect and exit
}

gboolean queries_timerfunc (gpointer data) {
   logprintf ("queries_timerfunc()\n");
   telegram_conn *conn = data;
   work_timers (conn->tg);

   if (conn->updated) {
       logprintf ("State updated, storing current session...\n");
       conn->updated = 0;
       telegram_store_session (conn->tg);
   }
   return 1;
}

void telegram_on_client_registration (struct telegram *instance)
{
    purple_debug_info(PLUGIN_ID, "Client is not registered, registering...\n");
    telegram_conn *conn = instance->extra;

    purple_request_input(
        conn->gc, // handle   (the PurpleAccount) 
        "Telegram Code",     // title 
        "Enter Telegram Code", // primary 
        "Telegram wants to verify your identity, please enter the code, that you have received via SMS.", // secondary 
        NULL,     // default_value 
        FALSE,    // multiline     
        FALSE,    // masked         
        "code",   // hint 
        "OK",     // ok_text      
        G_CALLBACK(client_registration_entered), // ok_cb 
        "Cancel", // cancel_text 
        G_CALLBACK(client_registration_canceled), // cancel_cb 
        conn->pa, // account    
        NULL,     // who 
        NULL,     // conv  
        conn->tg  // user_data      
    );
}

void telegram_on_ready (struct telegram *instance)
{
     purple_debug_info(PLUGIN_ID, "telegram_on_ready().\n");
     telegram_conn *conn = instance->extra;
     purple_connection_set_state(conn->gc, PURPLE_CONNECTED);
     purple_connection_set_display_name(conn->gc, purple_account_get_username(conn->pa));
     purple_blist_add_account(conn->pa);
     tggroup = purple_find_group("Telegram");
     if (tggroup == NULL) {
         purple_debug_info (PLUGIN_ID, "PurpleGroup = NULL, creating");
         tggroup = purple_group_new ("Telegram");
         purple_blist_add_group (tggroup, NULL);
     }
     do_get_difference(instance, 0);
     do_get_dialog_list(instance);
     do_update_contact_list(instance);
     telegram_flush (conn->tg);
     conn->timer = purple_timeout_add (5000, queries_timerfunc, conn);
}

/**
 * A proxy connection was created by purple
 *
 * Use the connection to create a new mtproto-connection and create a handle,
 * with additional info for libpurple associated with the new connection
 */
void tgprpl_login_on_connected(gpointer *data, gint fd, const gchar *error_message)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_login_on_connected()\n");
    struct telegram *tg = (struct telegram*)data;
    if (fd == -1) {
        logprintf("purple_proxy_connect failed: %s\n", error_message);
        telegram_destroy(tg);
        return;
    }

    mtproto_handle *conn = talloc(sizeof (mtproto_handle));
    conn->fd = fd;
    conn->wh = purple_input_add(fd, PURPLE_INPUT_WRITE, tgprpl_output_cb, conn);
    conn->rh = purple_input_add(fd, PURPLE_INPUT_READ, tgprpl_input_cb, conn);
    conn->mtp = telegram_add_proxy(tg, fd, conn);
}

void telegram_on_disconnected (struct telegram *tg)
{
     logprintf ("telegram_on_disconnected()\n");
     assert (0);
}

struct telegram_config tgconf = {
    NULL,
    tgprpl_has_output, // on output
    telegram_on_proxy_request,
    telegram_on_proxy_close,
    telegram_on_phone_registration,
    telegram_on_client_registration,
    telegram_on_ready,
    telegram_on_disconnected,

    message_allocated_handler,
    peer_allocated_handler,
    on_new_user_status,
    on_user_typing,
    on_chat_joined
};


/**
 * This must be implemented.
 */
static void tgprpl_login(PurpleAccount * acct)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_login()\n");
    PurpleConnection *gc = purple_account_get_connection(acct);
    char const *username = purple_account_get_username(acct);
    struct dc DC;
    init_dc_settings(acct, &DC);
    
    // create a new instance of telegram
    struct telegram *tg = telegram_new (&DC, username, &tgconf); 
    telegram_restore_session(tg);
   
    // create handle to store additional info for libpurple in
    // the new telegram instance
    telegram_conn *conn = g_new0(telegram_conn, 1);
    conn->tg = tg;
    conn->gc = gc;
    conn->pa = acct;
    purple_connection_set_protocol_data(gc, conn);
    tg->extra = conn;
    purple_connection_set_state (conn->gc, PURPLE_CONNECTING);
    telegram_connect (tg);
}

/**
 * This must be implemented.
 */
static void tgprpl_close(PurpleConnection * gc)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_close()\n");
    telegram_conn *conn = purple_connection_get_protocol_data(gc);
    purple_timeout_remove(conn->timer);
    telegram_destroy(conn->tg);
}

void message_allocated_handler(struct telegram *tg, struct message *M)
{
    logprintf ("message_allocated_handler\n");
    telegram_conn *conn = tg->extra;
    PurpleConnection *gc = conn->gc;

    // TODO: this should probably be freed again somwhere
    int id = get_peer_id(M->from_id);

    if (M->service) {
       // TODO: handle service messages properly, currently adding them 
       // causes a segfault for an unknown reason
       logprintf ("service message, skipping...\n");
       return;
    }

    //
    peer_id_t to_id = M->to_id;
    char *from = g_strdup_printf("%d", id);
    char *to = g_strdup_printf("%d", to_id.id);
    switch (to_id.type) {
        case PEER_CHAT:
            logprintf ("PEER_CHAT\n");
            chat_show (gc, to_id.id);
            if (M->from_id.id == tg->our_id) {
                serv_got_chat_in(gc, get_peer_id(M->to_id), "You", PURPLE_MESSAGE_RECV, M->message, time((time_t *) &M->date));
            } else {
                peer_t *fromPeer = user_chat_get (tg->bl, M->from_id);
                char *alias = malloc(BUDDYNAME_MAX_LENGTH);
                user_get_alias(fromPeer, alias, BUDDYNAME_MAX_LENGTH);
                serv_got_chat_in(gc, get_peer_id(M->to_id), alias, PURPLE_MESSAGE_RECV, M->message, time((time_t *) &M->date));
                g_free(alias);
            }
        break;

        case PEER_USER:
            logprintf ("PEER_USER\n");
            if (M->from_id.id == tg->our_id) {
                serv_got_im(gc, to, M->message, PURPLE_MESSAGE_SEND, time((time_t *) &M->date));
            } else {
                serv_got_im(gc, from, M->message, PURPLE_MESSAGE_RECV, time((time_t *) &M->date));
            }
        break;

        case PEER_ENCR_CHAT:
        break;

        case PEER_GEO_CHAT:
        break;
    }
    g_free(from);
    conn->updated = 1;
}

void on_new_user_status(struct telegram *tg, void *peer)
{
    telegram_conn *conn = tg->extra;
    peer_t *p = peer;

    //  purple_debug_info(PLUGIN_ID, "New User Status: %s\n", peer->user.status.online);
    // TODO: this should probably be freed again somwhere
    char *who = g_strdup_printf("%d", get_peer_id(p->user.id));
   
    PurpleAccount *account = purple_connection_get_account(conn->gc);
    if (p->user.status.online == 1) 
        purple_prpl_got_user_status(account, who, "available", "message", "", NULL);
    else
        purple_prpl_got_user_status(account, who, "unavailable", "message", "", NULL);
    g_free(who);
}

void on_user_typing(struct telegram *tg, void *peer)
{
    telegram_conn *conn = tg->extra;
    peer_t *p = peer;

    char *who = g_strdup_printf("%d", get_peer_id(p->user.id));
    serv_got_typing(conn->gc, who, 2, PURPLE_TYPING);
    g_free(who);
}

/*
 * Search chats in hash table
 *
 * TODO: There has to be an easier way to do this
 */
static PurpleChat *blist_find_chat_by_hasht_cond(PurpleConnection *gc, int (*fn)(GHashTable *hasht, void *data), void *data)
{
    PurpleAccount *account = purple_connection_get_account(gc);
    PurpleBlistNode *node = purple_blist_get_root();
    GHashTable *hasht;
    while (node) {
        if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
            PurpleChat *ch = PURPLE_CHAT(node);
            if (purple_chat_get_account(ch) == account) {
                hasht = purple_chat_get_components(ch);
                if (fn(hasht, data))
                    return ch;
            }
        }
        node = purple_blist_node_next(node, FALSE);
    }
    return NULL;
}
static int hasht_cmp_id(GHashTable *hasht, void *data)
{
    return !strcmp(g_hash_table_lookup(hasht, "id"), *((char **)data));
}
static PurpleChat *blist_find_chat_by_id(PurpleConnection *gc, const char *id)
{
    return blist_find_chat_by_hasht_cond(gc, hasht_cmp_id, &id);
}

static char *peer_get_peer_id_as_string(peer_t *user)
{
    return g_strdup_printf("%d", get_peer_id(user->id));
}

void peer_allocated_handler(struct telegram *tg, void *usr)
{
    telegram_conn *conn = tg->extra;
    PurpleConnection *gc = conn->gc;
    PurpleAccount *pa = conn->pa;

    peer_t *user = usr;
    gchar *name = peer_get_peer_id_as_string(user); //g_strdup_printf("%d", get_peer_id(user->id));
    logprintf("Allocated peer: %s\n", name);
    switch (user->id.type) {
        case PEER_USER: {
            logprintf("Peer type: user.\n");
            // TODO: this should probably be freed again somwhere
            char *alias = malloc(BUDDYNAME_MAX_LENGTH);
            if (user_get_alias(user, alias, BUDDYNAME_MAX_LENGTH) < 0) {
                purple_debug_info(PLUGIN_ID, "Buddyalias of (%d) too long, not adding to buddy list.\n", 
                    get_peer_id(user->id));
                return;
            }
            PurpleBuddy *buddy = purple_find_buddy(pa, name);
            if (!buddy) { 
                char *actual = user->id.id == tg->our_id ? "You" : alias;
                purple_debug_info(PLUGIN_ID, "Adding %s to buddy list\n", name);
                purple_debug_info(PLUGIN_ID, "Alias %s\n", actual);
                buddy = purple_buddy_new(pa, name, actual);
                purple_blist_add_buddy(buddy, NULL, tggroup, NULL);
            }
            purple_buddy_set_protocol_data(buddy, (gpointer)&user->id);
            
            PurpleAccount *account = purple_connection_get_account(gc);
            if (user->user.status.online == 1)
                purple_prpl_got_user_status(account, name, "available", "message", "", NULL);
            else
                purple_prpl_got_user_status(account, name, "unavailable", "message", "", NULL);
        
            g_free(alias);
            g_free(name);
        }
        break;
        case PEER_CHAT: {
            logprintf("Peer type: chat.\n");
            PurpleChat *ch = blist_find_chat_by_id(gc, name);
            if (!ch) {
                gchar *admin = g_strdup_printf("%d", user->chat.admin_id);
                GHashTable *htable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
                g_hash_table_insert(htable, g_strdup("subject"), user->chat.title);
                g_hash_table_insert(htable, g_strdup("id"), name);
                g_hash_table_insert(htable, g_strdup("owner"), admin);
                logprintf("Adding chat to blist: %s (%s, %s)\n", user->chat.title, name, admin);
                ch = purple_chat_new(pa, user->chat.title, htable);
                purple_blist_add_chat(ch, NULL, NULL);
            }
            
            GHashTable *gh = purple_chat_get_components(ch);
            //char const *id = g_hash_table_lookup(gh, "id");
            char const *owner = g_hash_table_lookup(gh, "owner");
    
            PurpleConversation *conv = purple_find_chat(gc, atoi(name));

            purple_conv_chat_clear_users(purple_conversation_get_chat_data(conv));
            if (conv) {
                struct chat_user *usr = user->chat.user_list;
                int size = user->chat.user_list_size;
                int i;
                for (i = 0; i < size; i++) {
                    struct chat_user *cu = (usr + i);
                    // TODO: Inviter ID
                    // peer_id_t u = MK_USER (cu->user_id);
                    // peer_t *uchat = user_chat_get(u);
                    const char *cuname = g_strdup_printf("%d", cu->user_id);
                    logprintf("Adding user %s to chat %s\n", cuname, name);
                    purple_conv_chat_add_user(purple_conversation_get_chat_data(conv), cuname, "", 
                        PURPLE_CBFLAGS_NONE | (!strcmp(owner, cuname) ? PURPLE_CBFLAGS_FOUNDER : 0), FALSE);
                }
            }
        }
        break;
        case PEER_GEO_CHAT:
            logprintf("Peer type: geo-chat.\n");
        break;
        case PEER_ENCR_CHAT:
            logprintf("Peer type: encrypted chat.\n");
        break;
        case PEER_UNKNOWN:
            logprintf("Peer type: unknown.\n");
        break;
    }
}

/**
 * This PRPL function should return a positive value on success.
 * If the message is too big to be sent, return -E2BIG.  If
 * the account is not connected, return -ENOTCONN.  If the
 * PRPL is unable to send the message for another reason, return
 * some other negative value.  You can use one of the valid
 * errno values, or just big something.  If the message should
 * not be echoed to the conversation window, return 0.
 */
static int tgprpl_send_im(PurpleConnection * gc, const char *who, const char *message, PurpleMessageFlags flags)
{
    telegram_conn *conn = purple_connection_get_protocol_data(gc);
    PurpleAccount *pa = conn->pa;

    purple_debug_info (PLUGIN_ID, "tgprpl_send_im()\n");
    PurpleBuddy *b = purple_find_buddy (pa, who);
    peer_id_t *peer = purple_buddy_get_protocol_data (b);
    do_send_message (conn->tg, *peer, message, strlen(message));
    telegram_flush (conn->tg);
    return 1;
}

/**
 * Send a message to a chat.
 * This PRPL function should return a positive value on success.
 * If the message is too big to be sent, return -E2BIG.  If
 * the account is not connected, return -ENOTCONN.  If the
 * PRPL is unable to send the message for another reason, return
 * some other negative value.  You can use one of the valid
 * errno values, or just big something.
 *
 * @param id      The id of the chat to send the message to.
 * @param message The message to send to the chat.
 * @param flags   A bitwise OR of #PurpleMessageFlags representing
 *                message flags.
 * @return       A positive number or 0 in case of success,
 *                a negative error number in case of failure.
 */
static int tgprpl_send_chat(PurpleConnection * gc, int id, const char *message, PurpleMessageFlags flags)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_send_chat()\n");
    telegram_conn *conn = purple_connection_get_protocol_data (gc);
    //PurpleConversation *convo = purple_find_chat(gc, id);
    do_send_message (conn->tg, MK_CHAT(id), message, strlen(message));

    char *who = g_strdup_printf("%d", id);
    serv_got_chat_in(gc, id, "You", PURPLE_MESSAGE_RECV, message, time(NULL));
    g_free(who);
    return 1;
}

/**
 * Add a buddy to a group on the server.
 *
 * This PRPL function may be called in situations in which the buddy is
 * already in the specified group. If the protocol supports
 * authorization and the user is not already authorized to see the
 * status of \a buddy, \a add_buddy should request authorization.
 *
 * @deprecated Since 2.8.0, add_buddy_with_invite is preferred.
 * @see add_buddy_with_invite
 */
static void tgprpl_add_buddy(PurpleConnection * gc, PurpleBuddy * buddy, PurpleGroup * group)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_add_buddy()\n");
}

static void tgprpl_add_buddies(PurpleConnection * gc, GList * buddies, GList * groups)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_add_buddies()\n");
}

static void tgprpl_remove_buddy(PurpleConnection * gc, PurpleBuddy * buddy, PurpleGroup * group)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_remove_buddy()\n");
}

static void tgprpl_remove_buddies(PurpleConnection * gc, GList * buddies, GList * groups)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_remove_buddies()\n");

}

static void tgprpl_convo_closed(PurpleConnection * gc, const char *who)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_convo_closed()\n");

}

static void tgprpl_add_deny(PurpleConnection * gc, const char *name)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_add_deny()\n");

}

static void tgprpl_rem_deny(PurpleConnection * gc, const char *name)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_rem_deny()\n");

}

/**
 * @return If this protocol requires the PURPLE_TYPING message to
 *         be sent repeatedly to signify that the user is still
 *         typing, then the PRPL should return the number of
 *         seconds to wait before sending a subsequent notification.
 *         Otherwise the PRPL should return 0.
 */
static unsigned int tgprpl_send_typing(PurpleConnection * gc, const char *who, PurpleTypingState typing)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_send_typing()\n");
    telegram_conn *conn = purple_connection_get_protocol_data(gc);
    PurpleBuddy *b = purple_find_buddy(conn->pa, who);
    if (b) {
        peer_id_t *peer = purple_buddy_get_protocol_data(b);
        if (peer) {
            do_update_typing (conn->tg, *peer);
        }
    }
    return 0;
}

/**
 * Set the buddy icon for the given connection to @a img.  The prpl
 * does NOT own a reference to @a img; if it needs one, it must
 * #purple_imgstore_ref(@a img) itself.
 */
static void tgprpl_set_buddy_icon(PurpleConnection * gc, PurpleStoredImage * img)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_set_buddy_icon()\n");

}

/**
 * File transfer callback
 */
static gboolean tgprpl_can_receive_file(PurpleConnection * gc, const char *who)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_can_receive_file()\n");
    return FALSE;
}

/**
 *  Checks whether offline messages to @a buddy are supported.

 *  @return @c TRUE if @a buddy can be sent messages while they are
 *          offline, or @c FALSE if not.
 */
static gboolean tgprpl_offline_message(const PurpleBuddy * buddy)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_offline_message()\n");
    return FALSE;
}

/**
 * Returns a list of #PurpleStatusType which exist for this account;
 * this must be implemented, and must add at least the offline and
 * online states.
 */
static GList *tgprpl_status_types(PurpleAccount * acct)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_status_types()\n");
    GList *types = NULL;
    PurpleStatusType *type;
    type = purple_status_type_new_with_attrs(PURPLE_STATUS_AVAILABLE, "available", NULL, 
        TRUE, TRUE, FALSE, "message", "Message", purple_value_new(PURPLE_TYPE_STRING), NULL);
    types = g_list_prepend(types, type);

    type = purple_status_type_new_with_attrs(PURPLE_STATUS_AWAY, "unavailable", NULL, TRUE, 
        TRUE, FALSE, "message", "Message", purple_value_new(PURPLE_TYPE_STRING), NULL);
    types = g_list_prepend(types, type);

    type = purple_status_type_new(PURPLE_STATUS_OFFLINE, NULL, NULL, TRUE);
    types = g_list_append(types, type);

    return g_list_reverse(types);
}

static void tgprpl_set_status(PurpleAccount * acct, PurpleStatus * status)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_set_status()\n");

}

/**
 * Should arrange for purple_notify_userinfo() to be called with
 * @a who's user info.
 */
static void tgprpl_get_info(PurpleConnection * gc, const char *username)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_get_info()\n");

}

/**
 * change a buddy's group on a server list/roster
 */
static void tgprpl_group_buddy(PurpleConnection * gc, const char *who, const char *old_group, const char *new_group)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_group_buddy()\n");

}

/**
 * rename a group on a server list/roster
 */
static void tgprpl_rename_group(PurpleConnection * gc, const char *old_name, PurpleGroup * group, GList * moved_buddies)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_rename_group()\n");

}

/**
 * Returns a list of #proto_chat_entry structs, which represent
 * information required by the PRPL to join a chat. libpurple will
 * call join_chat along with the information filled by the user.
 *
 * @return A list of #proto_chat_entry structs
 */
static GList *tgprpl_chat_join_info(PurpleConnection * gc)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_chat_join_info()\n");
    struct proto_chat_entry *pce;

    pce = g_new0(struct proto_chat_entry, 1);
    pce->label = "_Subject:";
    pce->identifier = "subject";
    pce->required = TRUE;
    return g_list_append(NULL, pce);
}

/**
 * Returns a hashtable which maps #proto_chat_entry struct identifiers
 * to default options as strings based on chat_name. The resulting
 * hashtable should be created with g_hash_table_new_full(g_str_hash,
 * g_str_equal, NULL, g_free);. Use #get_chat_name if you instead need
 * to extract a chat name from a hashtable.
 *
 * @param chat_name The chat name to be turned into components
 * @return Hashtable containing the information extracted from chat_name
 */
static GHashTable *tgprpl_chat_info_defaults(PurpleConnection * gc, const char *chat_name)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_chat_info_defaults()\n");
    return NULL;
}

/**
 * Called when the user requests joining a chat. Should arrange for
 * #serv_got_joined_chat to be called.
 *
 * @param components A hashtable containing information required to
 *                   join the chat as described by the entries returned
 *                   by #chat_info. It may also be called when accepting
 *                   an invitation, in which case this matches the
 *                   data parameter passed to #serv_got_chat_invite.
 */
static void tgprpl_chat_join(PurpleConnection * gc, GHashTable * data)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_chat_join()\n");

    telegram_conn *conn = purple_connection_get_protocol_data(gc);
    const char *groupname = g_hash_table_lookup(data, "subject");

    char *id = g_hash_table_lookup(data, "id");
    if (!id) { 
        logprintf ("Got no chat id, aborting...\n");
        return;
    }
    if (!purple_find_chat(gc, atoi(id))) {
        logprintf ("chat now known\n");
        char *subject, *owner, *part;
        do_get_chat_info (conn->tg, MK_CHAT(atoi(id)));
        telegram_flush (conn->tg);
    } else {
        logprintf ("chat already known\n");
        serv_got_joined_chat(conn->gc, atoi(id), groupname);
    }
}

void on_chat_joined (struct telegram *instance, peer_id_t chatid)
{
    logprintf ("on_chat_joined(%d)\n", chatid.id);
    telegram_conn *conn = instance->extra;

    peer_t *peer = user_chat_get (instance->bl, chatid);
    if (!peer) {
        logprintf ("ERROR: chat with given id %d is not existing.\n", chatid.id);
        return;
    }
    if (!peer->id.type == PEER_CHAT) {
        logprintf ("ERROR: peer with given id %d and type %d is not a chat.\n", peer->id.id, peer->id.type);
        return;
    } 
    PurpleConversation *conv = serv_got_joined_chat(conn->gc, chatid.id, peer->chat.title);
    int cnt = peer->chat.user_list_size;
    struct chat_user *curr = peer->chat.user_list;
    int i;
    for (i = 0; i < cnt; i++) {
        peer_id_t part_id = MK_USER((curr + i)->user_id);
        char *name = g_strdup_printf ("%d", part_id.id);
        int flags = PURPLE_CBFLAGS_NONE | peer->chat.admin_id == part_id.id ? PURPLE_CBFLAGS_FOUNDER : 0;
        logprintf ("purple_conv_chat_add_user (..., name=%s, ..., flags=%d)", name, flags);
        purple_conv_chat_add_user(
            purple_conversation_get_chat_data(conv), 
            name, 
            "", 
            flags, 
            0
        );
    }

}

/**
 * Invite a user to join a chat.
 *
 * @param id      The id of the chat to invite the user to.
 * @param message A message displayed to the user when the invitation
 *                is received.
 * @param who     The name of the user to send the invation to.
 */
static void tgprpl_chat_invite(PurpleConnection * gc, int id, const char *message, const char *name)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_chat_invite()\n");
}

/**
 * Returns a chat name based on the information in components. Use
 * #chat_info_defaults if you instead need to generate a hashtable
 * from a chat name.
 *
 * @param components A hashtable containing information about the chat.
 */
static char *tgprpl_get_chat_name(GHashTable * data)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_get_chat_name()\n");
    return g_strdup(g_hash_table_lookup(data, "subject"));
}

/**
 * File transfer callback.
 */
static PurpleXfer *tgprpl_new_xfer(PurpleConnection * gc, const char *who)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_new_xfer()\n");
    return (PurpleXfer *)NULL;
}

/**
 * File transfer callback.
 */
static void tgprpl_send_file(PurpleConnection * gc, const char *who, const char *file)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_send_file()\n");

}

// SEE prpl.h
static PurplePluginProtocolInfo prpl_info = {
    OPT_PROTO_NO_PASSWORD,                    /* options */
    NULL,                                          /* user_splits, initialized in tgprpl_init() */
    NULL,                                          /* protocol_options, initialized in tgprpl_init() */
    {                                         /* icon_spec, a PurpleBuddyIconSpec */
        "png",                                   /* format */
        1,                                          /* min_width */
        1,                                          /* min_height */
        512,                                      /* max_width */
        512,                                      /* max_height */
        64000,                                   /* max_filesize */
        PURPLE_ICON_SCALE_SEND,                   /* scale_rules */
    },
    tgprpl_list_icon,
    NULL,
    NULL,
    tgprpl_tooltip_text,
    tgprpl_status_types,
    NULL,                                     /* blist_node_menu */
    tgprpl_chat_join_info,
    tgprpl_chat_info_defaults,                /* chat_info_defaults */
    tgprpl_login,                             /* login */
    tgprpl_close,                                /* close */
    tgprpl_send_im,                             /* send_im */
    NULL,                                     /* set_info */
    tgprpl_send_typing,                         /* send_typing */
    tgprpl_get_info,                            /* get_info */
    tgprpl_set_status,                         /* set_status */
    NULL,                                     /* set_idle */
    NULL,                                     /* change_passwd */
    tgprpl_add_buddy,                            /* add_buddy */
    tgprpl_add_buddies,                         /* add_buddies */
    tgprpl_remove_buddy,                         /* remove_buddy */
    tgprpl_remove_buddies,                      /* remove_buddies */
    NULL,                                     /* add_permit */
    tgprpl_add_deny,                            /* add_deny */
    NULL,                                     /* rem_permit */
    tgprpl_rem_deny,                            /* rem_deny */
    NULL,                                     /* set_permit_deny */
    tgprpl_chat_join,    /* join_chat */
    NULL,                                     /* reject_chat */
    tgprpl_get_chat_name,                      /* get_chat_name */
    tgprpl_chat_invite,                         /* chat_invite */
    NULL,                                     /* chat_leave */
    NULL,                                     /* chat_whisper */
    tgprpl_send_chat,                            /* chat_send */
    NULL,                                     /* keepalive */
    NULL,                                     /* register_user */
    NULL,                                     /* get_cb_info */
    NULL,                                     /* get_cb_away */
    NULL,                                     /* alias_buddy */
    tgprpl_group_buddy,                         /* group_buddy */
    tgprpl_rename_group,                         /* rename_group */
    NULL,                                     /* buddy_free */
    tgprpl_convo_closed,                         /* convo_closed */
    purple_normalize_nocase,                   /* normalize */
    tgprpl_set_buddy_icon,                      /* set_buddy_icon */
    NULL,                                     /* remove_group */
    NULL,                                     /* get_cb_real_name */
    NULL,                                     /* set_chat_topic */
    NULL,                                     /* find_blist_chat */
    NULL,                                     /* roomlist_get_list */
    NULL,                                     /* roomlist_cancel */
    NULL,                                     /* roomlist_expand_category */
    tgprpl_can_receive_file,                   /* can_receive_file */
    tgprpl_send_file,                            /* send_file */
    tgprpl_new_xfer,                            /* new_xfer */
    tgprpl_offline_message,                      /* offline_message */
    NULL,                                     /* whiteboard_prpl_ops */
    NULL,                                     /* send_raw */
    NULL,                                     /* roomlist_room_serialize */
    NULL,                                     /* unregister_user */
    NULL,                                     /* send_attention */
    NULL,                                     /* get_attention_types */
    sizeof(PurplePluginProtocolInfo),          /* struct_size */
    NULL,                                     /* get_account_text_table */
    NULL,                                     /* initiate_media */
    NULL,                                     /* get_media_caps */
    NULL,                                     /* get_moods */
    NULL,                                     /* set_public_alias */
    NULL,                                     /* get_public_alias */
    NULL,                                     /* add_buddy_with_invite */
    NULL                                          /* add_buddies_with_invite */
};

static void tgprpl_init(PurplePlugin *plugin)
{
    const char *dir = ".telegram";
    if (!tgconf.base_config_path) {
        struct passwd *pw = getpwuid(getuid());
        int len = strlen (dir) + strlen (pw->pw_dir) + 2;    
        tgconf.base_config_path = talloc (len);
        tsnprintf (tgconf.base_config_path, len, "%s/%s", pw->pw_dir, dir);
        logprintf ("base configuration path: %s", tgconf.base_config_path);
    }

    PurpleAccountOption *option;
    PurpleAccountUserSplit *split;
    GList *verification_values = NULL;

    // Redirect the msglog of the telegram-cli application to the libpurple logger
    set_log_cb(&tg_cli_log_cb);

    // Extra Options
 #define ADD_VALUE(list, desc, v) { \
     PurpleKeyValuePair *kvp = g_new0(PurpleKeyValuePair, 1); \
     kvp->key = g_strdup((desc)); \
     kvp->value = g_strdup((v)); \
     list = g_list_prepend(list, kvp); \
 }
    ADD_VALUE(verification_values, "Phone", TELEGRAM_AUTH_MODE_PHONE);
    ADD_VALUE(verification_values, "SMS", TELEGRAM_AUTH_MODE_SMS);

    option = purple_account_option_list_new("Verification type", "verification_type", verification_values);
    prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

    option = purple_account_option_string_new("Verification key", "verification_key", NULL);
    prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

    option = purple_account_option_string_new("Verification hash", "verification_hash", NULL);
    prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

    option = purple_account_option_string_new("First Name", "first_name", NULL);
    prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

    option = purple_account_option_string_new("Last Name", "last_name", NULL);
    prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);


    // TODO: Path to public key (When you can change the server hostname,
    //        you should also be able to change the public key)

    option = purple_account_option_string_new("Server", "server", TELEGRAM_TEST_SERVER);
    prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

    option = purple_account_option_int_new("Port", "port", TELEGRAM_DEFAULT_PORT);
    prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

    _telegram_protocol = plugin;
}

static GList *tgprpl_actions(PurplePlugin * plugin, gpointer context)
{
    // return possible actions (See Libpurple doc)
    return (GList *)NULL;
}

static PurplePluginInfo info = {
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
    "0.1",
    "Telegram protocol",
    "Adds support for the telegram protocol to libpurple.",
    "Christopher Althaus <althaus.christopher@gmail.com>, Markus Endres <endresma45241@th-nuernberg.de>, Matthias Jentsch <mtthsjntsch@gmail.com>",
    "https://bitbucket.org/telegrampurple/telegram-purple",
    NULL,           // on load
    NULL,           // on unload
    NULL,           // on destroy
    NULL,           // ui specific struct
    &prpl_info,     // plugin info struct
    NULL,           // prefs info
    tgprpl_actions, // actions
    NULL,           // reserved
    NULL,           // reserved
    NULL,           // reserved
    NULL            // reserved
};


PURPLE_INIT_PLUGIN(telegram, tgprpl_init, info)

