/**
 * @file gaim-compat.h Gaim Compat macros
 * @ingroup core
 */

/* pidgin
 *
 * Pidgin is the legal property of its developers, whose names are too numerous
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
#ifndef _GAIM_COMPAT_H_
#define _GAIM_COMPAT_H_

#include <glib.h>

/* from account.h */
#define GaimAccountUiOps PurpleAccountUiOps
#define GaimAccount PurpleAccount

#define GaimFilterAccountFunc PurpleFilterAccountFunc
#define GaimAccountRequestAuthorizationCb PurpleAccountRequestAuthorizationCb

#define gaim_account_new           purple_account_new
#define gaim_account_destroy       purple_account_destroy
#define gaim_account_connect       purple_account_connect
#define gaim_account_register      purple_account_register
#define gaim_account_disconnect    purple_account_disconnect
#define gaim_account_notify_added  purple_account_notify_added
#define gaim_account_request_add   purple_account_request_add
#define gaim_account_request_close   purple_account_request_close

#define gaim_account_request_authorization     purple_account_request_authorization
#define gaim_account_request_change_password   purple_account_request_change_password
#define gaim_account_request_change_user_info  purple_account_request_change_user_info

#define gaim_account_set_username            purple_account_set_username
#define gaim_account_set_password            purple_account_set_password
#define gaim_account_set_alias               purple_account_set_alias
#define gaim_account_set_user_info           purple_account_set_user_info
#define gaim_account_set_buddy_icon_path     purple_account_set_buddy_icon_path
#define gaim_account_set_protocol_id         purple_account_set_protocol_id
#define gaim_account_set_connection          purple_account_set_connection
#define gaim_account_set_remember_password   purple_account_set_remember_password
#define gaim_account_set_check_mail          purple_account_set_check_mail
#define gaim_account_set_enabled             purple_account_set_enabled
#define gaim_account_set_proxy_info          purple_account_set_proxy_info
#define gaim_account_set_status_types        purple_account_set_status_types
#define gaim_account_set_status              purple_account_set_status
#define gaim_account_set_status_list         purple_account_set_status_list

#define gaim_account_clear_settings   purple_account_clear_settings

#define gaim_account_set_int    purple_account_set_int
#define gaim_account_set_string purple_account_set_string
#define gaim_account_set_bool   purple_account_set_bool

#define gaim_account_set_ui_int     purple_account_set_ui_int
#define gaim_account_set_ui_string  purple_account_set_ui_string
#define gaim_account_set_ui_bool    purple_account_set_ui_bool

#define gaim_account_is_connected     purple_account_is_connected
#define gaim_account_is_connecting    purple_account_is_connecting
#define gaim_account_is_disconnected  purple_account_is_disconnected

#define gaim_account_get_username           purple_account_get_username
#define gaim_account_get_password           purple_account_get_password
#define gaim_account_get_alias              purple_account_get_alias
#define gaim_account_get_user_info          purple_account_get_user_info
#define gaim_account_get_buddy_icon_path    purple_account_get_buddy_icon_path
#define gaim_account_get_protocol_id        purple_account_get_protocol_id
#define gaim_account_get_protocol_name      purple_account_get_protocol_name
#define gaim_account_get_connection         purple_account_get_connection
#define gaim_account_get_remember_password  purple_account_get_remember_password
#define gaim_account_get_check_mail         purple_account_get_check_mail
#define gaim_account_get_enabled            purple_account_get_enabled
#define gaim_account_get_proxy_info         purple_account_get_proxy_info
#define gaim_account_get_active_status      purple_account_get_active_status
#define gaim_account_get_status             purple_account_get_status
#define gaim_account_get_status_type        purple_account_get_status_type
#define gaim_account_get_status_type_with_primitive \
	purple_account_get_status_type_with_primitive

#define gaim_account_get_presence       purple_account_get_presence
#define gaim_account_is_status_active   purple_account_is_status_active
#define gaim_account_get_status_types   purple_account_get_status_types

#define gaim_account_get_int            purple_account_get_int
#define gaim_account_get_string         purple_account_get_string
#define gaim_account_get_bool           purple_account_get_bool

#define gaim_account_get_ui_int     purple_account_get_ui_int
#define gaim_account_get_ui_string  purple_account_get_ui_string
#define gaim_account_get_ui_bool    purple_account_get_ui_bool


#define gaim_account_get_log      purple_account_get_log
#define gaim_account_destroy_log  purple_account_destroy_log

#define gaim_account_add_buddy       purple_account_add_buddy
#define gaim_account_add_buddies     purple_account_add_buddies
#define gaim_account_remove_buddy    purple_account_remove_buddy
#define gaim_account_remove_buddies  purple_account_remove_buddies

#define gaim_account_remove_group  purple_account_remove_group

#define gaim_account_change_password  purple_account_change_password

#define gaim_account_supports_offline_message  purple_account_supports_offline_message

#define gaim_accounts_add      purple_accounts_add
#define gaim_accounts_remove   purple_accounts_remove
#define gaim_accounts_delete   purple_accounts_delete
#define gaim_accounts_reorder  purple_accounts_reorder

#define gaim_accounts_get_all         purple_accounts_get_all
#define gaim_accounts_get_all_active  purple_accounts_get_all_active

#define gaim_accounts_find   purple_accounts_find

#define gaim_accounts_restore_current_statuses  purple_accounts_restore_current_statuses

#define gaim_accounts_set_ui_ops  purple_accounts_set_ui_ops
#define gaim_accounts_get_ui_ops  purple_accounts_get_ui_ops

#define gaim_accounts_get_handle  purple_accounts_get_handle

#define gaim_accounts_init    purple_accounts_init
#define gaim_accounts_uninit  purple_accounts_uninit

/* from accountopt.h */

#define GaimAccountOption     PurpleAccountOption
#define GaimAccountUserSplit  PurpleAccountUserSplit

#define gaim_account_option_new         purple_account_option_new
#define gaim_account_option_bool_new    purple_account_option_bool_new
#define gaim_account_option_int_new     purple_account_option_int_new
#define gaim_account_option_string_new  purple_account_option_string_new
#define gaim_account_option_list_new    purple_account_option_list_new

#define gaim_account_option_destroy  purple_account_option_destroy

#define gaim_account_option_set_default_bool    purple_account_option_set_default_bool
#define gaim_account_option_set_default_int     purple_account_option_set_default_int
#define gaim_account_option_set_default_string  purple_account_option_set_default_string

#define gaim_account_option_set_masked  purple_account_option_set_masked

#define gaim_account_option_set_list  purple_account_option_set_list

#define gaim_account_option_add_list_item  purple_account_option_add_list_item

#define gaim_account_option_get_type     purple_account_option_get_type
#define gaim_account_option_get_text     purple_account_option_get_text
#define gaim_account_option_get_setting  purple_account_option_get_setting

#define gaim_account_option_get_default_bool        purple_account_option_get_default_bool
#define gaim_account_option_get_default_int         purple_account_option_get_default_int
#define gaim_account_option_get_default_string      purple_account_option_get_default_string
#define gaim_account_option_get_default_list_value  purple_account_option_get_default_list_value

#define gaim_account_option_get_masked  purple_account_option_get_masked
#define gaim_account_option_get_list    purple_account_option_get_list

#define gaim_account_user_split_new      purple_account_user_split_new
#define gaim_account_user_split_destroy  purple_account_user_split_destroy

#define gaim_account_user_split_get_text           purple_account_user_split_get_text
#define gaim_account_user_split_get_default_value  purple_account_user_split_get_default_value
#define gaim_account_user_split_get_separator      purple_account_user_split_get_separator

/* from blist.h */

#define GaimBuddyList    PurpleBuddyList
#define GaimBlistUiOps   PurpleBlistUiOps
#define GaimBlistNode    PurpleBlistNode

#define GaimChat     PurpleChat
#define GaimGroup    PurpleGroup
#define GaimContact  PurpleContact
#define GaimBuddy    PurpleBuddy

#define GAIM_BLIST_GROUP_NODE     PURPLE_BLIST_GROUP_NODE
#define GAIM_BLIST_CONTACT_NODE   PURPLE_BLIST_CONTACT_NODE
#define GAIM_BLIST_BUDDY_NODE     PURPLE_BLIST_BUDDY_NODE
#define GAIM_BLIST_CHAT_NODE      PURPLE_BLIST_CHAT_NODE
#define GAIM_BLIST_OTHER_NODE     PURPLE_BLIST_OTHER_NODE
#define GaimBlistNodeType         PurpleBlistNodeType

#define GAIM_BLIST_NODE_IS_CHAT       PURPLE_BLIST_NODE_IS_CHAT
#define GAIM_BLIST_NODE_IS_BUDDY      PURPLE_BLIST_NODE_IS_BUDDY
#define GAIM_BLIST_NODE_IS_CONTACT    PURPLE_BLIST_NODE_IS_CONTACT
#define GAIM_BLIST_NODE_IS_GROUP      PURPLE_BLIST_NODE_IS_GROUP

#define GAIM_BUDDY_IS_ONLINE PURPLE_BUDDY_IS_ONLINE

#define GAIM_BLIST_NODE_FLAG_NO_SAVE  PURPLE_BLIST_NODE_FLAG_NO_SAVE
#define GaimBlistNodeFlags            PurpleBlistNodeFlags

#define GAIM_BLIST_NODE_HAS_FLAG     PURPLE_BLIST_NODE_HAS_FLAG
#define GAIM_BLIST_NODE_SHOULD_SAVE  PURPLE_BLIST_NODE_SHOULD_SAVE

#define GAIM_BLIST_NODE_NAME   PURPLE_BLIST_NODE_NAME


#define gaim_blist_new  purple_blist_new
#define gaim_set_blist  purple_set_blist
#define gaim_get_blist  purple_get_blist

#define gaim_blist_get_root   purple_blist_get_root
#define gaim_blist_node_next  purple_blist_node_next

#define gaim_blist_show  purple_blist_show

#define gaim_blist_destroy  purple_blist_destroy

#define gaim_blist_set_visible  purple_blist_set_visible

#define gaim_blist_update_buddy_status  purple_blist_update_buddy_status
#define gaim_blist_update_buddy_icon    purple_blist_update_buddy_icon


#define gaim_blist_alias_contact       purple_blist_alias_contact
#define gaim_blist_alias_buddy         purple_blist_alias_buddy
#define gaim_blist_server_alias_buddy  purple_blist_server_alias_buddy
#define gaim_blist_alias_chat          purple_blist_alias_chat

#define gaim_blist_rename_buddy  purple_blist_rename_buddy
#define gaim_blist_rename_group  purple_blist_rename_group

#define gaim_chat_new        purple_chat_new
#define gaim_blist_add_chat  purple_blist_add_chat

#define gaim_buddy_new           purple_buddy_new
#define gaim_buddy_set_icon      purple_buddy_set_icon
#define gaim_buddy_get_account   purple_buddy_get_account
#define gaim_buddy_get_name      purple_buddy_get_name
#define gaim_buddy_get_icon      purple_buddy_get_icon
#define gaim_buddy_get_contact   purple_buddy_get_contact
#define gaim_buddy_get_presence  purple_buddy_get_presence

#define gaim_blist_add_buddy  purple_blist_add_buddy

#define gaim_group_new  purple_group_new

#define gaim_blist_add_group  purple_blist_add_group

#define gaim_contact_new  purple_contact_new

#define gaim_blist_add_contact    purple_blist_add_contact
#define gaim_blist_merge_contact  purple_blist_merge_contact

#define gaim_contact_get_priority_buddy  purple_contact_get_priority_buddy
#define gaim_contact_set_alias           purple_contact_set_alias
#define gaim_contact_get_alias           purple_contact_get_alias
#define gaim_contact_on_account          purple_contact_on_account

#define gaim_contact_invalidate_priority_buddy  purple_contact_invalidate_priority_buddy

#define gaim_blist_remove_buddy    purple_blist_remove_buddy
#define gaim_blist_remove_contact  purple_blist_remove_contact
#define gaim_blist_remove_chat     purple_blist_remove_chat
#define gaim_blist_remove_group    purple_blist_remove_group

#define gaim_buddy_get_alias_only     purple_buddy_get_alias_only
#define gaim_buddy_get_server_alias   purple_buddy_get_server_alias
#define gaim_buddy_get_contact_alias  purple_buddy_get_contact_alias
#define gaim_buddy_get_local_alias    purple_buddy_get_local_alias
#define gaim_buddy_get_alias          purple_buddy_get_alias

#define gaim_chat_get_name  purple_chat_get_name

#define gaim_find_buddy           purple_find_buddy
#define gaim_find_buddy_in_group  purple_find_buddy_in_group
#define gaim_find_buddies         purple_find_buddies

#define gaim_find_group  purple_find_group

#define gaim_blist_find_chat  purple_blist_find_chat

#define gaim_chat_get_group   purple_chat_get_group
#define gaim_buddy_get_group  purple_buddy_get_group

#define gaim_group_get_accounts  purple_group_get_accounts
#define gaim_group_on_account    purple_group_on_account

#define gaim_blist_add_account     purple_blist_add_account
#define gaim_blist_remove_account  purple_blist_remove_account

#define gaim_blist_get_group_size          purple_blist_get_group_size
#define gaim_blist_get_group_online_count  purple_blist_get_group_online_count

#define gaim_blist_load           purple_blist_load
#define gaim_blist_schedule_save  purple_blist_schedule_save

#define gaim_blist_request_add_buddy  purple_blist_request_add_buddy
#define gaim_blist_request_add_chat   purple_blist_request_add_chat
#define gaim_blist_request_add_group  purple_blist_request_add_group

#define gaim_blist_node_set_bool    purple_blist_node_set_bool
#define gaim_blist_node_get_bool    purple_blist_node_get_bool
#define gaim_blist_node_set_int     purple_blist_node_set_int
#define gaim_blist_node_get_int     purple_blist_node_get_int
#define gaim_blist_node_set_string  purple_blist_node_set_string
#define gaim_blist_node_get_string  purple_blist_node_get_string

#define gaim_blist_node_remove_setting  purple_blist_node_remove_setting

#define gaim_blist_node_set_flags  purple_blist_node_set_flags
#define gaim_blist_node_get_flags  purple_blist_node_get_flags

#define gaim_blist_node_get_extended_menu  purple_blist_node_get_extended_menu

#define gaim_blist_set_ui_ops  purple_blist_set_ui_ops
#define gaim_blist_get_ui_ops  purple_blist_get_ui_ops

#define gaim_blist_get_handle  purple_blist_get_handle

#define gaim_blist_init    purple_blist_init
#define gaim_blist_uninit  purple_blist_uninit


#define GaimBuddyIcon  PurpleBuddyIcon

#define gaim_buddy_icon_new(account, username, icon_data, icon_len)\
        purple_buddy_icon_new(account, username, g_memdup(icon_data, icon_len), icon_len)
#define gaim_buddy_icon_ref      purple_buddy_icon_ref
#define gaim_buddy_icon_unref    purple_buddy_icon_unref
#define gaim_buddy_icon_update   purple_buddy_icon_update

#define gaim_buddy_icon_set_data(icon, data, len) \
        purple_buddy_icon_set_data(icon, g_memdup(data, len), len, NULL);

#define gaim_buddy_icon_get_account   purple_buddy_icon_get_account
#define gaim_buddy_icon_get_username  purple_buddy_icon_get_username
#define gaim_buddy_icon_get_data      purple_buddy_icon_get_data
#define gaim_buddy_icon_get_type      purple_buddy_icon_get_extension

#define gaim_buddy_icons_set_for_user(icon, data, len) \
        purple_buddy_icons_set_for_user(icon, g_memdup(data, len), len, NULL)
#define gaim_buddy_icons_set_caching    purple_buddy_icons_set_caching
#define gaim_buddy_icons_is_caching     purple_buddy_icons_is_caching
#define gaim_buddy_icons_set_cache_dir  purple_buddy_icons_set_cache_dir
#define gaim_buddy_icons_get_cache_dir  purple_buddy_icons_get_cache_dir
#define gaim_buddy_icons_get_handle     purple_buddy_icons_get_handle

#define gaim_buddy_icons_init    purple_buddy_icons_init
#define gaim_buddy_icons_uninit  purple_buddy_icons_uninit

#define gaim_buddy_icon_get_scale_size  purple_buddy_icon_get_scale_size

/* from cipher.h */

#define GAIM_CIPHER          PURPLE_CIPHER
#define GAIM_CIPHER_OPS      PURPLE_CIPHER_OPS
#define GAIM_CIPHER_CONTEXT  PURPLE_CIPHER_CONTEXT

#define GaimCipher         PurpleCipher
#define GaimCipherOps      PurpleCipherOps
#define GaimCipherContext  PurpleCipherContext

#define GAIM_CIPHER_CAPS_SET_OPT  PURPLE_CIPHER_CAPS_SET_OPT
#define GAIM_CIPHER_CAPS_GET_OPT  PURPLE_CIPHER_CAPS_GET_OPT
#define GAIM_CIPHER_CAPS_INIT     PURPLE_CIPHER_CAPS_INIT
#define GAIM_CIPHER_CAPS_RESET    PURPLE_CIPHER_CAPS_RESET
#define GAIM_CIPHER_CAPS_UNINIT   PURPLE_CIPHER_CAPS_UNINIT
#define GAIM_CIPHER_CAPS_SET_IV   PURPLE_CIPHER_CAPS_SET_IV
#define GAIM_CIPHER_CAPS_APPEND   PURPLE_CIPHER_CAPS_APPEND
#define GAIM_CIPHER_CAPS_DIGEST   PURPLE_CIPHER_CAPS_DIGEST
#define GAIM_CIPHER_CAPS_ENCRYPT  PURPLE_CIPHER_CAPS_ENCRYPT
#define GAIM_CIPHER_CAPS_DECRYPT  PURPLE_CIPHER_CAPS_DECRYPT
#define GAIM_CIPHER_CAPS_SET_SALT  PURPLE_CIPHER_CAPS_SET_SALT
#define GAIM_CIPHER_CAPS_GET_SALT_SIZE  PURPLE_CIPHER_CAPS_GET_SALT_SIZE
#define GAIM_CIPHER_CAPS_SET_KEY        PURPLE_CIPHER_CAPS_SET_KEY
#define GAIM_CIPHER_CAPS_GET_KEY_SIZE   PURPLE_CIPHER_CAPS_GET_KEY_SIZE
#define GAIM_CIPHER_CAPS_UNKNOWN        PURPLE_CIPHER_CAPS_UNKNOWN

#define gaim_cipher_get_name          purple_cipher_get_name
#define gaim_cipher_get_capabilities  purple_cipher_get_capabilities
#define gaim_cipher_digest_region     purple_cipher_digest_region

#define gaim_ciphers_find_cipher        purple_ciphers_find_cipher
#define gaim_ciphers_register_cipher    purple_ciphers_register_cipher
#define gaim_ciphers_unregister_cipher  purple_ciphers_unregister_cipher
#define gaim_ciphers_get_ciphers        purple_ciphers_get_ciphers

#define gaim_ciphers_get_handle  purple_ciphers_get_handle
#define gaim_ciphers_init        purple_ciphers_init
#define gaim_ciphers_uninit      purple_ciphers_uninit

#define gaim_cipher_context_set_option  purple_cipher_context_set_option
#define gaim_cipher_context_get_option  purple_cipher_context_get_option

#define gaim_cipher_context_new            purple_cipher_context_new
#define gaim_cipher_context_new_by_name    purple_cipher_context_new_by_name
#define gaim_cipher_context_reset          purple_cipher_context_reset
#define gaim_cipher_context_destroy        purple_cipher_context_destroy
#define gaim_cipher_context_set_iv         purple_cipher_context_set_iv
#define gaim_cipher_context_append         purple_cipher_context_append
#define gaim_cipher_context_digest         purple_cipher_context_digest
#define gaim_cipher_context_digest_to_str  purple_cipher_context_digest_to_str
#define gaim_cipher_context_encrypt        purple_cipher_context_encrypt
#define gaim_cipher_context_decrypt        purple_cipher_context_decrypt
#define gaim_cipher_context_set_salt       purple_cipher_context_set_salt
#define gaim_cipher_context_get_salt_size  purple_cipher_context_get_salt_size
#define gaim_cipher_context_set_key        purple_cipher_context_set_key
#define gaim_cipher_context_get_key_size   purple_cipher_context_get_key_size
#define gaim_cipher_context_set_data       purple_cipher_context_set_data
#define gaim_cipher_context_get_data       purple_cipher_context_get_data

#define gaim_cipher_http_digest_calculate_session_key \
	purple_cipher_http_digest_calculate_session_key

#define gaim_cipher_http_digest_calculate_response \
	purple_cipher_http_digest_calculate_response

/* from circbuffer.h */

#define GaimCircBuffer  PurpleCircBuffer

#define gaim_circ_buffer_new           purple_circ_buffer_new
#define gaim_circ_buffer_destroy       purple_circ_buffer_destroy
#define gaim_circ_buffer_append        purple_circ_buffer_append
#define gaim_circ_buffer_get_max_read  purple_circ_buffer_get_max_read
#define gaim_circ_buffer_mark_read     purple_circ_buffer_mark_read

/* from cmds.h */

#define GaimCmdPriority  PurpleCmdPriority
#define GaimCmdFlag      PurpleCmdFlag
#define GaimCmdStatus    PurpleCmdStatus
#define GaimCmdRet       PurpleCmdRet

#define GAIM_CMD_STATUS_OK            PURPLE_CMD_STATUS_OK
#define GAIM_CMD_STATUS_FAILED        PURPLE_CMD_STATUS_FAILED
#define GAIM_CMD_STATUS_NOT_FOUND     PURPLE_CMD_STATUS_NOT_FOUND
#define GAIM_CMD_STATUS_WRONG_ARGS    PURPLE_CMD_STATUS_WRONG_ARGS
#define GAIM_CMD_STATUS_WRONG_PRPL    PURPLE_CMD_STATUS_WRONG_PRPL
#define GAIM_CMD_STATUS_WRONG_TYPE    PURPLE_CMD_STATUS_WRONG_TYPE

#define GAIM_CMD_FUNC  PURPLE_CMD_FUNC

#define GAIM_CMD_RET_OK			PURPLE_CMD_RET_OK
#define GAIM_CMD_RET_FAILED		PURPLE_CMD_RET_FAILED
#define GAIM_CMD_RET_CONTINUE	PURPLE_CMD_RET_CONTINUE

#define GAIM_CMD_P_VERY_LOW		PURPLE_CMD_P_VERY_LOW
#define GAIM_CMD_P_LOW			PURPLE_CMD_P_LOW
#define GAIM_CMD_P_DEFAULT		PURPLE_CMD_P_DEFAULT
#define GAIM_CMD_P_PRPL			PURPLE_CMD_P_PRPL
#define GAIM_CMD_P_PLUGIN		PURPLE_CMD_P_PLUGIN
#define GAIM_CMD_P_ALIAS		PURPLE_CMD_P_ALIAS
#define GAIM_CMD_P_HIGH			PURPLE_CMD_P_HIGH
#define GAIM_CMD_P_VERY_HIGH	PURPLE_CMD_P_VERY_HIGH

#define GAIM_CMD_FLAG_IM		PURPLE_CMD_FLAG_IM
#define GAIM_CMD_FLAG_CHAT		PURPLE_CMD_FLAG_CHAT
#define GAIM_CMD_FLAG_PRPL_ONLY	PURPLE_CMD_FLAG_PRPL_ONLY
#define GAIM_CMD_FLAG_ALLOW_WRONG_ARGS	PURPLE_CMD_FLAG_ALLOW_WRONG_ARGS


#define GaimCmdFunc  PurpleCmdFunc

#define GaimCmdId  PurpleCmdId

#define gaim_cmd_register    purple_cmd_register
#define gaim_cmd_unregister  purple_cmd_unregister
#define gaim_cmd_do_command  purple_cmd_do_command
#define gaim_cmd_list        purple_cmd_list
#define gaim_cmd_help        purple_cmd_help

/* from connection.h */

#define GaimConnection  PurpleConnection

#define GAIM_CONNECTION_HTML              PURPLE_CONNECTION_HTML
#define GAIM_CONNECTION_NO_BGCOLOR        PURPLE_CONNECTION_NO_BGCOLOR
#define GAIM_CONNECTION_AUTO_RESP         PURPLE_CONNECTION_AUTO_RESP
#define GAIM_CONNECTION_FORMATTING_WBFO   PURPLE_CONNECTION_FORMATTING_WBFO
#define GAIM_CONNECTION_NO_NEWLINES       PURPLE_CONNECTION_NO_NEWLINES
#define GAIM_CONNECTION_NO_FONTSIZE       PURPLE_CONNECTION_NO_FONTSIZE
#define GAIM_CONNECTION_NO_URLDESC        PURPLE_CONNECTION_NO_URLDESC
#define GAIM_CONNECTION_NO_IMAGES         PURPLE_CONNECTION_NO_IMAGES

#define GaimConnectionFlags  PurpleConnectionFlags

#define GAIM_DISCONNECTED  PURPLE_DISCONNECTED
#define GAIM_CONNECTED     PURPLE_CONNECTED
#define GAIM_CONNECTING    PURPLE_CONNECTING

#define GaimConnectionState  PurpleConnectionState

#define GaimConnectionUiOps  PurpleConnectionUiOps

#define gaim_connection_new      purple_connection_new
#define gaim_connection_destroy  purple_connection_destroy

#define gaim_connection_set_state         purple_connection_set_state
#define gaim_connection_set_account       purple_connection_set_account
#define gaim_connection_set_display_name  purple_connection_set_display_name
#define gaim_connection_get_state         purple_connection_get_state

#define GAIM_CONNECTION_IS_CONNECTED  PURPLE_CONNECTION_IS_CONNECTED

#define gaim_connection_get_account       purple_connection_get_account
#define gaim_connection_get_password      purple_connection_get_password
#define gaim_connection_get_display_name  purple_connection_get_display_name

#define gaim_connection_update_progress  purple_connection_update_progress

#define gaim_connection_notice  purple_connection_notice
#define gaim_connection_error   purple_connection_error

#define gaim_connections_disconnect_all  purple_connections_disconnect_all

#define gaim_connections_get_all         purple_connections_get_all
#define gaim_connections_get_connecting  purple_connections_get_connecting

#define GAIM_CONNECTION_IS_VALID  PURPLE_CONNECTION_IS_VALID

#define gaim_connections_set_ui_ops  purple_connections_set_ui_ops
#define gaim_connections_get_ui_ops  purple_connections_get_ui_ops

#define gaim_connections_init    purple_connections_init
#define gaim_connections_uninit  purple_connections_uninit
#define gaim_connections_get_handle  purple_connections_get_handle


/* from conversation.h */

#define GaimConversationUiOps  PurpleConversationUiOps
#define GaimConversation       PurpleConversation
#define GaimConvIm             PurpleConvIm
#define GaimConvChat           PurpleConvChat
#define GaimConvChatBuddy      PurpleConvChatBuddy

#define GAIM_CONV_TYPE_UNKNOWN  PURPLE_CONV_TYPE_UNKNOWN
#define GAIM_CONV_TYPE_IM       PURPLE_CONV_TYPE_IM
#define GAIM_CONV_TYPE_CHAT     PURPLE_CONV_TYPE_CHAT
#define GAIM_CONV_TYPE_MISC     PURPLE_CONV_TYPE_MISC
#define GAIM_CONV_TYPE_ANY      PURPLE_CONV_TYPE_ANY

#define GaimConversationType  PurpleConversationType

#define GAIM_CONV_UPDATE_ADD       PURPLE_CONV_UPDATE_ADD
#define GAIM_CONV_UPDATE_REMOVE    PURPLE_CONV_UPDATE_REMOVE
#define GAIM_CONV_UPDATE_ACCOUNT   PURPLE_CONV_UPDATE_ACCOUNT
#define GAIM_CONV_UPDATE_TYPING    PURPLE_CONV_UPDATE_TYPING
#define GAIM_CONV_UPDATE_UNSEEN    PURPLE_CONV_UPDATE_UNSEEN
#define GAIM_CONV_UPDATE_LOGGING   PURPLE_CONV_UPDATE_LOGGING
#define GAIM_CONV_UPDATE_TOPIC     PURPLE_CONV_UPDATE_TOPIC
#define GAIM_CONV_ACCOUNT_ONLINE   PURPLE_CONV_ACCOUNT_ONLINE
#define GAIM_CONV_ACCOUNT_OFFLINE  PURPLE_CONV_ACCOUNT_OFFLINE
#define GAIM_CONV_UPDATE_AWAY      PURPLE_CONV_UPDATE_AWAY
#define GAIM_CONV_UPDATE_ICON      PURPLE_CONV_UPDATE_ICON
#define GAIM_CONV_UPDATE_TITLE     PURPLE_CONV_UPDATE_TITLE
#define GAIM_CONV_UPDATE_CHATLEFT  PURPLE_CONV_UPDATE_CHATLEFT
#define GAIM_CONV_UPDATE_FEATURES  PURPLE_CONV_UPDATE_FEATURES

#define GaimConvUpdateType  PurpleConvUpdateType

#define GAIM_NOT_TYPING  PURPLE_NOT_TYPING
#define GAIM_TYPING      PURPLE_TYPING
#define GAIM_TYPED       PURPLE_TYPED

#define GaimTypingState  PurpleTypingState

#define GAIM_MESSAGE_SEND         PURPLE_MESSAGE_SEND
#define GAIM_MESSAGE_RECV         PURPLE_MESSAGE_RECV
#define GAIM_MESSAGE_SYSTEM       PURPLE_MESSAGE_SYSTEM
#define GAIM_MESSAGE_AUTO_RESP    PURPLE_MESSAGE_AUTO_RESP
#define GAIM_MESSAGE_ACTIVE_ONLY  PURPLE_MESSAGE_ACTIVE_ONLY
#define GAIM_MESSAGE_NICK         PURPLE_MESSAGE_NICK
#define GAIM_MESSAGE_NO_LOG       PURPLE_MESSAGE_NO_LOG
#define GAIM_MESSAGE_WHISPER      PURPLE_MESSAGE_WHISPER
#define GAIM_MESSAGE_ERROR        PURPLE_MESSAGE_ERROR
#define GAIM_MESSAGE_DELAYED      PURPLE_MESSAGE_DELAYED
#define GAIM_MESSAGE_RAW          PURPLE_MESSAGE_RAW
#define GAIM_MESSAGE_IMAGES       PURPLE_MESSAGE_IMAGES

#define GaimMessageFlags  PurpleMessageFlags

#define GAIM_CBFLAGS_NONE     PURPLE_CBFLAGS_NONE
#define GAIM_CBFLAGS_VOICE    PURPLE_CBFLAGS_VOICE
#define GAIM_CBFLAGS_HALFOP   PURPLE_CBFLAGS_HALFOP
#define GAIM_CBFLAGS_OP       PURPLE_CBFLAGS_OP
#define GAIM_CBFLAGS_FOUNDER  PURPLE_CBFLAGS_FOUNDER
#define GAIM_CBFLAGS_TYPING   PURPLE_CBFLAGS_TYPING

#define GaimConvChatBuddyFlags  PurpleConvChatBuddyFlags

#define gaim_conversations_set_ui_ops  purple_conversations_set_ui_ops

#define gaim_conversation_new          purple_conversation_new
#define gaim_conversation_destroy      purple_conversation_destroy
#define gaim_conversation_present      purple_conversation_present
#define gaim_conversation_get_type     purple_conversation_get_type
#define gaim_conversation_set_ui_ops   purple_conversation_set_ui_ops
#define gaim_conversation_get_ui_ops   purple_conversation_get_ui_ops
#define gaim_conversation_set_account  purple_conversation_set_account
#define gaim_conversation_get_account  purple_conversation_get_account
#define gaim_conversation_get_gc       purple_conversation_get_gc
#define gaim_conversation_set_title    purple_conversation_set_title
#define gaim_conversation_get_title    purple_conversation_get_title
#define gaim_conversation_autoset_title  purple_conversation_autoset_title
#define gaim_conversation_set_name       purple_conversation_set_name
#define gaim_conversation_get_name       purple_conversation_get_name
#define gaim_conversation_set_logging    purple_conversation_set_logging
#define gaim_conversation_is_logging     purple_conversation_is_logging
#define gaim_conversation_close_logs     purple_conversation_close_logs
#define gaim_conversation_get_im_data    purple_conversation_get_im_data

#define GAIM_CONV_IM    PURPLE_CONV_IM

#define gaim_conversation_get_chat_data  purple_conversation_get_chat_data

#define GAIM_CONV_CHAT  PURPLE_CONV_CHAT

#define gaim_conversation_set_data       purple_conversation_set_data
#define gaim_conversation_get_data       purple_conversation_get_data

#define gaim_get_conversations  purple_get_conversations
#define gaim_get_ims            purple_get_ims
#define gaim_get_chats          purple_get_chats

#define gaim_find_conversation_with_account \
	purple_find_conversation_with_account

#define gaim_conversation_write         purple_conversation_write
#define gaim_conversation_set_features  purple_conversation_set_features
#define gaim_conversation_get_features  purple_conversation_get_features
#define gaim_conversation_has_focus     purple_conversation_has_focus
#define gaim_conversation_update        purple_conversation_update
#define gaim_conversation_foreach       purple_conversation_foreach

#define gaim_conv_im_get_conversation  purple_conv_im_get_conversation
#define gaim_conv_im_set_icon          purple_conv_im_set_icon
#define gaim_conv_im_get_icon          purple_conv_im_get_icon
#define gaim_conv_im_set_typing_state  purple_conv_im_set_typing_state
#define gaim_conv_im_get_typing_state  purple_conv_im_get_typing_state

#define gaim_conv_im_start_typing_timeout  purple_conv_im_start_typing_timeout
#define gaim_conv_im_stop_typing_timeout   purple_conv_im_stop_typing_timeout
#define gaim_conv_im_get_typing_timeout    purple_conv_im_get_typing_timeout
#define gaim_conv_im_set_type_again        purple_conv_im_set_type_again
#define gaim_conv_im_get_type_again        purple_conv_im_get_type_again

#define gaim_conv_im_start_send_typed_timeout \
	purple_conv_im_start_send_typed_timeout

#define gaim_conv_im_stop_send_typed_timeout \
	purple_conv_im_stop_send_typed_timeout

#define gaim_conv_im_get_send_typed_timeout \
	purple_conv_im_get_send_typed_timeout

#define gaim_conv_present_error     purple_conv_present_error
#define gaim_conv_send_confirm      purple_conv_send_confirm

#define gaim_conv_im_update_typing    purple_conv_im_update_typing
#define gaim_conv_im_write            purple_conv_im_write
#define gaim_conv_im_send             purple_conv_im_send
#define gaim_conv_im_send_with_flags  purple_conv_im_send_with_flags

#define gaim_conv_custom_smiley_add    purple_conv_custom_smiley_add
#define gaim_conv_custom_smiley_write  purple_conv_custom_smiley_write
#define gaim_conv_custom_smiley_close  purple_conv_custom_smiley_close

#define gaim_conv_chat_get_conversation  purple_conv_chat_get_conversation
#define gaim_conv_chat_set_users         purple_conv_chat_set_users
#define gaim_conv_chat_get_users         purple_conv_chat_get_users
#define gaim_conv_chat_ignore            purple_conv_chat_ignore
#define gaim_conv_chat_unignore          purple_conv_chat_unignore
#define gaim_conv_chat_set_ignored       purple_conv_chat_set_ignored
#define gaim_conv_chat_get_ignored       purple_conv_chat_get_ignored
#define gaim_conv_chat_get_ignored_user  purple_conv_chat_get_ignored_user
#define gaim_conv_chat_is_user_ignored   purple_conv_chat_is_user_ignored
#define gaim_conv_chat_set_topic         purple_conv_chat_set_topic
#define gaim_conv_chat_get_topic         purple_conv_chat_get_topic
#define gaim_conv_chat_set_id            purple_conv_chat_set_id
#define gaim_conv_chat_get_id            purple_conv_chat_get_id
#define gaim_conv_chat_write             purple_conv_chat_write
#define gaim_conv_chat_send              purple_conv_chat_send
#define gaim_conv_chat_send_with_flags   purple_conv_chat_send_with_flags
#define gaim_conv_chat_add_user          purple_conv_chat_add_user
#define gaim_conv_chat_add_users         purple_conv_chat_add_users
#define gaim_conv_chat_rename_user       purple_conv_chat_rename_user
#define gaim_conv_chat_remove_user       purple_conv_chat_remove_user
#define gaim_conv_chat_remove_users      purple_conv_chat_remove_users
#define gaim_conv_chat_find_user         purple_conv_chat_find_user
#define gaim_conv_chat_user_set_flags    purple_conv_chat_user_set_flags
#define gaim_conv_chat_user_get_flags    purple_conv_chat_user_get_flags
#define gaim_conv_chat_clear_users       purple_conv_chat_clear_users
#define gaim_conv_chat_set_nick          purple_conv_chat_set_nick
#define gaim_conv_chat_get_nick          purple_conv_chat_get_nick
#define gaim_conv_chat_left              purple_conv_chat_left
#define gaim_conv_chat_has_left          purple_conv_chat_has_left

#define gaim_find_chat                   purple_find_chat

#define gaim_conv_chat_cb_new            purple_conv_chat_cb_new
#define gaim_conv_chat_cb_find           purple_conv_chat_cb_find
#define gaim_conv_chat_cb_get_name       purple_conv_chat_cb_get_name
#define gaim_conv_chat_cb_destroy        purple_conv_chat_cb_destroy

#define gaim_conversations_get_handle    purple_conversations_get_handle
#define gaim_conversations_init          purple_conversations_init
#define gaim_conversations_uninit        purple_conversations_uninit

/* from core.h */

#define GaimCore  PurpleCore

#define GaimCoreUiOps  PurpleCoreUiOps

#define gaim_core_init  purple_core_init
#define gaim_core_quit  purple_core_quit

#define gaim_core_quit_cb      purple_core_quit_cb
#define gaim_core_get_version  purple_core_get_version
#define gaim_core_get_ui       purple_core_get_ui
#define gaim_get_core          purple_get_core
#define gaim_core_set_ui_ops   purple_core_set_ui_ops
#define gaim_core_get_ui_ops   purple_core_get_ui_ops

/* from debug.h */

#define GAIM_DEBUG_ALL      PURPLE_DEBUG_ALL
#define GAIM_DEBUG_MISC     PURPLE_DEBUG_MISC
#define GAIM_DEBUG_INFO     PURPLE_DEBUG_INFO
#define GAIM_DEBUG_WARNING  PURPLE_DEBUG_WARNING
#define GAIM_DEBUG_ERROR    PURPLE_DEBUG_ERROR
#define GAIM_DEBUG_FATAL    PURPLE_DEBUG_FATAL

#define GaimDebugLevel  PurpleDebugLevel

#define GaimDebugUiOps  PurpleDebugUiOps


#define gaim_debug          purple_debug
#define gaim_debug_misc     purple_debug_misc
#define gaim_debug_info     purple_debug_info
#define gaim_debug_warning  purple_debug_warning
#define gaim_debug_error    purple_debug_error
#define gaim_debug_fatal    purple_debug_fatal

#define gaim_debug_set_enabled  purple_debug_set_enabled
#define gaim_debug_is_enabled   purple_debug_is_enabled

#define gaim_debug_set_ui_ops  purple_debug_set_ui_ops
#define gaim_debug_get_ui_ops  purple_debug_get_ui_ops

#define gaim_debug_init  purple_debug_init

/* from desktopitem.h */

#define GAIM_DESKTOP_ITEM_TYPE_NULL          PURPLE_DESKTOP_ITEM_TYPE_NULL
#define GAIM_DESKTOP_ITEM_TYPE_OTHER         PURPLE_DESKTOP_ITEM_TYPE_OTHER
#define GAIM_DESKTOP_ITEM_TYPE_APPLICATION   PURPLE_DESKTOP_ITEM_TYPE_APPLICATION
#define GAIM_DESKTOP_ITEM_TYPE_LINK          PURPLE_DESKTOP_ITEM_TYPE_LINK
#define GAIM_DESKTOP_ITEM_TYPE_FSDEVICE      PURPLE_DESKTOP_ITEM_TYPE_FSDEVICE
#define GAIM_DESKTOP_ITEM_TYPE_MIME_TYPE     PURPLE_DESKTOP_ITEM_TYPE_MIME_TYPE
#define GAIM_DESKTOP_ITEM_TYPE_DIRECTORY     PURPLE_DESKTOP_ITEM_TYPE_DIRECTORY
#define GAIM_DESKTOP_ITEM_TYPE_SERVICE       PURPLE_DESKTOP_ITEM_TYPE_SERVICE
#define GAIM_DESKTOP_ITEM_TYPE_SERVICE_TYPE  PURPLE_DESKTOP_ITEM_TYPE_SERVICE_TYPE

#define GaimDesktopItemType  PurpleDesktopItemType

#define GaimDesktopItem  PurpleDesktopItem

#define GAIM_TYPE_DESKTOP_ITEM         PURPLE_TYPE_DESKTOP_ITEM
#define gaim_desktop_item_get_type     purple_desktop_item_get_type

/* standard */
/* ugh, i'm just copying these as strings, rather than pidginifying them */
#define GAIM_DESKTOP_ITEM_ENCODING	"Encoding" /* string */
#define GAIM_DESKTOP_ITEM_VERSION	"Version"  /* numeric */
#define GAIM_DESKTOP_ITEM_NAME		"Name" /* localestring */
#define GAIM_DESKTOP_ITEM_GENERIC_NAME	"GenericName" /* localestring */
#define GAIM_DESKTOP_ITEM_TYPE		"Type" /* string */
#define GAIM_DESKTOP_ITEM_FILE_PATTERN "FilePattern" /* regexp(s) */
#define GAIM_DESKTOP_ITEM_TRY_EXEC	"TryExec" /* string */
#define GAIM_DESKTOP_ITEM_NO_DISPLAY	"NoDisplay" /* boolean */
#define GAIM_DESKTOP_ITEM_COMMENT	"Comment" /* localestring */
#define GAIM_DESKTOP_ITEM_EXEC		"Exec" /* string */
#define GAIM_DESKTOP_ITEM_ACTIONS	"Actions" /* strings */
#define GAIM_DESKTOP_ITEM_ICON		"Icon" /* string */
#define GAIM_DESKTOP_ITEM_MINI_ICON	"MiniIcon" /* string */
#define GAIM_DESKTOP_ITEM_HIDDEN	"Hidden" /* boolean */
#define GAIM_DESKTOP_ITEM_PATH		"Path" /* string */
#define GAIM_DESKTOP_ITEM_TERMINAL	"Terminal" /* boolean */
#define GAIM_DESKTOP_ITEM_TERMINAL_OPTIONS "TerminalOptions" /* string */
#define GAIM_DESKTOP_ITEM_SWALLOW_TITLE "SwallowTitle" /* string */
#define GAIM_DESKTOP_ITEM_SWALLOW_EXEC	"SwallowExec" /* string */
#define GAIM_DESKTOP_ITEM_MIME_TYPE	"MimeType" /* regexp(s) */
#define GAIM_DESKTOP_ITEM_PATTERNS	"Patterns" /* regexp(s) */
#define GAIM_DESKTOP_ITEM_DEFAULT_APP	"DefaultApp" /* string */
#define GAIM_DESKTOP_ITEM_DEV		"Dev" /* string */
#define GAIM_DESKTOP_ITEM_FS_TYPE	"FSType" /* string */
#define GAIM_DESKTOP_ITEM_MOUNT_POINT	"MountPoint" /* string */
#define GAIM_DESKTOP_ITEM_READ_ONLY	"ReadOnly" /* boolean */
#define GAIM_DESKTOP_ITEM_UNMOUNT_ICON "UnmountIcon" /* string */
#define GAIM_DESKTOP_ITEM_SORT_ORDER	"SortOrder" /* strings */
#define GAIM_DESKTOP_ITEM_URL		"URL" /* string */
#define GAIM_DESKTOP_ITEM_DOC_PATH	"X-GNOME-DocPath" /* string */

#define gaim_desktop_item_new_from_file   purple_desktop_item_new_from_file
#define gaim_desktop_item_get_entry_type  purple_desktop_item_get_entry_type
#define gaim_desktop_item_get_string      purple_desktop_item_get_string
#define gaim_desktop_item_copy            purple_desktop_item_copy
#define gaim_desktop_item_unref           purple_desktop_item_unref

/* from dnsquery.h */

#define GaimDnsQueryData  PurpleDnsQueryData
#define GaimDnsQueryConnectFunction  PurpleDnsQueryConnectFunction

#define gaim_dnsquery_a        		purple_dnsquery_a
#define gaim_dnsquery_destroy  		purple_dnsquery_destroy
#define gaim_dnsquery_init     		purple_dnsquery_init
#define gaim_dnsquery_uninit   		purple_dnsquery_uninit
#define gaim_dnsquery_set_ui_ops	purple_dnsquery_set_ui_ops
#define gaim_dnsquery_get_host 		purple_dnsquery_get_host
#define gaim_dnsquery_get_port 		purple_dnsquery_get_port

/* from dnssrv.h */

#define GaimSrvResponse   PurpleSrvResponse
#define GaimSrvQueryData  PurpleSrvTxtQueryData
#define GaimSrvCallback   PurpleSrvCallback

#define gaim_srv_resolve  purple_srv_resolve
#define gaim_srv_cancel   purple_srv_cancel

/* from eventloop.h */

#define GAIM_INPUT_READ   PURPLE_INPUT_READ
#define GAIM_INPUT_WRITE  PURPLE_INPUT_WRITE

#define GaimInputCondition  PurpleInputCondition
#define GaimInputFunction   PurpleInputFunction
#define GaimEventLoopUiOps  PurpleEventLoopUiOps

#define gaim_timeout_add     purple_timeout_add
#define gaim_timeout_remove  purple_timeout_remove
#define gaim_input_add       purple_input_add
#define gaim_input_remove    purple_input_remove

#define gaim_eventloop_set_ui_ops  purple_eventloop_set_ui_ops
#define gaim_eventloop_get_ui_ops  purple_eventloop_get_ui_ops

/* from ft.h */

#define GaimXfer  PurpleXfer

#define GAIM_XFER_UNKNOWN  PURPLE_XFER_UNKNOWN
#define GAIM_XFER_SEND     PURPLE_XFER_SEND
#define GAIM_XFER_RECEIVE  PURPLE_XFER_RECEIVE

#define GaimXferType  PurpleXferType

#define GAIM_XFER_STATUS_UNKNOWN        PURPLE_XFER_STATUS_UNKNOWN
#define GAIM_XFER_STATUS_NOT_STARTED    PURPLE_XFER_STATUS_NOT_STARTED
#define GAIM_XFER_STATUS_ACCEPTED       PURPLE_XFER_STATUS_ACCEPTED
#define GAIM_XFER_STATUS_STARTED        PURPLE_XFER_STATUS_STARTED
#define GAIM_XFER_STATUS_DONE           PURPLE_XFER_STATUS_DONE
#define GAIM_XFER_STATUS_CANCEL_LOCAL   PURPLE_XFER_STATUS_CANCEL_LOCAL
#define GAIM_XFER_STATUS_CANCEL_REMOTE  PURPLE_XFER_STATUS_CANCEL_REMOTE

#define GaimXferStatusType  PurpleXferStatusType

#define GaimXferUiOps  PurpleXferUiOps

#define gaim_xfer_new                  purple_xfer_new
#define gaim_xfer_ref                  purple_xfer_ref
#define gaim_xfer_unref                purple_xfer_unref
#define gaim_xfer_request              purple_xfer_request
#define gaim_xfer_request_accepted     purple_xfer_request_accepted
#define gaim_xfer_request_denied       purple_xfer_request_denied
#define gaim_xfer_get_type             purple_xfer_get_type
#define gaim_xfer_get_account          purple_xfer_get_account
#define gaim_xfer_get_status           purple_xfer_get_status
#define gaim_xfer_is_canceled          purple_xfer_is_canceled
#define gaim_xfer_is_completed         purple_xfer_is_completed
#define gaim_xfer_get_filename         purple_xfer_get_filename
#define gaim_xfer_get_local_filename   purple_xfer_get_local_filename
#define gaim_xfer_get_bytes_sent       purple_xfer_get_bytes_sent
#define gaim_xfer_get_bytes_remaining  purple_xfer_get_bytes_remaining
#define gaim_xfer_get_size             purple_xfer_get_size
#define gaim_xfer_get_progress         purple_xfer_get_progress
#define gaim_xfer_get_local_port       purple_xfer_get_local_port
#define gaim_xfer_get_remote_ip        purple_xfer_get_remote_ip
#define gaim_xfer_get_remote_port      purple_xfer_get_remote_port
#define gaim_xfer_set_completed        purple_xfer_set_completed
#define gaim_xfer_set_message          purple_xfer_set_message
#define gaim_xfer_set_filename         purple_xfer_set_filename
#define gaim_xfer_set_local_filename   purple_xfer_set_local_filename
#define gaim_xfer_set_size             purple_xfer_set_size
#define gaim_xfer_set_bytes_sent       purple_xfer_set_bytes_sent
#define gaim_xfer_get_ui_ops           purple_xfer_get_ui_ops
#define gaim_xfer_set_read_fnc         purple_xfer_set_read_fnc
#define gaim_xfer_set_write_fnc        purple_xfer_set_write_fnc
#define gaim_xfer_set_ack_fnc          purple_xfer_set_ack_fnc
#define gaim_xfer_set_request_denied_fnc  purple_xfer_set_request_denied_fnc
#define gaim_xfer_set_init_fnc         purple_xfer_set_init_fnc
#define gaim_xfer_set_start_fnc        purple_xfer_set_start_fnc
#define gaim_xfer_set_end_fnc          purple_xfer_set_end_fnc
#define gaim_xfer_set_cancel_send_fnc  purple_xfer_set_cancel_send_fnc
#define gaim_xfer_set_cancel_recv_fnc  purple_xfer_set_cancel_recv_fnc

#define gaim_xfer_read                purple_xfer_read
#define gaim_xfer_write               purple_xfer_write
#define gaim_xfer_start               purple_xfer_start
#define gaim_xfer_end                 purple_xfer_end
#define gaim_xfer_add                 purple_xfer_add
#define gaim_xfer_cancel_local        purple_xfer_cancel_local
#define gaim_xfer_cancel_remote       purple_xfer_cancel_remote
#define gaim_xfer_error               purple_xfer_error
#define gaim_xfer_update_progress     purple_xfer_update_progress
#define gaim_xfer_conversation_write  purple_xfer_conversation_write

#define gaim_xfers_get_handle  purple_xfers_get_handle
#define gaim_xfers_init        purple_xfers_init
#define gaim_xfers_uninit      purple_xfers_uninit
#define gaim_xfers_set_ui_ops  purple_xfers_set_ui_ops
#define gaim_xfers_get_ui_ops  purple_xfers_get_ui_ops

/* from gaim-client.h */

#define gaim_init  purple_init

/* from idle.h */

#define GaimIdleUiOps  PurpleIdleUiOps

#define gaim_idle_touch       purple_idle_touch
#define gaim_idle_set         purple_idle_set
#define gaim_idle_set_ui_ops  purple_idle_set_ui_ops
#define gaim_idle_get_ui_ops  purple_idle_get_ui_ops
#define gaim_idle_init        purple_idle_init
#define gaim_idle_uninit      purple_idle_uninit

/* from imgstore.h */

#define GaimStoredImage  PurpleStoredImage

#define gaim_imgstore_add(data, size, filename) \
        purple_imgstore_add_with_id(g_memdup(data, size), size, filename)
#define gaim_imgstore_get           purple_imgstore_find_by_id
#define gaim_imgstore_get_data      purple_imgstore_get_data
#define gaim_imgstore_get_size      purple_imgstore_get_size
#define gaim_imgstore_get_filename  purple_imgstore_get_filename
#define gaim_imgstore_ref           purple_imgstore_ref_by_id
#define gaim_imgstore_unref         purple_imgstore_unref_by_id


/* from log.h */

#define GaimLog                  PurpleLog
#define GaimLogLogger            PurpleLogLogger
#define GaimLogCommonLoggerData  PurpleLogCommonLoggerData
#define GaimLogSet               PurpleLogSet

#define GAIM_LOG_IM      PURPLE_LOG_IM
#define GAIM_LOG_CHAT    PURPLE_LOG_CHAT
#define GAIM_LOG_SYSTEM  PURPLE_LOG_SYSTEM

#define GaimLogType  PurpleLogType

#define GAIM_LOG_READ_NO_NEWLINE  PURPLE_LOG_READ_NO_NEWLINE

#define GaimLogReadFlags  PurpleLogReadFlags

#define GaimLogSetCallback  PurpleLogSetCallback

#define gaim_log_new    purple_log_new
#define gaim_log_free   purple_log_free
#define gaim_log_write  purple_log_write
#define gaim_log_read   purple_log_read

#define gaim_log_get_logs         purple_log_get_logs
#define gaim_log_get_log_sets     purple_log_get_log_sets
#define gaim_log_get_system_logs  purple_log_get_system_logs
#define gaim_log_get_size         purple_log_get_size
#define gaim_log_get_total_size   purple_log_get_total_size
#define gaim_log_get_log_dir      purple_log_get_log_dir
#define gaim_log_compare          purple_log_compare
#define gaim_log_set_compare      purple_log_set_compare
#define gaim_log_set_free         purple_log_set_free

#define gaim_log_common_writer       purple_log_common_writer
#define gaim_log_common_lister       purple_log_common_lister
#define gaim_log_common_total_sizer  purple_log_common_total_sizer
#define gaim_log_common_sizer        purple_log_common_sizer

#define gaim_log_logger_new     purple_log_logger_new
#define gaim_log_logger_free    purple_log_logger_free
#define gaim_log_logger_add     purple_log_logger_add
#define gaim_log_logger_remove  purple_log_logger_remove
#define gaim_log_logger_set     purple_log_logger_set
#define gaim_log_logger_get     purple_log_logger_get

#define gaim_log_logger_get_options  purple_log_logger_get_options

#define gaim_log_init        purple_log_init
#define gaim_log_get_handle  purple_log_get_handle
#define gaim_log_uninit      purple_log_uninit

/* from mime.h */

#define GaimMimeDocument  PurpleMimeDocument
#define GaimMimePart      PurpleMimePart

#define gaim_mime_document_new         purple_mime_document_new
#define gaim_mime_document_free        purple_mime_document_free
#define gaim_mime_document_parse       purple_mime_document_parse
#define gaim_mime_document_parsen      purple_mime_document_parsen
#define gaim_mime_document_write       purple_mime_document_write
#define gaim_mime_document_get_fields  purple_mime_document_get_fields
#define gaim_mime_document_get_field   purple_mime_document_get_field
#define gaim_mime_document_set_field   purple_mime_document_set_field
#define gaim_mime_document_get_parts   purple_mime_document_get_parts

#define gaim_mime_part_new                purple_mime_part_new
#define gaim_mime_part_get_fields         purple_mime_part_get_fields
#define gaim_mime_part_get_field          purple_mime_part_get_field
#define gaim_mime_part_get_field_decoded  purple_mime_part_get_field_decoded
#define gaim_mime_part_set_field          purple_mime_part_set_field
#define gaim_mime_part_get_data           purple_mime_part_get_data
#define gaim_mime_part_get_data_decoded   purple_mime_part_get_data_decoded
#define gaim_mime_part_get_length         purple_mime_part_get_length
#define gaim_mime_part_set_data           purple_mime_part_set_data


/* from network.h */

#define GaimNetworkListenData  PurpleNetworkListenData

#define GaimNetworkListenCallback  PurpleNetworkListenCallback

#define gaim_network_ip_atoi              purple_network_ip_atoi
#define gaim_network_set_public_ip        purple_network_set_public_ip
#define gaim_network_get_public_ip        purple_network_get_public_ip
#define gaim_network_get_local_system_ip  purple_network_get_local_system_ip
#define gaim_network_get_my_ip            purple_network_get_my_ip

#define gaim_network_listen            purple_network_listen
#define gaim_network_listen_range      purple_network_listen_range
#define gaim_network_listen_cancel     purple_network_listen_cancel
#define gaim_network_get_port_from_fd  purple_network_get_port_from_fd

#define gaim_network_is_available  purple_network_is_available

#define gaim_network_init    purple_network_init
#define gaim_network_uninit  purple_network_uninit

/* from notify.h */


#define GaimNotifyUserInfoEntry  PurpleNotifyUserInfoEntry
#define GaimNotifyUserInfo       PurpleNotifyUserInfo

#define GaimNotifyCloseCallback  PurpleNotifyCloseCallback

#define GAIM_NOTIFY_MESSAGE        PURPLE_NOTIFY_MESSAGE
#define GAIM_NOTIFY_EMAIL          PURPLE_NOTIFY_EMAIL
#define GAIM_NOTIFY_EMAILS         PURPLE_NOTIFY_EMAILS
#define GAIM_NOTIFY_FORMATTED      PURPLE_NOTIFY_FORMATTED
#define GAIM_NOTIFY_SEARCHRESULTS  PURPLE_NOTIFY_SEARCHRESULTS
#define GAIM_NOTIFY_USERINFO       PURPLE_NOTIFY_USERINFO
#define GAIM_NOTIFY_URI            PURPLE_NOTIFY_URI

#define GaimNotifyType  PurpleNotifyType

#define GAIM_NOTIFY_MSG_ERROR    PURPLE_NOTIFY_MSG_ERROR
#define GAIM_NOTIFY_MSG_WARNING  PURPLE_NOTIFY_MSG_WARNING
#define GAIM_NOTIFY_MSG_INFO     PURPLE_NOTIFY_MSG_INFO

#define GaimNotifyMsgType  PurpleNotifyMsgType

#define GAIM_NOTIFY_BUTTON_LABELED   PURPLE_NOTIFY_BUTTON_LABELED
#define GAIM_NOTIFY_BUTTON_CONTINUE  PURPLE_NOTIFY_BUTTON_CONTINUE
#define GAIM_NOTIFY_BUTTON_ADD       PURPLE_NOTIFY_BUTTON_ADD
#define GAIM_NOTIFY_BUTTON_INFO      PURPLE_NOTIFY_BUTTON_INFO
#define GAIM_NOTIFY_BUTTON_IM        PURPLE_NOTIFY_BUTTON_IM
#define GAIM_NOTIFY_BUTTON_JOIN      PURPLE_NOTIFY_BUTTON_JOIN
#define GAIM_NOTIFY_BUTTON_INVITE    PURPLE_NOTIFY_BUTTON_INVITE

#define GaimNotifySearchButtonType  PurpleNotifySearchButtonType

#define GaimNotifySearchResults  PurpleNotifySearchResults

#define GAIM_NOTIFY_USER_INFO_ENTRY_PAIR            PURPLE_NOTIFY_USER_INFO_ENTRY_PAIR
#define GAIM_NOTIFY_USER_INFO_ENTRY_SECTION_BREAK   PURPLE_NOTIFY_USER_INFO_ENTRY_SECTION_BREAK
#define GAIM_NOTIFY_USER_INFO_ENTRY_SECTION_HEADER  PURPLE_NOTIFY_USER_INFO_ENTRY_SECTION_HEADER

#define GaimNotifyUserInfoEntryType  PurpleNotifyUserInfoEntryType

#define GaimNotifySearchColumn           PurpleNotifySearchColumn
#define GaimNotifySearchResultsCallback  PurpleNotifySearchResultsCallback
#define GaimNotifySearchButton           PurpleNotifySearchButton

#define GaimNotifyUiOps  PurpleNotifyUiOps

#define gaim_notify_searchresults                     purple_notify_searchresults
#define gaim_notify_searchresults_free                purple_notify_searchresults_free
#define gaim_notify_searchresults_new_rows            purple_notify_searchresults_new_rows
#define gaim_notify_searchresults_button_add          purple_notify_searchresults_button_add
#define gaim_notify_searchresults_button_add_labeled  purple_notify_searchresults_button_add_labeled
#define gaim_notify_searchresults_new                 purple_notify_searchresults_new
#define gaim_notify_searchresults_column_new          purple_notify_searchresults_column_new
#define gaim_notify_searchresults_column_add          purple_notify_searchresults_column_add
#define gaim_notify_searchresults_row_add             purple_notify_searchresults_row_add
#define gaim_notify_searchresults_get_rows_count      purple_notify_searchresults_get_rows_count
#define gaim_notify_searchresults_get_columns_count   purple_notify_searchresults_get_columns_count
#define gaim_notify_searchresults_row_get             purple_notify_searchresults_row_get
#define gaim_notify_searchresults_column_get_title    purple_notify_searchresults_column_get_title

#define gaim_notify_message    purple_notify_message
#define gaim_notify_email      purple_notify_email
#define gaim_notify_emails     purple_notify_emails
#define gaim_notify_formatted  purple_notify_formatted
#define gaim_notify_userinfo   purple_notify_userinfo

#define gaim_notify_user_info_new                    purple_notify_user_info_new
#define gaim_notify_user_info_destroy                purple_notify_user_info_destroy
#define gaim_notify_user_info_get_entries            purple_notify_user_info_get_entries
#define gaim_notify_user_info_get_text_with_newline  purple_notify_user_info_get_text_with_newline
#define gaim_notify_user_info_add_pair               purple_notify_user_info_add_pair
#define gaim_notify_user_info_prepend_pair           purple_notify_user_info_prepend_pair
#define gaim_notify_user_info_remove_entry           purple_notify_user_info_remove_entry
#define gaim_notify_user_info_entry_new              purple_notify_user_info_entry_new
#define gaim_notify_user_info_add_section_break      purple_notify_user_info_add_section_break
#define gaim_notify_user_info_add_section_header     purple_notify_user_info_add_section_header
#define gaim_notify_user_info_remove_last_item       purple_notify_user_info_remove_last_item
#define gaim_notify_user_info_entry_get_label        purple_notify_user_info_entry_get_label
#define gaim_notify_user_info_entry_set_label        purple_notify_user_info_entry_set_label
#define gaim_notify_user_info_entry_get_value        purple_notify_user_info_entry_get_value
#define gaim_notify_user_info_entry_set_value        purple_notify_user_info_entry_set_value
#define gaim_notify_user_info_entry_get_type         purple_notify_user_info_entry_get_type
#define gaim_notify_user_info_entry_set_type         purple_notify_user_info_entry_set_type

#define gaim_notify_uri                purple_notify_uri
#define gaim_notify_close              purple_notify_close
#define gaim_notify_close_with_handle  purple_notify_close_with_handle

#define gaim_notify_info     purple_notify_info
#define gaim_notify_warning  purple_notify_warning
#define gaim_notify_error    purple_notify_error

#define gaim_notify_set_ui_ops  purple_notify_set_ui_ops
#define gaim_notify_get_ui_ops  purple_notify_get_ui_ops

#define gaim_notify_get_handle  purple_notify_get_handle

#define gaim_notify_init    purple_notify_init
#define gaim_notify_uninit  purple_notify_uninit

/* from ntlm.h */

#define gaim_ntlm_gen_type1    purple_ntlm_gen_type1
#define gaim_ntlm_parse_type2  purple_ntlm_parse_type2
#define gaim_ntlm_gen_type3    purple_ntlm_gen_type3

/* from plugin.h */

#ifdef GAIM_PLUGINS
#ifndef PURPLE_PLUGINS
#define PURPLE_PLUGINS
#endif
#endif

#define GaimPlugin            PurplePlugin
#define GaimPluginInfo        PurplePluginInfo
#define GaimPluginUiInfo      PurplePluginUiInfo
#define GaimPluginLoaderInfo  PurplePluginLoaderInfo
#define GaimPluginAction      PurplePluginAction
#define GaimPluginPriority    PurplePluginPriority

#define GAIM_PLUGIN_UNKNOWN   PURPLE_PLUGIN_UNKNOWN
#define GAIM_PLUGIN_STANDARD  PURPLE_PLUGIN_STANDARD
#define GAIM_PLUGIN_LOADER    PURPLE_PLUGIN_LOADER
#define GAIM_PLUGIN_PROTOCOL  PURPLE_PLUGIN_PROTOCOL

#define GaimPluginType        PurplePluginType

#define GAIM_PRIORITY_DEFAULT  PURPLE_PRIORITY_DEFAULT
#define GAIM_PRIORITY_HIGHEST  PURPLE_PRIORITY_HIGHEST
#define GAIM_PRIORITY_LOWEST   PURPLE_PRIORITY_LOWEST

#define GAIM_PLUGIN_FLAG_INVISIBLE  PURPLE_PLUGIN_FLAG_INVISIBLE

#define GAIM_PLUGIN_MAGIC  PURPLE_PLUGIN_MAGIC

#define GAIM_PLUGIN_LOADER_INFO     PURPLE_PLUGIN_LOADER_INFO
#define GAIM_PLUGIN_HAS_PREF_FRAME  PURPLE_PLUGIN_HAS_PREF_FRAME
#define GAIM_PLUGIN_UI_INFO         PURPLE_PLUGIN_UI_INFO

#define GAIM_PLUGIN_HAS_ACTIONS  PURPLE_PLUGIN_HAS_ACTIONS
#define GAIM_PLUGIN_ACTIONS      PURPLE_PLUGIN_ACTIONS

#define GAIM_INIT_PLUGIN  PURPLE_INIT_PLUGIN

#define gaim_plugin_new              purple_plugin_new
#define gaim_plugin_probe            purple_plugin_probe
#define gaim_plugin_register         purple_plugin_register
#define gaim_plugin_load             purple_plugin_load
#define gaim_plugin_unload           purple_plugin_unload
#define gaim_plugin_reload           purple_plugin_reload
#define gaim_plugin_destroy          purple_plugin_destroy
#define gaim_plugin_is_loaded        purple_plugin_is_loaded
#define gaim_plugin_is_unloadable    purple_plugin_is_unloadable
#define gaim_plugin_get_id           purple_plugin_get_id
#define gaim_plugin_get_name         purple_plugin_get_name
#define gaim_plugin_get_version      purple_plugin_get_version
#define gaim_plugin_get_summary      purple_plugin_get_summary
#define gaim_plugin_get_description  purple_plugin_get_description
#define gaim_plugin_get_author       purple_plugin_get_author
#define gaim_plugin_get_homepage     purple_plugin_get_homepage

#define gaim_plugin_ipc_register        purple_plugin_ipc_register
#define gaim_plugin_ipc_unregister      purple_plugin_ipc_unregister
#define gaim_plugin_ipc_unregister_all  purple_plugin_ipc_unregister_all
#define gaim_plugin_ipc_get_params      purple_plugin_ipc_get_params
#define gaim_plugin_ipc_call            purple_plugin_ipc_call

#define gaim_plugins_add_search_path  purple_plugins_add_search_path
#define gaim_plugins_unload_all       purple_plugins_unload_all
#define gaim_plugins_destroy_all      purple_plugins_destroy_all
#define gaim_plugins_save_loaded      purple_plugins_save_loaded
#define gaim_plugins_load_saved       purple_plugins_load_saved
#define gaim_plugins_probe            purple_plugins_probe
#define gaim_plugins_enabled          purple_plugins_enabled

#define gaim_plugins_register_probe_notify_cb     purple_plugins_register_probe_notify_cb
#define gaim_plugins_unregister_probe_notify_cb   purple_plugins_unregister_probe_notify_cb
#define gaim_plugins_register_load_notify_cb      purple_plugins_register_load_notify_cb
#define gaim_plugins_unregister_load_notify_cb    purple_plugins_unregister_load_notify_cb
#define gaim_plugins_register_unload_notify_cb    purple_plugins_register_unload_notify_cb
#define gaim_plugins_unregister_unload_notify_cb  purple_plugins_unregister_unload_notify_cb

#define gaim_plugins_find_with_name      purple_plugins_find_with_name
#define gaim_plugins_find_with_filename  purple_plugins_find_with_filename
#define gaim_plugins_find_with_basename  purple_plugins_find_with_basename
#define gaim_plugins_find_with_id        purple_plugins_find_with_id

#define gaim_plugins_get_loaded     purple_plugins_get_loaded
#define gaim_plugins_get_protocols  purple_plugins_get_protocols
#define gaim_plugins_get_all        purple_plugins_get_all

#define gaim_plugins_get_handle  purple_plugins_get_handle
#define gaim_plugins_init        purple_plugins_init
#define gaim_plugins_uninit      purple_plugins_uninit

#define gaim_plugin_action_new   purple_plugin_action_new
#define gaim_plugin_action_free  purple_plugin_action_free

/* pluginpref.h */

#define GaimPluginPrefFrame  PurplePluginPrefFrame
#define GaimPluginPref       PurplePluginPref

#define GAIM_STRING_FORMAT_TYPE_NONE       PURPLE_STRING_FORMAT_TYPE_NONE
#define GAIM_STRING_FORMAT_TYPE_MULTILINE  PURPLE_STRING_FORMAT_TYPE_MULTILINE
#define GAIM_STRING_FORMAT_TYPE_HTML       PURPLE_STRING_FORMAT_TYPE_HTML

#define GaimStringFormatType  PurpleStringFormatType

#define GAIM_PLUGIN_PREF_NONE           PURPLE_PLUGIN_PREF_NONE
#define GAIM_PLUGIN_PREF_CHOICE         PURPLE_PLUGIN_PREF_CHOICE
#define GAIM_PLUGIN_PREF_INFO           PURPLE_PLUGIN_PREF_INFO
#define GAIM_PLUGIN_PREF_STRING_FORMAT  PURPLE_PLUGIN_PREF_STRING_FORMAT

#define GaimPluginPrefType  PurplePluginPrefType

#define gaim_plugin_pref_frame_new        purple_plugin_pref_frame_new
#define gaim_plugin_pref_frame_destroy    purple_plugin_pref_frame_destroy
#define gaim_plugin_pref_frame_add        purple_plugin_pref_frame_add
#define gaim_plugin_pref_frame_get_prefs  purple_plugin_pref_frame_get_prefs

#define gaim_plugin_pref_new                      purple_plugin_pref_new
#define gaim_plugin_pref_new_with_name            purple_plugin_pref_new_with_name
#define gaim_plugin_pref_new_with_label           purple_plugin_pref_new_with_label
#define gaim_plugin_pref_new_with_name_and_label  purple_plugin_pref_new_with_name_and_label
#define gaim_plugin_pref_destroy                  purple_plugin_pref_destroy
#define gaim_plugin_pref_set_name                 purple_plugin_pref_set_name
#define gaim_plugin_pref_get_name                 purple_plugin_pref_get_name
#define gaim_plugin_pref_set_label                purple_plugin_pref_set_label
#define gaim_plugin_pref_get_label                purple_plugin_pref_get_label
#define gaim_plugin_pref_set_bounds               purple_plugin_pref_set_bounds
#define gaim_plugin_pref_get_bounds               purple_plugin_pref_get_bounds
#define gaim_plugin_pref_set_type                 purple_plugin_pref_set_type
#define gaim_plugin_pref_get_type                 purple_plugin_pref_get_type
#define gaim_plugin_pref_add_choice               purple_plugin_pref_add_choice
#define gaim_plugin_pref_get_choices              purple_plugin_pref_get_choices
#define gaim_plugin_pref_set_max_length           purple_plugin_pref_set_max_length
#define gaim_plugin_pref_get_max_length           purple_plugin_pref_get_max_length
#define gaim_plugin_pref_set_masked               purple_plugin_pref_set_masked
#define gaim_plugin_pref_get_masked               purple_plugin_pref_get_masked
#define gaim_plugin_pref_set_format_type          purple_plugin_pref_set_format_type
#define gaim_plugin_pref_get_format_type          purple_plugin_pref_get_format_type

/* from pounce.h */

#define GaimPounce  PurplePounce

#define GAIM_POUNCE_NONE              PURPLE_POUNCE_NONE
#define GAIM_POUNCE_SIGNON            PURPLE_POUNCE_SIGNON
#define GAIM_POUNCE_SIGNOFF           PURPLE_POUNCE_SIGNOFF
#define GAIM_POUNCE_AWAY              PURPLE_POUNCE_AWAY
#define GAIM_POUNCE_AWAY_RETURN       PURPLE_POUNCE_AWAY_RETURN
#define GAIM_POUNCE_IDLE              PURPLE_POUNCE_IDLE
#define GAIM_POUNCE_IDLE_RETURN       PURPLE_POUNCE_IDLE_RETURN
#define GAIM_POUNCE_TYPING            PURPLE_POUNCE_TYPING
#define GAIM_POUNCE_TYPED             PURPLE_POUNCE_TYPED
#define GAIM_POUNCE_TYPING_STOPPED    PURPLE_POUNCE_TYPING_STOPPED
#define GAIM_POUNCE_MESSAGE_RECEIVED  PURPLE_POUNCE_MESSAGE_RECEIVED
#define GaimPounceEvent  PurplePounceEvent

#define GAIM_POUNCE_OPTION_NONE  PURPLE_POUNCE_OPTION_NONE
#define GAIM_POUNCE_OPTION_AWAY  PURPLE_POUNCE_OPTION_AWAY
#define GaimPounceOption  PurplePounceOption

#define GaimPounceCb  PurplePounceCb

#define gaim_pounce_new                     purple_pounce_new
#define gaim_pounce_destroy                 purple_pounce_destroy
#define gaim_pounce_destroy_all_by_account  purple_pounce_destroy_all_by_account
#define gaim_pounce_set_events              purple_pounce_set_events
#define gaim_pounce_set_options             purple_pounce_set_options
#define gaim_pounce_set_pouncer             purple_pounce_set_pouncer
#define gaim_pounce_set_pouncee             purple_pounce_set_pouncee
#define gaim_pounce_set_save                purple_pounce_set_save
#define gaim_pounce_action_register         purple_pounce_action_register
#define gaim_pounce_action_set_enabled      purple_pounce_action_set_enabled
#define gaim_pounce_action_set_attribute    purple_pounce_action_set_attribute
#define gaim_pounce_set_data                purple_pounce_set_data
#define gaim_pounce_get_events              purple_pounce_get_events
#define gaim_pounce_get_options             purple_pounce_get_options
#define gaim_pounce_get_pouncer             purple_pounce_get_pouncer
#define gaim_pounce_get_pouncee             purple_pounce_get_pouncee
#define gaim_pounce_get_save                purple_pounce_get_save
#define gaim_pounce_action_is_enabled       purple_pounce_action_is_enabled
#define gaim_pounce_action_get_attribute    purple_pounce_action_get_attribute
#define gaim_pounce_get_data                purple_pounce_get_data
#define gaim_pounce_execute                 purple_pounce_execute

#define gaim_find_pounce                 purple_find_pounce
#define gaim_pounces_load                purple_pounces_load
#define gaim_pounces_register_handler    purple_pounces_register_handler
#define gaim_pounces_unregister_handler  purple_pounces_unregister_handler
#define gaim_pounces_get_all             purple_pounces_get_all
#define gaim_pounces_get_handle          purple_pounces_get_handle
#define gaim_pounces_init                purple_pounces_init
#define gaim_pounces_uninit              purple_pounces_uninit

/* from prefs.h */


#define GAIM_PREF_NONE         PURPLE_PREF_NONE
#define GAIM_PREF_BOOLEAN      PURPLE_PREF_BOOLEAN
#define GAIM_PREF_INT          PURPLE_PREF_INT
#define GAIM_PREF_STRING       PURPLE_PREF_STRING
#define GAIM_PREF_STRING_LIST  PURPLE_PREF_STRING_LIST
#define GAIM_PREF_PATH         PURPLE_PREF_PATH
#define GAIM_PREF_PATH_LIST    PURPLE_PREF_PATH_LIST
#define GaimPrefType  PurplePrefType

#define GaimPrefCallback  PurplePrefCallback

#define gaim_prefs_get_handle             purple_prefs_get_handle
#define gaim_prefs_init                   purple_prefs_init
#define gaim_prefs_uninit                 purple_prefs_uninit
#define gaim_prefs_add_none               purple_prefs_add_none
#define gaim_prefs_add_bool               purple_prefs_add_bool
#define gaim_prefs_add_int                purple_prefs_add_int
#define gaim_prefs_add_string             purple_prefs_add_string
#define gaim_prefs_add_string_list        purple_prefs_add_string_list
#define gaim_prefs_add_path               purple_prefs_add_path
#define gaim_prefs_add_path_list          purple_prefs_add_path_list
#define gaim_prefs_remove                 purple_prefs_remove
#define gaim_prefs_rename                 purple_prefs_rename
#define gaim_prefs_rename_boolean_toggle  purple_prefs_rename_boolean_toggle
#define gaim_prefs_destroy                purple_prefs_destroy
#define gaim_prefs_set_generic            purple_prefs_set_generic
#define gaim_prefs_set_bool               purple_prefs_set_bool
#define gaim_prefs_set_int                purple_prefs_set_int
#define gaim_prefs_set_string             purple_prefs_set_string
#define gaim_prefs_set_string_list        purple_prefs_set_string_list
#define gaim_prefs_set_path               purple_prefs_set_path
#define gaim_prefs_set_path_list          purple_prefs_set_path_list
#define gaim_prefs_exists                 purple_prefs_exists
#define gaim_prefs_get_type               purple_prefs_get_type
#define gaim_prefs_get_bool               purple_prefs_get_bool
#define gaim_prefs_get_int                purple_prefs_get_int
#define gaim_prefs_get_string             purple_prefs_get_string
#define gaim_prefs_get_string_list        purple_prefs_get_string_list
#define gaim_prefs_get_path               purple_prefs_get_path
#define gaim_prefs_get_path_list          purple_prefs_get_path_list
#define gaim_prefs_connect_callback       purple_prefs_connect_callback
#define gaim_prefs_disconnect_callback    purple_prefs_disconnect_callback
#define gaim_prefs_disconnect_by_handle   purple_prefs_disconnect_by_handle
#define gaim_prefs_trigger_callback       purple_prefs_trigger_callback
#define gaim_prefs_load                   purple_prefs_load
#define gaim_prefs_update_old             purple_prefs_update_old

/* from privacy.h */

#define GAIM_PRIVACY_ALLOW_ALL        PURPLE_PRIVACY_ALLOW_ALL
#define GAIM_PRIVACY_DENY_ALL         PURPLE_PRIVACY_DENY_ALL
#define GAIM_PRIVACY_ALLOW_USERS      PURPLE_PRIVACY_ALLOW_USERS
#define GAIM_PRIVACY_DENY_USERS       PURPLE_PRIVACY_DENY_USERS
#define GAIM_PRIVACY_ALLOW_BUDDYLIST  PURPLE_PRIVACY_ALLOW_BUDDYLIST
#define GaimPrivacyType  PurplePrivacyType

#define GaimPrivacyUiOps  PurplePrivacyUiOps

#define gaim_privacy_permit_add     purple_privacy_permit_add
#define gaim_privacy_permit_remove  purple_privacy_permit_remove
#define gaim_privacy_deny_add       purple_privacy_deny_add
#define gaim_privacy_deny_remove    purple_privacy_deny_remove
#define gaim_privacy_allow          purple_privacy_allow
#define gaim_privacy_deny           purple_privacy_deny
#define gaim_privacy_check          purple_privacy_check
#define gaim_privacy_set_ui_ops     purple_privacy_set_ui_ops
#define gaim_privacy_get_ui_ops     purple_privacy_get_ui_ops
#define gaim_privacy_init           purple_privacy_init

/* from proxy.h */

#define GAIM_PROXY_USE_GLOBAL  PURPLE_PROXY_USE_GLOBAL
#define GAIM_PROXY_NONE        PURPLE_PROXY_NONE
#define GAIM_PROXY_HTTP        PURPLE_PROXY_HTTP
#define GAIM_PROXY_SOCKS4      PURPLE_PROXY_SOCKS4
#define GAIM_PROXY_SOCKS5      PURPLE_PROXY_SOCKS5
#define GAIM_PROXY_USE_ENVVAR  PURPLE_PROXY_USE_ENVVAR
#define GaimProxyType  PurpleProxyType

#define GaimProxyInfo  PurpleProxyInfo

#define GaimProxyConnectData      PurpleProxyConnectData
#define GaimProxyConnectFunction  PurpleProxyConnectFunction

#define gaim_proxy_info_new           purple_proxy_info_new
#define gaim_proxy_info_destroy       purple_proxy_info_destroy
#define gaim_proxy_info_set_type      purple_proxy_info_set_type
#define gaim_proxy_info_set_host      purple_proxy_info_set_host
#define gaim_proxy_info_set_port      purple_proxy_info_set_port
#define gaim_proxy_info_set_username  purple_proxy_info_set_username
#define gaim_proxy_info_set_password  purple_proxy_info_set_password
#define gaim_proxy_info_get_type      purple_proxy_info_get_type
#define gaim_proxy_info_get_host      purple_proxy_info_get_host
#define gaim_proxy_info_get_port      purple_proxy_info_get_port
#define gaim_proxy_info_get_username  purple_proxy_info_get_username
#define gaim_proxy_info_get_password  purple_proxy_info_get_password

#define gaim_global_proxy_get_info    purple_global_proxy_get_info
#define gaim_proxy_get_handle         purple_proxy_get_handle
#define gaim_proxy_init               purple_proxy_init
#define gaim_proxy_uninit             purple_proxy_uninit
#define gaim_proxy_get_setup          purple_proxy_get_setup

#define gaim_proxy_connect                     purple_proxy_connect
#define gaim_proxy_connect_socks5              purple_proxy_connect_socks5
#define gaim_proxy_connect_cancel              purple_proxy_connect_cancel
#define gaim_proxy_connect_cancel_with_handle  purple_proxy_connect_cancel_with_handle

/* from prpl.h */

#define GaimPluginProtocolInfo  PurplePluginProtocolInfo

#define GAIM_ICON_SCALE_DISPLAY  PURPLE_ICON_SCALE_DISPLAY
#define GAIM_ICON_SCALE_SEND     PURPLE_ICON_SCALE_SEND
#define GaimIconScaleRules  PurpleIconScaleRules

#define GaimBuddyIconSpec  PurpleBuddyIconSpec

#define GaimProtocolOptions  PurpleProtocolOptions

#define GAIM_IS_PROTOCOL_PLUGIN  PURPLE_IS_PROTOCOL_PLUGIN

#define GAIM_PLUGIN_PROTOCOL_INFO  PURPLE_PLUGIN_PROTOCOL_INFO

#define gaim_prpl_got_account_idle        purple_prpl_got_account_idle
#define gaim_prpl_got_account_login_time  purple_prpl_got_account_login_time
#define gaim_prpl_got_account_status      purple_prpl_got_account_status
#define gaim_prpl_got_user_idle           purple_prpl_got_user_idle
#define gaim_prpl_got_user_login_time     purple_prpl_got_user_login_time
#define gaim_prpl_got_user_status         purple_prpl_got_user_status
#define gaim_prpl_change_account_status   purple_prpl_change_account_status
#define gaim_prpl_get_statuses            purple_prpl_get_statuses

#define gaim_find_prpl  purple_find_prpl

/* from request.h */

#define GAIM_DEFAULT_ACTION_NONE  PURPLE_DEFAULT_ACTION_NONE

#define GAIM_REQUEST_INPUT   PURPLE_REQUEST_INPUT
#define GAIM_REQUEST_CHOICE  PURPLE_REQUEST_CHOICE
#define GAIM_REQUEST_ACTION  PURPLE_REQUEST_ACTION
#define GAIM_REQUEST_FIELDS  PURPLE_REQUEST_FIELDS
#define GAIM_REQUEST_FILE    PURPLE_REQUEST_FILE
#define GAIM_REQUEST_FOLDER  PURPLE_REQUEST_FOLDER
#define GaimRequestType  PurpleRequestType

#define GAIM_REQUEST_FIELD_NONE     PURPLE_REQUEST_FIELD_NONE
#define GAIM_REQUEST_FIELD_STRING   PURPLE_REQUEST_FIELD_STRING
#define GAIM_REQUEST_FIELD_INTEGER  PURPLE_REQUEST_FIELD_INTEGER
#define GAIM_REQUEST_FIELD_BOOLEAN  PURPLE_REQUEST_FIELD_BOOLEAN
#define GAIM_REQUEST_FIELD_CHOICE   PURPLE_REQUEST_FIELD_CHOICE
#define GAIM_REQUEST_FIELD_LIST     PURPLE_REQUEST_FIELD_LIST
#define GAIM_REQUEST_FIELD_LABEL    PURPLE_REQUEST_FIELD_LABEL
#define GAIM_REQUEST_FIELD_IMAGE    PURPLE_REQUEST_FIELD_IMAGE
#define GAIM_REQUEST_FIELD_ACCOUNT  PURPLE_REQUEST_FIELD_ACCOUNT
#define GaimRequestFieldType  PurpleRequestFieldType

#define GaimRequestFields  PurpleRequestFields

#define GaimRequestFieldGroup  PurpleRequestFieldGroup

#define GaimRequestField  PurpleRequestField

#define GaimRequestUiOps  PurpleRequestUiOps

#define GaimRequestInputCb   PurpleRequestInputCb
#define GaimRequestActionCb  PurpleRequestActionCb
#define GaimRequestChoiceCb  PurpleRequestChoiceCb
#define GaimRequestFieldsCb  PurpleRequestFieldsCb
#define GaimRequestFileCb    PurpleRequestFileCb

#define gaim_request_fields_new                  purple_request_fields_new
#define gaim_request_fields_destroy              purple_request_fields_destroy
#define gaim_request_fields_add_group            purple_request_fields_add_group
#define gaim_request_fields_get_groups           purple_request_fields_get_groups
#define gaim_request_fields_exists               purple_request_fields_exists
#define gaim_request_fields_get_required         purple_request_fields_get_required
#define gaim_request_fields_is_field_required    purple_request_fields_is_field_required
#define gaim_request_fields_all_required_filled  purple_request_fields_all_required_filled
#define gaim_request_fields_get_field            purple_request_fields_get_field
#define gaim_request_fields_get_string           purple_request_fields_get_string
#define gaim_request_fields_get_integer          purple_request_fields_get_integer
#define gaim_request_fields_get_bool             purple_request_fields_get_bool
#define gaim_request_fields_get_choice           purple_request_fields_get_choice
#define gaim_request_fields_get_account          purple_request_fields_get_account

#define gaim_request_field_group_new         purple_request_field_group_new
#define gaim_request_field_group_destroy     purple_request_field_group_destroy
#define gaim_request_field_group_add_field   purple_request_field_group_add_field
#define gaim_request_field_group_get_title   purple_request_field_group_get_title
#define gaim_request_field_group_get_fields  purple_request_field_group_get_fields

#define gaim_request_field_new            purple_request_field_new
#define gaim_request_field_destroy        purple_request_field_destroy
#define gaim_request_field_set_label      purple_request_field_set_label
#define gaim_request_field_set_visible    purple_request_field_set_visible
#define gaim_request_field_set_type_hint  purple_request_field_set_type_hint
#define gaim_request_field_set_required   purple_request_field_set_required
#define gaim_request_field_get_type       purple_request_field_get_type
#define gaim_request_field_get_id         purple_request_field_get_id
#define gaim_request_field_get_label      purple_request_field_get_label
#define gaim_request_field_is_visible     purple_request_field_is_visible
#define gaim_request_field_get_type_hint  purple_request_field_get_type_hint
#define gaim_request_field_is_required    purple_request_field_is_required

#define gaim_request_field_string_new           purple_request_field_string_new
#define gaim_request_field_string_set_default_value \
	purple_request_field_string_set_default_value
#define gaim_request_field_string_set_value     purple_request_field_string_set_value
#define gaim_request_field_string_set_masked    purple_request_field_string_set_masked
#define gaim_request_field_string_set_editable  purple_request_field_string_set_editable
#define gaim_request_field_string_get_default_value \
	purple_request_field_string_get_default_value
#define gaim_request_field_string_get_value     purple_request_field_string_get_value
#define gaim_request_field_string_is_multiline  purple_request_field_string_is_multiline
#define gaim_request_field_string_is_masked     purple_request_field_string_is_masked
#define gaim_request_field_string_is_editable   purple_request_field_string_is_editable

#define gaim_request_field_int_new        purple_request_field_int_new
#define gaim_request_field_int_set_default_value \
	purple_request_field_int_set_default_value
#define gaim_request_field_int_set_value  purple_request_field_int_set_value
#define gaim_request_field_int_get_default_value \
	purple_request_field_int_get_default_value
#define gaim_request_field_int_get_value  purple_request_field_int_get_value

#define gaim_request_field_bool_new        purple_request_field_bool_new
#define gaim_request_field_bool_set_default_value \
	purple_request_field_book_set_default_value
#define gaim_request_field_bool_set_value  purple_request_field_bool_set_value
#define gaim_request_field_bool_get_default_value \
	purple_request_field_bool_get_default_value
#define gaim_request_field_bool_get_value  purple_request_field_bool_get_value

#define gaim_request_field_choice_new         purple_request_field_choice_new
#define gaim_request_field_choice_add         purple_request_field_choice_add
#define gaim_request_field_choice_set_default_value \
	purple_request_field_choice_set_default_value
#define gaim_request_field_choice_set_value   purple_request_field_choice_set_value
#define gaim_request_field_choice_get_default_value \
	purple_request_field_choice_get_default_value
#define gaim_request_field_choice_get_value   purple_request_field_choice_get_value
#define gaim_request_field_choice_get_labels  purple_request_field_choice_get_labels

#define gaim_request_field_list_new               purple_request_field_list_new
#define gaim_request_field_list_set_multi_select  purple_request_field_list_set_multi_select
#define gaim_request_field_list_get_multi_select  purple_request_field_list_get_multi_select
#define gaim_request_field_list_get_data          purple_request_field_list_get_data
#define gaim_request_field_list_add               purple_request_field_list_add
#define gaim_request_field_list_add_selected      purple_request_field_list_add_selected
#define gaim_request_field_list_clear_selected    purple_request_field_list_clear_selected
#define gaim_request_field_list_set_selected      purple_request_field_list_set_selected
#define gaim_request_field_list_is_selected       purple_request_field_list_is_selected
#define gaim_request_field_list_get_selected      purple_request_field_list_get_selected
#define gaim_request_field_list_get_items         purple_request_field_list_get_items

#define gaim_request_field_label_new  purple_request_field_label_new

#define gaim_request_field_image_new          purple_request_field_image_new
#define gaim_request_field_image_set_scale    purple_request_field_image_set_scale
#define gaim_request_field_image_get_buffer   purple_request_field_image_get_buffer
#define gaim_request_field_image_get_size     purple_request_field_image_get_size
#define gaim_request_field_image_get_scale_x  purple_request_field_image_get_scale_x
#define gaim_request_field_image_get_scale_y  purple_request_field_image_get_scale_y

#define gaim_request_field_account_new                purple_request_field_account_new
#define gaim_request_field_account_set_default_value  purple_request_field_account_set_default_value
#define gaim_request_field_account_set_value          purple_request_field_account_set_value
#define gaim_request_field_account_set_show_all       purple_request_field_account_set_show_all
#define gaim_request_field_account_set_filter         purple_request_field_account_set_filter
#define gaim_request_field_account_get_default_value  purple_request_field_account_get_default_value
#define gaim_request_field_account_get_value          purple_request_field_account_get_value
#define gaim_request_field_account_get_show_all       purple_request_field_account_get_show_all
#define gaim_request_field_account_get_filter         purple_request_field_account_get_filter

#define gaim_request_input              purple_request_input
#define gaim_request_choice             purple_request_choice
#define gaim_request_choice_varg        purple_request_choice_varg
#define gaim_request_action             purple_request_action
#define gaim_request_action_varg        purple_request_action_varg
#define gaim_request_fields(handle, title, primary, secondary, fields, ok_text, ok_cb, cancel_text, cancel_cb, user_data)             purple_request_fields(handle, title, primary, secondary, fields, ok_text, ok_cb, cancel_text, cancel_cb, NULL, NULL, NULL, user_data)
#define gaim_request_close              purple_request_close
#define gaim_request_close_with_handle  purple_request_close_with_handle

#define gaim_request_yes_no         purple_request_yes_no
#define gaim_request_ok_cancel      purple_request_ok_cancel
#define gaim_request_accept_cancel  purple_request_accept_cancel

#define gaim_request_file    purple_request_file
#define gaim_request_folder  purple_request_folder

#define gaim_request_set_ui_ops  purple_request_set_ui_ops
#define gaim_request_get_ui_ops  purple_request_get_ui_ops

/* from roomlist.h */

#define GaimRoomlist       PurpleRoomlist
#define GaimRoomlistRoom   PurpleRoomlistRoom
#define GaimRoomlistField  PurpleRoomlistField
#define GaimRoomlistUiOps  PurpleRoomlistUiOps

#define GAIM_ROOMLIST_ROOMTYPE_CATEGORY  PURPLE_ROOMLIST_ROOMTYPE_CATEGORY
#define GAIM_ROOMLIST_ROOMTYPE_ROOM      PURPLE_ROOMLIST_ROOMTYPE_ROOM
#define GaimRoomlistRoomType  PurpleRoomlistRoomType

#define GAIM_ROOMLIST_FIELD_BOOL    PURPLE_ROOMLIST_BOOL
#define GAIM_ROOMLIST_FIELD_INT     PURPLE_ROOMLIST_INT
#define GAIM_ROOMLIST_FIELD_STRING  PURPLE_ROOMLIST_STRING
#define GaimRoomlistFieldType  PurpleRoomlistFieldType

#define gaim_roomlist_show_with_account  purple_roomlist_show_with_account
#define gaim_roomlist_new                purple_roomlist_new
#define gaim_roomlist_ref                purple_roomlist_ref
#define gaim_roomlist_unref              purple_roomlist_unref
#define gaim_roomlist_set_fields         purple_roomlist_set_fields
#define gaim_roomlist_set_in_progress    purple_roomlist_set_in_progress
#define gaim_roomlist_get_in_progress    purple_roomlist_get_in_progress
#define gaim_roomlist_room_add           purple_roomlist_room_add

#define gaim_roomlist_get_list         purple_roomlist_get_list
#define gaim_roomlist_cancel_get_list  purple_roomlist_cancel_get_list
#define gaim_roomlist_expand_category  purple_roomlist_expand_category

#define gaim_roomlist_room_new        purple_roomlist_room_new
#define gaim_roomlist_room_add_field  purple_roomlist_room_add_field
#define gaim_roomlist_room_join       purple_roomlist_room_join
#define gaim_roomlist_field_new       purple_roomlist_field_new

#define gaim_roomlist_set_ui_ops  purple_roomlist_set_ui_ops
#define gaim_roomlist_get_ui_ops  purple_roomlist_get_ui_ops

/* from savedstatuses.h */

#define GaimSavedStatus     PurpleSavedStatus
#define GaimSavedStatusSub  PurpleSavedStatusSub

#define gaim_savedstatus_new              purple_savedstatus_new
#define gaim_savedstatus_set_title        purple_savedstatus_set_title
#define gaim_savedstatus_set_type         purple_savedstatus_set_type
#define gaim_savedstatus_set_message      purple_savedstatus_set_message
#define gaim_savedstatus_set_substatus    purple_savedstatus_set_substatus
#define gaim_savedstatus_unset_substatus  purple_savedstatus_unset_substatus
#define gaim_savedstatus_delete           purple_savedstatus_delete

#define gaim_savedstatuses_get_all              purple_savedstatuses_get_all
#define gaim_savedstatuses_get_popular          purple_savedstatuses_get_popular
#define gaim_savedstatus_get_current            purple_savedstatus_get_current
#define gaim_savedstatus_get_default            purple_savedstatus_get_default
#define gaim_savedstatus_get_idleaway           purple_savedstatus_get_idleaway
#define gaim_savedstatus_is_idleaway            purple_savedstatus_is_idleaway
#define gaim_savedstatus_set_idleaway           purple_savedstatus_set_idleaway
#define gaim_savedstatus_get_startup            purple_savedstatus_get_startup
#define gaim_savedstatus_find                   purple_savedstatus_find
#define gaim_savedstatus_find_by_creation_time  purple_savedstatus_find_by_creation_time
#define gaim_savedstatus_find_transient_by_type_and_message \
	purple_savedstatus_find_transient_by_type_and_message

#define gaim_savedstatus_is_transient           purple_savedstatus_is_transient
#define gaim_savedstatus_get_title              purple_savedstatus_get_title
#define gaim_savedstatus_get_type               purple_savedstatus_get_type
#define gaim_savedstatus_get_message            purple_savedstatus_get_message
#define gaim_savedstatus_get_creation_time      purple_savedstatus_get_creation_time
#define gaim_savedstatus_has_substatuses        purple_savedstatus_has_substatuses
#define gaim_savedstatus_get_substatus          purple_savedstatus_get_substatus
#define gaim_savedstatus_substatus_get_type     purple_savedstatus_substatus_get_type
#define gaim_savedstatus_substatus_get_message  purple_savedstatus_substatus_get_message
#define gaim_savedstatus_activate               purple_savedstatus_activate
#define gaim_savedstatus_activate_for_account   purple_savedstatus_activate_for_account

#define gaim_savedstatuses_get_handle  purple_savedstatuses_get_handle
#define gaim_savedstatuses_init        purple_savedstatuses_init
#define gaim_savedstatuses_uninit      purple_savedstatuses_uninit

/* from signals.h */

#define GAIM_CALLBACK  PURPLE_CALLBACK

#define GaimCallback           PurpleCallback
#define GaimSignalMarshalFunc  PurpleSignalMarshalFunc

#define GAIM_SIGNAL_PRIORITY_DEFAULT  PURPLE_SIGNAL_PRIORITY_DEFAULT
#define GAIM_SIGNAL_PRIORITY_HIGHEST  PURPLE_SIGNAL_PRIORITY_HIGHEST
#define GAIM_SIGNAL_PRIORITY_LOWEST   PURPLE_SIGNAL_PRIORITY_LOWEST

#define gaim_signal_register    purple_signal_register
#define gaim_signal_unregister  purple_signal_unregister

#define gaim_signals_unregister_by_instance  purple_signals_unregister_by_instance

#define gaim_signal_get_values              purple_signal_get_values
#define gaim_signal_connect_priority        purple_signal_connect_priority
#define gaim_signal_connect                 purple_signal_connect
#define gaim_signal_connect_priority_vargs  purple_signal_connect_priority_vargs
#define gaim_signal_connect_vargs           purple_signal_connect_vargs
#define gaim_signal_disconnect              purple_signal_disconnect

#define gaim_signals_disconnect_by_handle  purple_signals_disconnect_by_handle

#define gaim_signal_emit                 purple_signal_emit
#define gaim_signal_emit_vargs           purple_signal_emit_vargs
#define gaim_signal_emit_return_1        purple_signal_emit_vargs
#define gaim_signal_emit_vargs_return_1  purple_signal_emit_vargs_return_1

#define gaim_signals_init    purple_signals_init
#define gaim_signals_uninit  purple_signals_uninit

#define gaim_marshal_VOID \
	purple_marshal_VOID
#define gaim_marshal_VOID__INT \
	purple_marshal_VOID__INT
#define gaim_marshal_VOID__INT_INT \
	purple_marshal_VOID_INT_INT
#define gaim_marshal_VOID__POINTER \
	purple_marshal_VOID__POINTER
#define gaim_marshal_VOID__POINTER_UINT \
	purple_marshal_VOID__POINTER_UINT
#define gaim_marshal_VOID__POINTER_INT_INT \
	purple_marshal_VOID__POINTER_INT_INT
#define gaim_marshal_VOID__POINTER_POINTER \
	purple_marshal_VOID__POINTER_POINTER
#define gaim_marshal_VOID__POINTER_POINTER_UINT \
	purple_marshal_VOID__POINTER_POINTER_UINT
#define gaim_marshal_VOID__POINTER_POINTER_UINT_UINT \
	purple_marshal_VOID__POINTER_POINTER_UINT_UINT
#define gaim_marshal_VOID__POINTER_POINTER_POINTER \
	purple_marshal_VOID__POINTER_POINTER_POINTER
#define gaim_marshal_VOID__POINTER_POINTER_POINTER_POINTER \
	purple_marshal_VOID__POINTER_POINTER_POINTER_POINTER
#define gaim_marshal_VOID__POINTER_POINTER_POINTER_POINTER_POINTER \
	purple_marshal_VOID__POINTER_POINTER_POINTER_POINTER_POINTER
#define gaim_marshal_VOID__POINTER_POINTER_POINTER_UINT \
	purple_marshal_VOID__POINTER_POINTER_POINTER_UINT
#define gaim_marshal_VOID__POINTER_POINTER_POINTER_POINTER_UINT \
	purple_marshal_VOID__POINTER_POINTER_POINTER_POINTER_UINT
#define gaim_marshal_VOID__POINTER_POINTER_POINTER_UINT_UINT \
	purple_marshal_VOID__POINTER_POINTER_POINTER_UINT_UINT

#define gaim_marshal_INT__INT \
	purple_marshal_INT__INT
#define gaim_marshal_INT__INT_INT \
	purple_marshal_INT__INT_INT
#define gaim_marshal_INT__POINTER_POINTER_POINTER_POINTER_POINTER \
	purple_marshal_INT__POINTER_POINTER_POINTER_POINTER_POINTER

#define gaim_marshal_BOOLEAN__POINTER \
	purple_marshal_BOOLEAN__POINTER
#define gaim_marshal_BOOLEAN__POINTER_POINTER \
	purple_marshal_BOOLEAN__POINTER_POINTER
#define gaim_marshal_BOOLEAN__POINTER_POINTER_POINTER \
	purple_marshal_BOOLEAN__POINTER_POINTER_POINTER
#define gaim_marshal_BOOLEAN__POINTER_POINTER_UINT \
	purple_marshal_BOOLEAN__POINTER_POINTER_UINT
#define gaim_marshal_BOOLEAN__POINTER_POINTER_POINTER_UINT \
	purple_marshal_BOOLEAN__POINTER_POINTER_POINTER_UINT
#define gaim_marshal_BOOLEAN__POINTER_POINTER_POINTER_POINTER \
	purple_marshal_BOOLEAN__POINTER_POINTER_POINTER_POINTER
#define gaim_marshal_BOOLEAN__POINTER_POINTER_POINTER_POINTER_POINTER \
	purple_marshal_BOOLEAN__POINTER_POINTER_POINTER_POINTER_POINTER

#define gaim_marshal_BOOLEAN__INT_POINTER \
	purple_marshal_BOOLEAN__INT_POINTER

#define gaim_marshal_POINTER__POINTER_INT \
	purple_marshal_POINTER__POINTER_INT
#define gaim_marshal_POINTER__POINTER_INT64 \
	purple_marshal_POINTER__POINTER_INT64
#define gaim_marshal_POINTER__POINTER_INT_BOOLEAN \
	purple_marshal_POINTER__POINTER_INT_BOOLEAN
#define gaim_marshal_POINTER__POINTER_INT64_BOOLEAN \
	purple_marshal_POINTER__POINTER_INT64_BOOLEAN
#define gaim_marshal_POINTER__POINTER_POINTER \
	purple_marshal_POINTER__POINTER_POINTER

/* from sound.h */

#define GAIM_SOUND_BUDDY_ARRIVE    PURPLE_SOUND_BUDDY_ARRIVE
#define GAIM_SOUND_BUDDY_LEAVE     PURPLE_SOUND_BUDDY_LEAVE
#define GAIM_SOUND_RECEIVE         PURPLE_SOUND_RECEIVE
#define GAIM_SOUND_FIRST_RECEIVE   PURPLE_SOUND_FIRST_RECEIVE
#define GAIM_SOUND_SEND            PURPLE_SOUND_SEND
#define GAIM_SOUND_CHAT_JOIN       PURPLE_SOUND_CHAT_JOIN
#define GAIM_SOUND_CHAT_LEAVE      PURPLE_SOUND_CHAT_LEAVE
#define GAIM_SOUND_CHAT_YOU_SAY    PURPLE_SOUND_CHAT_YOU_SAY
#define GAIM_SOUND_CHAT_SAY        PURPLE_SOUND_CHAT_SAY
#define GAIM_SOUND_POUNCE_DEFAULT  PURPLE_SOUND_POUNCE_DEFAULT
#define GAIM_SOUND_CHAT_NICK       PURPLE_SOUND_CHAT_NICK
#define GAIM_NUM_SOUNDS            PURPLE_NUM_SOUNDS
#define GaimSoundEventID  PurpleSoundEventID

#define GaimSoundUiOps  PurpleSoundUiOps

#define gaim_sound_play_file   purple_sound_play_file
#define gaim_sound_play_event  purple_sound_play_event
#define gaim_sound_set_ui_ops  purple_sound_set_ui_ops
#define gaim_sound_get_ui_ops  purple_sound_get_ui_ops
#define gaim_sound_init        purple_sound_init
#define gaim_sound_uninit      purple_sound_uninit

#define gaim_sounds_get_handle  purple_sounds_get_handle

/* from sslconn.h */

#define GAIM_SSL_DEFAULT_PORT  PURPLE_SSL_DEFAULT_PORT

#define GAIM_SSL_HANDSHAKE_FAILED  PURPLE_SSL_HANDSHAKE_FAILED
#define GAIM_SSL_CONNECT_FAILED    PURPLE_SSL_CONNECT_FAILED
#define GaimSslErrorType  PurpleSslErrorType

#define GaimSslConnection  PurpleSslConnection

#define GaimSslInputFunction  PurpleSslInputFunction
#define GaimSslErrorFunction  PurpleSslErrorFunction

#define GaimSslOps  PurpleSslOps

#define gaim_ssl_is_supported  purple_ssl_is_supported
#define gaim_ssl_connect       purple_ssl_connect
#define gaim_ssl_connect_fd    purple_ssl_connect_fd
#define gaim_ssl_input_add     purple_ssl_input_add
#define gaim_ssl_close         purple_ssl_close
#define gaim_ssl_read          purple_ssl_read
#define gaim_ssl_write         purple_ssl_write

#define gaim_ssl_set_ops  purple_ssl_set_ops
#define gaim_ssl_get_ops  purple_ssl_get_ops
#define gaim_ssl_init     purple_ssl_init
#define gaim_ssl_uninit   purple_ssl_uninit

/* from status.h */

#define GaimStatusType  PurpleStatusType
#define GaimStatusAttr  PurpleStatusAttr
#define GaimPresence    PurplePresence
#define GaimStatus      PurpleStatus

#define GAIM_PRESENCE_CONTEXT_UNSET    PURPLE_PRESENCE_CONTEXT_UNSET
#define GAIM_PRESENCE_CONTEXT_ACCOUNT  PURPLE_PRESENCE_CONTEXT_ACCOUNT
#define GAIM_PRESENCE_CONTEXT_CONV     PURPLE_PRESENCE_CONTEXT_CONV
#define GAIM_PRESENCE_CONTEXT_BUDDY    PURPLE_PRESENCE_CONTEXT_BUDDY
#define GaimPresenceContext  PurplePresenceContext

#define GAIM_STATUS_UNSET           PURPLE_STATUS_UNSET
#define GAIM_STATUS_OFFLINE         PURPLE_STATUS_OFFLINE
#define GAIM_STATUS_AVAILABLE       PURPLE_STATUS_AVAILABLE
#define GAIM_STATUS_UNAVAILABLE     PURPLE_STATUS_UNAVAILABLE
#define GAIM_STATUS_INVISIBLE       PURPLE_STATUS_INVISIBLE
#define GAIM_STATUS_AWAY            PURPLE_STATUS_AWAY
#define GAIM_STATUS_EXTENDED_AWAY   PURPLE_STATUS_EXTENDED_AWAY
#define GAIM_STATUS_MOBILE          PURPLE_STATUS_MOBILE
#define GAIM_STATUS_NUM_PRIMITIVES  PURPLE_STATUS_NUM_PRIMITIVES
#define GaimStatusPrimitive  PurpleStatusPrimitive

#define gaim_primitive_get_id_from_type    purple_primitive_get_id_from_type
#define gaim_primitive_get_name_from_type  purple_primitive_get_name_from_type
#define gaim_primitive_get_type_from_id    purple_primitive_get_type_from_id

#define gaim_status_type_new_full          purple_status_type_new_full
#define gaim_status_type_new               purple_status_type_new
#define gaim_status_type_new_with_attrs    purple_status_type_new_with_attrs
#define gaim_status_type_destroy           purple_status_type_destroy
#define gaim_status_type_set_primary_attr  purple_status_type_set_primary_attr
#define gaim_status_type_add_attr          purple_status_type_add_attr
#define gaim_status_type_add_attrs         purple_status_type_add_attrs
#define gaim_status_type_add_attrs_vargs   purple_status_type_add_attrs_vargs
#define gaim_status_type_get_primitive     purple_status_type_get_primitive
#define gaim_status_type_get_id            purple_status_type_get_id
#define gaim_status_type_get_name          purple_status_type_get_name
#define gaim_status_type_is_saveable       purple_status_type_is_saveable
#define gaim_status_type_is_user_settable  purple_status_type_is_user_settable
#define gaim_status_type_is_independent    purple_status_type_is_independent
#define gaim_status_type_is_exclusive      purple_status_type_is_exclusive
#define gaim_status_type_is_available      purple_status_type_is_available
#define gaim_status_type_get_primary_attr  purple_status_type_get_primary_attr
#define gaim_status_type_get_attr          purple_status_type_get_attr
#define gaim_status_type_get_attrs         purple_status_type_get_attrs
#define gaim_status_type_find_with_id      purple_status_type_find_with_id

#define gaim_status_attr_new        purple_status_attr_new
#define gaim_status_attr_destroy    purple_status_attr_destroy
#define gaim_status_attr_get_id     purple_status_attr_get_id
#define gaim_status_attr_get_name   purple_status_attr_get_name
#define gaim_status_attr_get_value  purple_status_attr_get_value

#define gaim_status_new                         purple_status_new
#define gaim_status_destroy                     purple_status_destroy
#define gaim_status_set_active                  purple_status_set_active
#define gaim_status_set_active_with_attrs       purple_status_set_active_with_attrs
#define gaim_status_set_active_with_attrs_list  purple_status_set_active_with_attrs_list
#define gaim_status_set_attr_boolean            purple_status_set_attr_boolean
#define gaim_status_set_attr_int                purple_status_set_attr_int
#define gaim_status_set_attr_string             purple_status_set_attr_string
#define gaim_status_get_type                    purple_status_get_type
#define gaim_status_get_presence                purple_status_get_presence
#define gaim_status_get_id                      purple_status_get_id
#define gaim_status_get_name                    purple_status_get_name
#define gaim_status_is_independent              purple_status_is_independent
#define gaim_status_is_exclusive                purple_status_is_exclusive
#define gaim_status_is_available                purple_status_is_available
#define gaim_status_is_active                   purple_status_is_active
#define gaim_status_is_online                   purple_status_is_online
#define gaim_status_get_attr_value              purple_status_get_attr_value
#define gaim_status_get_attr_boolean            purple_status_get_attr_boolean
#define gaim_status_get_attr_int                purple_status_get_attr_int
#define gaim_status_get_attr_string             purple_status_get_attr_string
#define gaim_status_compare                     purple_status_compare

#define gaim_presence_new                purple_presence_new
#define gaim_presence_new_for_account    purple_presence_new_for_account
#define gaim_presence_new_for_conv       purple_presence_new_for_conv
#define gaim_presence_new_for_buddy      purple_presence_new_for_buddy
#define gaim_presence_destroy            purple_presence_destroy
#define gaim_presence_add_status         purple_presence_add_status
#define gaim_presence_add_list           purple_presence_add_list
#define gaim_presence_set_status_active  purple_presence_set_status_active
#define gaim_presence_switch_status      purple_presence_switch_status
#define gaim_presence_set_idle           purple_presence_set_idle
#define gaim_presence_set_login_time     purple_presence_set_login_time
#define gaim_presence_get_context        purple_presence_get_context
#define gaim_presence_get_account        purple_presence_get_account
#define gaim_presence_get_conversation   purple_presence_get_conversation
#define gaim_presence_get_chat_user      purple_presence_get_chat_user
#define gaim_presence_get_statuses       purple_presence_get_statuses
#define gaim_presence_get_status         purple_presence_get_status
#define gaim_presence_get_active_status  purple_presence_get_active_status
#define gaim_presence_is_available       purple_presence_is_available
#define gaim_presence_is_online          purple_presence_is_online
#define gaim_presence_is_status_active   purple_presence_is_status_active
#define gaim_presence_is_status_primitive_active \
	purple_presence_is_status_primitive_active
#define gaim_presence_is_idle            purple_presence_is_idle
#define gaim_presence_get_idle_time      purple_presence_get_idle_time
#define gaim_presence_get_login_time     purple_presence_get_login_time
#define gaim_presence_compare            purple_presence_compare

#define gaim_status_get_handle  purple_status_get_handle
#define gaim_status_init        purple_status_init
#define gaim_status_uninit      purple_status_uninit

/* from stringref.h */

#define GaimStringref  PurpleStringref

#define gaim_stringref_new        purple_stringref_new
#define gaim_stringref_new_noref  purple_stringref_new_noref
#define gaim_stringref_printf     purple_stringref_printf
#define gaim_stringref_ref        purple_stringref_ref
#define gaim_stringref_unref      purple_stringref_unref
#define gaim_stringref_value      purple_stringref_value
#define gaim_stringref_cmp        purple_stringref_cmp
#define gaim_stringref_len        purple_stringref_len

/* from stun.h */

#define GaimStunNatDiscovery  PurpleStunNatDiscovery

#define GAIM_STUN_STATUS_UNDISCOVERED  PURPLE_STUN_STATUS_UNDISCOVERED
#define GAIM_STUN_STATUS_UNKNOWN       PURPLE_STUN_STATUS_UNKNOWN
#define GAIM_STUN_STATUS_DISCOVERING   PURPLE_STUN_STATUS_DISCOVERING
#define GAIM_STUN_STATUS_DISCOVERED    PURPLE_STUN_STATUS_DISCOVERED
#define GaimStunStatus  PurpleStunStatus

#define GAIM_STUN_NAT_TYPE_PUBLIC_IP             PURPLE_STUN_NAT_TYPE_PUBLIC_IP
#define GAIM_STUN_NAT_TYPE_UNKNOWN_NAT           PURPLE_STUN_NAT_TYPE_UNKNOWN_NAT
#define GAIM_STUN_NAT_TYPE_FULL_CONE             PURPLE_STUN_NAT_TYPE_FULL_CONE
#define GAIM_STUN_NAT_TYPE_RESTRICTED_CONE       PURPLE_STUN_NAT_TYPE_RESTRICTED_CONE
#define GAIM_STUN_NAT_TYPE_PORT_RESTRICTED_CONE  PURPLE_STUN_NAT_TYPE_PORT_RESTRICTED_CONE
#define GAIM_STUN_NAT_TYPE_SYMMETRIC             PURPLE_STUN_NAT_TYPE_SYMMETRIC
#define GaimStunNatType  PurpleStunNatType

/* why didn't this have a Gaim prefix before? */
#define StunCallback  PurpleStunCallback

#define gaim_stun_discover  purple_stun_discover
#define gaim_stun_init      purple_stun_init

/* from upnp.h */

/* suggested rename: PurpleUPnpMappingHandle */
#define UPnPMappingAddRemove  PurpleUPnPMappingAddRemove

#define GaimUPnPCallback  PurpleUPnPCallback

#define gaim_upnp_discover             purple_upnp_discover
#define gaim_upnp_get_public_ip        purple_upnp_get_public_ip
#define gaim_upnp_cancel_port_mapping  purple_upnp_cancel_port_mapping
#define gaim_upnp_set_port_mapping     purple_upnp_set_port_mapping

#define gaim_upnp_remove_port_mapping  purple_upnp_remove_port_mapping

/* from util.h */

#define GaimUtilFetchUrlData  PurpleUtilFetchUrlData
#define GaimMenuAction        PurpleMenuAction

#define GaimInfoFieldFormatCallback  PurpleIntoFieldFormatCallback

#define GaimKeyValuePair  PurpleKeyValuePair

#define gaim_menu_action_new   purple_menu_action_new
#define gaim_menu_action_free  purple_menu_action_free

#define gaim_base16_encode   purple_base16_encode
#define gaim_base16_decode   purple_base16_decode
#define gaim_base64_encode   purple_base64_encode
#define gaim_base64_decode   purple_base64_decode
#define gaim_quotedp_decode  purple_quotedp_decode

#define gaim_mime_decode_field  purple_mime_deco_field

#define gaim_utf8_strftime      purple_utf8_strftime
#define gaim_date_format_short  purple_date_format_short
#define gaim_date_format_long   purple_date_format_long
#define gaim_date_format_full   purple_date_format_full
#define gaim_time_format        purple_time_format
#define gaim_time_build         purple_time_build

#define GAIM_NO_TZ_OFF  PURPLE_NO_TZ_OFF

#define gaim_str_to_time  purple_str_to_time

#define gaim_markup_find_tag            purple_markup_find_tag
#define gaim_markup_extract_info_field  purple_markup_extract_info_field
#define gaim_markup_html_to_xhtml       purple_markup_html_to_xhtml
#define gaim_markup_strip_html          purple_markup_strip_html
#define gaim_markup_linkify             purple_markup_linkify
#define gaim_markup_slice               purple_markup_slice
#define gaim_markup_get_tag_name        purple_markup_get_tag_name
#define gaim_unescape_html              purple_unescape_html

#define gaim_home_dir  purple_home_dir
#define gaim_user_dir  purple_user_dir

#define gaim_util_set_user_dir  purple_util_set_user_dir

#define gaim_build_dir  purple_build_dir

#define gaim_util_write_data_to_file  purple_util_write_data_to_file

#define gaim_util_read_xml_from_file  purple_util_read_xml_from_file

#define gaim_mkstemp  purple_mkstemp

#define gaim_program_is_valid  purple_program_is_valid

#define gaim_running_gnome  purple_running_gnome
#define gaim_running_kde    purple_running_kde
#define gaim_running_osx    purple_running_osx

#define gaim_fd_get_ip  purple_fd_get_ip

#define gaim_normalize         purple_normalize
#define gaim_normalize_nocase  purple_normalize_nocase

#define gaim_strdup_withhtml  purple_strdup_withhtml

#define gaim_str_has_prefix  purple_str_has_prefix
#define gaim_str_has_suffix  purple_str_has_suffix
#define gaim_str_add_cr      purple_str_add_cr
#define gaim_str_strip_char  purple_str_strip_char

#define gaim_util_chrreplace  purple_util_chrreplace

#define gaim_strreplace  purple_strreplace

#define gaim_utf8_ncr_encode  purple_utf8_ncr_encode
#define gaim_utf8_ncr_decode  purple_utf8_ncr_decode

#define gaim_strcasereplace  purple_strcasereplace
#define gaim_strcasestr      purple_strcasestr

#define gaim_str_size_to_units      purple_str_size_to_units
#define gaim_str_seconds_to_string  purple_str_seconds_to_string
#define gaim_str_binary_to_ascii    purple_str_binary_to_ascii


#define gaim_got_protocol_handler_uri  purple_got_protocol_handler_uri

#define gaim_url_parse  purple_url_parse

#define GaimUtilFetchUrlCallback  PurpleUtilFetchUrlCallback
#define gaim_util_fetch_url          purple_util_fetch_url
#define gaim_util_fetch_url_request  purple_util_fetch_url_request
#define gaim_util_fetch_url_cancel   purple_util_fetch_url_cancel

#define gaim_url_decode  purple_url_decode
#define gaim_url_encode  purple_url_encode

#define gaim_email_is_valid  purple_email_is_valid

#define gaim_uri_list_extract_uris       purple_uri_list_extract_uris
#define gaim_uri_list_extract_filenames  purple_uri_list_extract_filenames

#define gaim_utf8_try_convert  purple_utf8_try_convert
#define gaim_utf8_salvage      purple_utf8_salvage
#define gaim_utf8_strcasecmp   purple_utf8_strcasecmp
#define gaim_utf8_has_word     purple_utf8_has_word

#define gaim_print_utf8_to_console  purple_print_utf8_to_console

#define gaim_message_meify  purple_message_meify

#define gaim_text_strip_mnemonic  purple_text_strip_mnemonic

#define gaim_unescape_filename  purple_unescape_filename
#define gaim_escape_filename    purple_escape_filename

/* from value.h */

#define GAIM_TYPE_UNKNOWN  PURPLE_TYPE_UNKNOWN
#define GAIM_TYPE_SUBTYPE  PURPLE_TYPE_SUBTYPE
#define GAIM_TYPE_CHAR     PURPLE_TYPE_CHAR
#define GAIM_TYPE_UCHAR    PURPLE_TYPE_UCHAR
#define GAIM_TYPE_BOOLEAN  PURPLE_TYPE_BOOLEAN
#define GAIM_TYPE_SHORT    PURPLE_TYPE_SHORT
#define GAIM_TYPE_USHORT   PURPLE_TYPE_USHORT
#define GAIM_TYPE_INT      PURPLE_TYPE_INT
#define GAIM_TYPE_UINT     PURPLE_TYPE_UINT
#define GAIM_TYPE_LONG     PURPLE_TYPE_LONG
#define GAIM_TYPE_ULONG    PURPLE_TYPE_ULONG
#define GAIM_TYPE_INT64    PURPLE_TYPE_INT64
#define GAIM_TYPE_UINT64   PURPLE_TYPE_UINT64
#define GAIM_TYPE_STRING   PURPLE_TYPE_STRING
#define GAIM_TYPE_OBJECT   PURPLE_TYPE_OBJECT
#define GAIM_TYPE_POINTER  PURPLE_TYPE_POINTER
#define GAIM_TYPE_ENUM     PURPLE_TYPE_ENUM
#define GAIM_TYPE_BOXED    PURPLE_TYPE_BOXED
#define GaimType  PurpleType


#define GAIM_SUBTYPE_UNKNOWN       PURPLE_SUBTYPE_UNKNOWN
#define GAIM_SUBTYPE_ACCOUNT       PURPLE_SUBTYPE_ACCOUNT
#define GAIM_SUBTYPE_BLIST         PURPLE_SUBTYPE_BLIST
#define GAIM_SUBTYPE_BLIST_BUDDY   PURPLE_SUBTYPE_BLIST_BUDDY
#define GAIM_SUBTYPE_BLIST_GROUP   PURPLE_SUBTYPE_BLIST_GROUP
#define GAIM_SUBTYPE_BLIST_CHAT    PURPLE_SUBTYPE_BLIST_CHAT
#define GAIM_SUBTYPE_BUDDY_ICON    PURPLE_SUBTYPE_BUDDY_ICON
#define GAIM_SUBTYPE_CONNECTION    PURPLE_SUBTYPE_CONNECTION
#define GAIM_SUBTYPE_CONVERSATION  PURPLE_SUBTYPE_CONVERSATION
#define GAIM_SUBTYPE_PLUGIN        PURPLE_SUBTYPE_PLUGIN
#define GAIM_SUBTYPE_BLIST_NODE    PURPLE_SUBTYPE_BLIST_NODE
#define GAIM_SUBTYPE_CIPHER        PURPLE_SUBTYPE_CIPHER
#define GAIM_SUBTYPE_STATUS        PURPLE_SUBTYPE_STATUS
#define GAIM_SUBTYPE_LOG           PURPLE_SUBTYPE_LOG
#define GAIM_SUBTYPE_XFER          PURPLE_SUBTYPE_XFER
#define GAIM_SUBTYPE_SAVEDSTATUS   PURPLE_SUBTYPE_SAVEDSTATUS
#define GAIM_SUBTYPE_XMLNODE       PURPLE_SUBTYPE_XMLNODE
#define GAIM_SUBTYPE_USERINFO      PURPLE_SUBTYPE_USERINFO
#define GaimSubType  PurpleSubType

#define GaimValue  PurpleValue

#define gaim_value_new                purple_value_new
#define gaim_value_new_outgoing       purple_value_new_outgoing
#define gaim_value_destroy            purple_value_destroy
#define gaim_value_dup                purple_value_dup
#define gaim_value_purple_buddy_icon_get_extensionget_type           purple_value_get_type
#define gaim_value_get_subtype        purple_value_get_subtype
#define gaim_value_get_specific_type  purple_value_get_specific_type
#define gaim_value_is_outgoing        purple_value_is_outgoing
#define gaim_value_set_char           purple_value_set_char
#define gaim_value_set_uchar          purple_value_set_uchar
#define gaim_value_set_boolean        purple_value_set_boolean
#define gaim_value_set_short          purple_value_set_short
#define gaim_value_set_ushort         purple_value_set_ushort
#define gaim_value_set_int            purple_value_set_int
#define gaim_value_set_uint           purple_value_set_uint
#define gaim_value_set_long           purple_value_set_long
#define gaim_value_set_ulong          purple_value_set_ulong
#define gaim_value_set_int64          purple_value_set_int64
#define gaim_value_set_uint64         purple_value_set_uint64
#define gaim_value_set_string         purple_value_set_string
#define gaim_value_set_object         purple_value_set_object
#define gaim_value_set_pointer        purple_value_set_pointer
#define gaim_value_set_enum           purple_value_set_enum
#define gaim_value_set_boxed          purple_value_set_boxed
#define gaim_value_get_char           purple_value_get_char
#define gaim_value_get_uchar          purple_value_get_uchar
#define gaim_value_get_boolean        purple_value_get_boolean
#define gaim_value_get_short          purple_value_get_short
#define gaim_value_get_ushort         purple_value_get_ushort
#define gaim_value_get_int            purple_value_get_int
#define gaim_value_get_uint           purple_value_get_uint
#define gaim_value_get_long           purple_value_get_long
#define gaim_value_get_ulong          purple_value_get_ulong
#define gaim_value_get_int64          purple_value_get_int64
#define gaim_value_get_uint64         purple_value_get_uint64
#define gaim_value_get_string         purple_value_get_string
#define gaim_value_get_object         purple_value_get_object
#define gaim_value_get_pointer        purple_value_get_pointer
#define gaim_value_get_enum           purple_value_get_enum
#define gaim_value_get_boxed          purple_value_get_boxed

/* from version.h */

#define GAIM_MAJOR_VERSION  PURPLE_MAJOR_VERSION
#define GAIM_MINOR_VERSION  PURPLE_MINOR_VERSION
#define GAIM_MICRO_VERSION  PURPLE_MICRO_VERSION

#define GAIM_VERSION_CHECK  PURPLE_VERSION_CHECK

/* from whiteboard.h */

#define GaimWhiteboardPrplOps  PurpleWhiteboardPrplOps
#define GaimWhiteboard         PurpleWhiteboard
#define GaimWhiteboardUiOps    PurpleWhiteboardUiOps

#define gaim_whiteboard_set_ui_ops    purple_whiteboard_set_ui_ops
#define gaim_whiteboard_set_prpl_ops  purple_whiteboard_set_prpl_ops

#define gaim_whiteboard_create             purple_whiteboard_create
#define gaim_whiteboard_destroy            purple_whiteboard_destroy
#define gaim_whiteboard_start              purple_whiteboard_start
#define gaim_whiteboard_get_session        purple_whiteboard_get_session
#define gaim_whiteboard_draw_list_destroy  purple_whiteboard_draw_list_destroy
#define gaim_whiteboard_get_dimensions     purple_whiteboard_get_dimensions
#define gaim_whiteboard_set_dimensions     purple_whiteboard_set_dimensions
#define gaim_whiteboard_draw_point         purple_whiteboard_draw_point
#define gaim_whiteboard_send_draw_list     purple_whiteboard_send_draw_list
#define gaim_whiteboard_draw_line          purple_whiteboard_draw_line
#define gaim_whiteboard_clear              purple_whiteboard_clear
#define gaim_whiteboard_send_clear         purple_whiteboard_send_clear
#define gaim_whiteboard_send_brush         purple_whiteboard_send_brush
#define gaim_whiteboard_get_brush          purple_whiteboard_get_brush
#define gaim_whiteboard_set_brush          purple_whiteboard_set_brush

/* for static plugins */
#define gaim_init_ssl_plugin			purple_init_ssl_plugin
#define gaim_init_ssl_openssl_plugin	purple_init_ssl_openssl_plugin
#define gaim_init_ssl_gnutls_plugin		purple_init_ssl_gnutls_plugin
#define gaim_init_gg_plugin				purple_init_gg_plugin
#define gaim_init_jabber_plugin			purple_init_jabber_plugin
#define gaim_init_sametime_plugin		purple_init_sametime_plugin
#define gaim_init_msn_plugin			purple_init_msn_plugin
#define gaim_init_novell_plugin			purple_init_novell_plugin
#define gaim_init_qq_plugin				purple_init_qq_plugin
#define gaim_init_simple_plugin			purple_init_simple_plugin
#define gaim_init_yahoo_plugin			purple_init_yahoo_plugin
#define gaim_init_zephyr_plugin			purple_init_zephyr_plugin
#define gaim_init_aim_plugin			purple_init_aim_plugin
#define gaim_init_icq_plugin			purple_init_icq_plugin

#endif /* _GAIM_COMPAT_H_ */
