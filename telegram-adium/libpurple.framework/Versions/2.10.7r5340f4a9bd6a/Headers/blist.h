/**
 * @file blist.h Buddy List API
 * @ingroup core
 * @see @ref blist-signals
 */

/* purple
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
#ifndef _PURPLE_BLIST_H_
#define _PURPLE_BLIST_H_

/* I can't believe I let ChipX86 inspire me to write good code. -Sean */

#include <glib.h>

/** @copydoc _PurpleBuddyList */
typedef struct _PurpleBuddyList PurpleBuddyList;
/** @copydoc _PurpleBlistUiOps */
typedef struct _PurpleBlistUiOps PurpleBlistUiOps;
/** @copydoc _PurpleBlistNode */
typedef struct _PurpleBlistNode PurpleBlistNode;

/** @copydoc _PurpleChat */
typedef struct _PurpleChat PurpleChat;
/** @copydoc _PurpleGroup */
typedef struct _PurpleGroup PurpleGroup;
/** @copydoc _PurpleContact */
typedef struct _PurpleContact PurpleContact;
/** @copydoc _PurpleBuddy */
typedef struct _PurpleBuddy PurpleBuddy;

/**************************************************************************/
/* Enumerations                                                           */
/**************************************************************************/
typedef enum
{
	PURPLE_BLIST_GROUP_NODE,
	PURPLE_BLIST_CONTACT_NODE,
	PURPLE_BLIST_BUDDY_NODE,
	PURPLE_BLIST_CHAT_NODE,
	PURPLE_BLIST_OTHER_NODE

} PurpleBlistNodeType;

#define PURPLE_BLIST_NODE_IS_CHAT(n)    (purple_blist_node_get_type(n) == PURPLE_BLIST_CHAT_NODE)
#define PURPLE_BLIST_NODE_IS_BUDDY(n)   (purple_blist_node_get_type(n) == PURPLE_BLIST_BUDDY_NODE)
#define PURPLE_BLIST_NODE_IS_CONTACT(n) (purple_blist_node_get_type(n) == PURPLE_BLIST_CONTACT_NODE)
#define PURPLE_BLIST_NODE_IS_GROUP(n)   (purple_blist_node_get_type(n) == PURPLE_BLIST_GROUP_NODE)

#define PURPLE_BUDDY_IS_ONLINE(b) \
	((b) != NULL && purple_account_is_connected(purple_buddy_get_account(b)) && \
	 purple_presence_is_online(purple_buddy_get_presence(b)))

typedef enum
{
	PURPLE_BLIST_NODE_FLAG_NO_SAVE      = 1 << 0 /**< node should not be saved with the buddy list */

} PurpleBlistNodeFlags;

/**
 * @since 2.6.0
 */
#define PURPLE_BLIST_NODE(obj) ((PurpleBlistNode *)(obj))

#define PURPLE_BLIST_NODE_HAS_FLAG(b, f) (purple_blist_node_get_flags((PurpleBlistNode*)(b)) & (f))
#define PURPLE_BLIST_NODE_SHOULD_SAVE(b) (! PURPLE_BLIST_NODE_HAS_FLAG(b, PURPLE_BLIST_NODE_FLAG_NO_SAVE))

#define PURPLE_BLIST_NODE_NAME(n) (purple_blist_node_get_type(n) == PURPLE_BLIST_CHAT_NODE  ? purple_chat_get_name((PurpleChat*)n) :        \
				     purple_blist_node_get_type(n) == PURPLE_BLIST_BUDDY_NODE ? purple_buddy_get_name((PurpleBuddy*)n) : NULL)

/**
 * @since 2.6.0
 */
#define PURPLE_GROUP(obj) ((PurpleGroup *)(obj))

/**
 * @since 2.6.0
 */
#define PURPLE_CONTACT(obj) ((PurpleContact *)(obj))

/**
 * @since 2.6.0
 */
#define PURPLE_BUDDY(obj) ((PurpleBuddy *)(obj))

/**
 * @since 2.6.0
 */
#define PURPLE_CHAT(obj) ((PurpleChat *)(obj))

#include "account.h"
#include "buddyicon.h"
#include "media.h"
#include "status.h"

/**************************************************************************/
/* Data Structures                                                        */
/**************************************************************************/

#if !(defined PURPLE_HIDE_STRUCTS) || (defined _PURPLE_BLIST_C_)

/**
 * A Buddy list node.  This can represent a group, a buddy, or anything else.
 * This is a base class for PurpleBuddy, PurpleContact, PurpleGroup, and for
 * anything else that wants to put itself in the buddy list. */
struct _PurpleBlistNode {
	PurpleBlistNodeType type;             /**< The type of node this is       */
	PurpleBlistNode *prev;                /**< The sibling before this buddy. */
	PurpleBlistNode *next;                /**< The sibling after this buddy.  */
	PurpleBlistNode *parent;              /**< The parent of this node        */
	PurpleBlistNode *child;               /**< The child of this node         */
	GHashTable *settings;               /**< per-node settings              */
	void          *ui_data;             /**< The UI can put data here.      */
	PurpleBlistNodeFlags flags;           /**< The buddy flags                */
};

/**
 * A buddy.  This contains everything Purple will ever need to know about someone on the buddy list.  Everything.
 */
struct _PurpleBuddy {
	PurpleBlistNode node;                     /**< The node that this buddy inherits from */
	char *name;                             /**< The name of the buddy. */
	char *alias;                            /**< The user-set alias of the buddy */
	char *server_alias;                     /**< The server-specified alias of the buddy.  (i.e. MSN "Friendly Names") */
	void *proto_data;                       /**< This allows the prpl to associate whatever data it wants with a buddy */
	PurpleBuddyIcon *icon;                    /**< The buddy icon. */
	PurpleAccount *account;					/**< the account this buddy belongs to */
	PurplePresence *presence;
	PurpleMediaCaps media_caps;		/**< The media capabilities of the buddy. */
};

/**
 * A contact.  This contains everything Purple will ever need to know about a contact.
 */
struct _PurpleContact {
	PurpleBlistNode node;		/**< The node that this contact inherits from. */
	char *alias;            /**< The user-set alias of the contact */
	int totalsize;		    /**< The number of buddies in this contact */
	int currentsize;	    /**< The number of buddies in this contact corresponding to online accounts */
	int online;			    /**< The number of buddies in this contact who are currently online */
	PurpleBuddy *priority;    /**< The "top" buddy for this contact */
	gboolean priority_valid; /**< Is priority valid? */
};


/**
 * A group.  This contains everything Purple will ever need to know about a group.
 */
struct _PurpleGroup {
	PurpleBlistNode node;                    /**< The node that this group inherits from */
	char *name;                            /**< The name of this group. */
	int totalsize;			       /**< The number of chats and contacts in this group */
	int currentsize;		       /**< The number of chats and contacts in this group corresponding to online accounts */
	int online;			       /**< The number of chats and contacts in this group who are currently online */
};

/**
 * A chat.  This contains everything Purple needs to put a chat room in the
 * buddy list.
 */
struct _PurpleChat {
	PurpleBlistNode node;      /**< The node that this chat inherits from */
	char *alias;             /**< The display name of this chat. */
	GHashTable *components;  /**< the stuff the protocol needs to know to join the chat */
	PurpleAccount *account; /**< The account this chat is attached to */
};

/**
 * The Buddy List
 */
struct _PurpleBuddyList {
	PurpleBlistNode *root;          /**< The first node in the buddy list */
	GHashTable *buddies;          /**< Every buddy in this list */
	void *ui_data;                /**< UI-specific data. */
};

#endif /* PURPLE_HIDE_STRUCTS && PURPLE_BLIST_STRUCTS */

/**
 * Buddy list UI operations.
 *
 * Any UI representing a buddy list must assign a filled-out PurpleBlistUiOps
 * structure to the buddy list core.
 */
struct _PurpleBlistUiOps
{
	void (*new_list)(PurpleBuddyList *list); /**< Sets UI-specific data on a buddy list. */
	void (*new_node)(PurpleBlistNode *node); /**< Sets UI-specific data on a node. */
	void (*show)(PurpleBuddyList *list);     /**< The core will call this when it's finished doing its core stuff */
	void (*update)(PurpleBuddyList *list,
		       PurpleBlistNode *node);       /**< This will update a node in the buddy list. */
	void (*remove)(PurpleBuddyList *list,
		       PurpleBlistNode *node);       /**< This removes a node from the list */
	void (*destroy)(PurpleBuddyList *list);  /**< When the list is destroyed, this is called to destroy the UI. */
	void (*set_visible)(PurpleBuddyList *list,
			    gboolean show);            /**< Hides or unhides the buddy list */
	void (*request_add_buddy)(PurpleAccount *account, const char *username,
							  const char *group, const char *alias);
	void (*request_add_chat)(PurpleAccount *account, PurpleGroup *group,
							 const char *alias, const char *name);
	void (*request_add_group)(void);

	/**
	 * This is called when a node has been modified and should be saved.
	 *
	 * Implementation of this UI op is OPTIONAL. If not implemented, it will
	 * be set to a fallback function that saves data to blist.xml like in
	 * previous libpurple versions.
	 *
	 * @param node    The node which has been modified.
	 *
	 * @since 2.6.0.
	 */
	void (*save_node)(PurpleBlistNode *node);

	/**
	 * Called when a node is about to be removed from the buddy list.
	 * The UI op should update the relevant data structures to remove this
	 * node (for example, removing a buddy from the group this node is in).
	 *
	 * Implementation of this UI op is OPTIONAL. If not implemented, it will
	 * be set to a fallback function that saves data to blist.xml like in
	 * previous libpurple versions.
	 *
	 * @param node  The node which has been modified.
	 * @since 2.6.0.
	 */
	void (*remove_node)(PurpleBlistNode *node);

	/**
	 * Called to save all the data for an account. If the UI sets this,
	 * the callback must save the privacy and buddy list data for an account.
	 * If the account is NULL, save the data for all accounts.
	 *
	 * Implementation of this UI op is OPTIONAL. If not implemented, it will
	 * be set to a fallback function that saves data to blist.xml like in
	 * previous libpurple versions.
	 *
	 * @param account  The account whose data to save. If NULL, save all data
	 *                  for all accounts.
	 * @since 2.6.0.
	 */
	void (*save_account)(PurpleAccount *account);

	void (*_purple_reserved1)(void);
};

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Buddy List API                                                  */
/**************************************************************************/
/*@{*/

/**
 * Creates a new buddy list
 *
 * @return The new buddy list.
 * @deprecated In 3.0.0, this will be handled by purple_blist_init()
 */
PurpleBuddyList *purple_blist_new(void);

/**
 * Sets the main buddy list.
 *
 * @param blist The buddy list you want to use.
 * @deprecated In 3.0.0, this will be handled by purple_blist_init()
 */
void purple_set_blist(PurpleBuddyList *blist);

/**
 * Returns the main buddy list.
 *
 * @return The main buddy list.
 */
PurpleBuddyList *purple_get_blist(void);

/**
 * Returns the root node of the main buddy list.
 *
 * @return The root node.
 */
PurpleBlistNode *purple_blist_get_root(void);

/**
 * Returns a list of every buddy in the list.  Use of this function is
 * discouraged if you do not actually need every buddy in the list.  Use
 * purple_find_buddies instead.
 *
 * @return A list of every buddy in the list. Caller is responsible for
 *         freeing the list.
 *
 * @see purple_find_buddies
 * @since 2.6.0
 */
GSList *purple_blist_get_buddies(void);

/**
 * Returns the UI data for the list.
 *
 * @return The UI data for the list.
 *
 * @since 2.6.0
 */
gpointer purple_blist_get_ui_data(void);

/**
 * Sets the UI data for the list.
 *
 * @param ui_data The UI data for the list.
 *
 * @since 2.6.0
 */
void purple_blist_set_ui_data(gpointer ui_data);

/**
 * Returns the next node of a given node. This function is to be used to iterate
 * over the tree returned by purple_get_blist.
 *
 * @param node		A node.
 * @param offline	Whether to include nodes for offline accounts
 * @return	The next node
 * @see purple_blist_node_get_parent
 * @see purple_blist_node_get_first_child
 * @see purple_blist_node_get_sibling_next
 * @see purple_blist_node_get_sibling_prev
 */
PurpleBlistNode *purple_blist_node_next(PurpleBlistNode *node, gboolean offline);

/**
 * Returns the parent node of a given node.
 *
 * @param node A node.
 * @return  The parent node.
 * @since 2.4.0
 * @see purple_blist_node_get_first_child
 * @see purple_blist_node_get_sibling_next
 * @see purple_blist_node_get_sibling_prev
 * @see purple_blist_node_next
 */
PurpleBlistNode *purple_blist_node_get_parent(PurpleBlistNode *node);

/**
 * Returns the the first child node of a given node.
 *
 * @param node A node.
 * @return  The child node.
 * @since 2.4.0
 * @see purple_blist_node_get_parent
 * @see purple_blist_node_get_sibling_next
 * @see purple_blist_node_get_sibling_prev
 * @see purple_blist_node_next
 */
PurpleBlistNode *purple_blist_node_get_first_child(PurpleBlistNode *node);

/**
 * Returns the sibling node of a given node.
 *
 * @param node A node.
 * @return  The sibling node.
 * @since 2.4.0
 * @see purple_blist_node_get_parent
 * @see purple_blist_node_get_first_child
 * @see purple_blist_node_get_sibling_prev
 * @see purple_blist_node_next
 */
PurpleBlistNode *purple_blist_node_get_sibling_next(PurpleBlistNode *node);

/**
 * Returns the previous sibling node of a given node.
 *
 * @param node A node.
 * @return  The sibling node.
 * @since 2.4.0
 * @see purple_blist_node_get_parent
 * @see purple_blist_node_get_first_child
 * @see purple_blist_node_get_sibling_next
 * @see purple_blist_node_next
 */
PurpleBlistNode *purple_blist_node_get_sibling_prev(PurpleBlistNode *node);

/**
 * Returns the UI data of a given node.
 *
 * @param node The node.
 * @return The UI data.
 * @since 2.6.0
 */
gpointer purple_blist_node_get_ui_data(const PurpleBlistNode *node);

/**
 * Sets the UI data of a given node.
 *
 * @param node The node.
 * @param ui_data The UI data.
 *
 * @since 2.6.0
 */
void purple_blist_node_set_ui_data(PurpleBlistNode *node, gpointer ui_data);

/**
 * Shows the buddy list, creating a new one if necessary.
 */
void purple_blist_show(void);


/**
 * Destroys the buddy list window.
 *
 * @deprecated The UI is responsible for cleaning up the
 *             PurpleBuddyList->ui_data. purple_blist_uninit() will free the
 *             PurpleBuddyList* itself.
 */
void purple_blist_destroy(void);

/**
 * Hides or unhides the buddy list.
 *
 * @param show   Whether or not to show the buddy list
 */
void purple_blist_set_visible(gboolean show);

/**
 * Updates a buddy's status.
 *
 * This should only be called from within Purple.
 *
 * @param buddy      The buddy whose status has changed.
 * @param old_status The status from which we are changing.
 */
void purple_blist_update_buddy_status(PurpleBuddy *buddy, PurpleStatus *old_status);

/**
 * Updates a node's custom icon.
 *
 * @param node  The PurpleBlistNode whose custom icon has changed.
 *
 * @since 2.5.0
 */
void purple_blist_update_node_icon(PurpleBlistNode *node);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_BLIST_C_)
/**
 * Updates a buddy's icon.
 *
 * @param buddy  The buddy whose buddy icon has changed
 * @deprecated Use purple_blist_update_node_icon() instead.
 */
void purple_blist_update_buddy_icon(PurpleBuddy *buddy);
#endif

/**
 * Renames a buddy in the buddy list.
 *
 * @param buddy  The buddy whose name will be changed.
 * @param name   The new name of the buddy.
 */
void purple_blist_rename_buddy(PurpleBuddy *buddy, const char *name);

/**
 * Aliases a contact in the buddy list.
 *
 * @param contact The contact whose alias will be changed.
 * @param alias   The contact's alias.
 */
void purple_blist_alias_contact(PurpleContact *contact, const char *alias);

/**
 * Aliases a buddy in the buddy list.
 *
 * @param buddy  The buddy whose alias will be changed.
 * @param alias  The buddy's alias.
 */
void purple_blist_alias_buddy(PurpleBuddy *buddy, const char *alias);

/**
 * Sets the server-sent alias of a buddy in the buddy list.
 * PRPLs should call serv_got_alias() instead of this.
 *
 * @param buddy  The buddy whose alias will be changed.
 * @param alias  The buddy's "official" alias.
 */
void purple_blist_server_alias_buddy(PurpleBuddy *buddy, const char *alias);

/**
 * Aliases a chat in the buddy list.
 *
 * @param chat  The chat whose alias will be changed.
 * @param alias The chat's new alias.
 */
void purple_blist_alias_chat(PurpleChat *chat, const char *alias);

/**
 * Renames a group
 *
 * @param group  The group to rename
 * @param name   The new name
 */
void purple_blist_rename_group(PurpleGroup *group, const char *name);

/**
 * Creates a new chat for the buddy list
 *
 * @param account    The account this chat will get added to
 * @param alias      The alias of the new chat
 * @param components The info the prpl needs to join the chat.  The
 *                   hash function should be g_str_hash() and the
 *                   equal function should be g_str_equal().
 * @return           A newly allocated chat
 */
PurpleChat *purple_chat_new(PurpleAccount *account, const char *alias, GHashTable *components);

/**
 * Destroys a chat
 *
 * @param chat       The chat to destroy
 */
void purple_chat_destroy(PurpleChat *chat);

/**
 * Adds a new chat to the buddy list.
 *
 * The chat will be inserted right after node or appended to the end
 * of group if node is NULL.  If both are NULL, the buddy will be added to
 * the "Chats" group.
 *
 * @param chat  The new chat who gets added
 * @param group  The group to add the new chat to.
 * @param node   The insertion point
 */
void purple_blist_add_chat(PurpleChat *chat, PurpleGroup *group, PurpleBlistNode *node);

/**
 * Creates a new buddy.
 *
 * This function only creates the PurpleBuddy. Use purple_blist_add_buddy
 * to add the buddy to the list and purple_account_add_buddy to sync up
 * with the server.
 *
 * @param account    The account this buddy will get added to
 * @param name       The name of the new buddy
 * @param alias      The alias of the new buddy (or NULL if unaliased)
 * @return           A newly allocated buddy
 *
 * @see purple_account_add_buddy
 * @see purple_blist_add_buddy
 */
PurpleBuddy *purple_buddy_new(PurpleAccount *account, const char *name, const char *alias);

/**
 * Destroys a buddy
 *
 * @param buddy     The buddy to destroy
 */
void purple_buddy_destroy(PurpleBuddy *buddy);

/**
 * Sets a buddy's icon.
 *
 * This should only be called from within Purple. You probably want to
 * call purple_buddy_icon_set_data().
 *
 * @param buddy The buddy.
 * @param icon  The buddy icon.
 *
 * @see purple_buddy_icon_set_data()
 */
void purple_buddy_set_icon(PurpleBuddy *buddy, PurpleBuddyIcon *icon);

/**
 * Returns a buddy's account.
 *
 * @param buddy The buddy.
 *
 * @return The account
 */
PurpleAccount *purple_buddy_get_account(const PurpleBuddy *buddy);

/**
 * Returns a buddy's name
 *
 * @param buddy The buddy.
 *
 * @return The name.
 */
const char *purple_buddy_get_name(const PurpleBuddy *buddy);

/**
 * Returns a buddy's icon.
 *
 * @param buddy The buddy.
 *
 * @return The buddy icon.
 */
PurpleBuddyIcon *purple_buddy_get_icon(const PurpleBuddy *buddy);

/**
 * Returns a buddy's protocol-specific data.
 *
 * This should only be called from the associated prpl.
 *
 * @param buddy The buddy.
 * @return      The protocol data.
 *
 * @see purple_buddy_set_protocol_data()
 * @since 2.6.0
 */
gpointer purple_buddy_get_protocol_data(const PurpleBuddy *buddy);

/**
 * Sets a buddy's protocol-specific data.
 *
 * This should only be called from the associated prpl.
 *
 * @param buddy The buddy.
 * @param data  The data.
 *
 * @see purple_buddy_get_protocol_data()
 * @since 2.6.0
 */
void purple_buddy_set_protocol_data(PurpleBuddy *buddy, gpointer data);

/**
 * Returns a buddy's contact.
 *
 * @param buddy The buddy.
 *
 * @return The buddy's contact.
 */
PurpleContact *purple_buddy_get_contact(PurpleBuddy *buddy);

/**
 * Returns a buddy's presence.
 *
 * @param buddy The buddy.
 *
 * @return The buddy's presence.
 */
PurplePresence *purple_buddy_get_presence(const PurpleBuddy *buddy);

/**
 * Gets the media caps from a buddy.
 *
 * @param buddy The buddy.
 * @return      The media caps.
 *
 * @since 2.7.0
 */
PurpleMediaCaps purple_buddy_get_media_caps(const PurpleBuddy *buddy);

/**
 * Sets the media caps for a buddy.
 *
 * @param buddy      The PurpleBuddy.
 * @param media_caps The PurpleMediaCaps.
 */
void purple_buddy_set_media_caps(PurpleBuddy *buddy, PurpleMediaCaps media_caps);

/**
 * Adds a new buddy to the buddy list.
 *
 * The buddy will be inserted right after node or prepended to the
 * group if node is NULL.  If both are NULL, the buddy will be added to
 * the "Buddies" group.
 *
 * @param buddy   The new buddy who gets added
 * @param contact The optional contact to place the buddy in.
 * @param group   The group to add the new buddy to.
 * @param node    The insertion point.  Pass in NULL to add the node as
 *                the first child in the given group.
 */
void purple_blist_add_buddy(PurpleBuddy *buddy, PurpleContact *contact, PurpleGroup *group, PurpleBlistNode *node);

/**
 * Creates a new group
 *
 * You can't have more than one group with the same name.  Sorry.  If you pass
 * this the name of a group that already exists, it will return that group.
 *
 * @param name   The name of the new group
 * @return       A new group struct
*/
PurpleGroup *purple_group_new(const char *name);

/**
 * Destroys a group
 *
 * @param group  The group to destroy
*/
void purple_group_destroy(PurpleGroup *group);

/**
 * Adds a new group to the buddy list.
 *
 * The new group will be inserted after insert or prepended to the list if
 * node is NULL.
 *
 * @param group  The group
 * @param node   The insertion point
 */
void purple_blist_add_group(PurpleGroup *group, PurpleBlistNode *node);

/**
 * Creates a new contact
 *
 * @return       A new contact struct
 */
PurpleContact *purple_contact_new(void);

/**
 * Destroys a contact
 *
 * @param contact  The contact to destroy
 */
void purple_contact_destroy(PurpleContact *contact);

/**
 * Gets the PurpleGroup from a PurpleContact
 *
 * @param contact  The contact
 * @return         The group
 *
 * @since 2.7.0
 */
PurpleGroup *purple_contact_get_group(const PurpleContact *contact);

/**
 * Adds a new contact to the buddy list.
 *
 * The new contact will be inserted after insert or prepended to the list if
 * node is NULL.
 *
 * @param contact The contact
 * @param group   The group to add the contact to
 * @param node    The insertion point
 */
void purple_blist_add_contact(PurpleContact *contact, PurpleGroup *group, PurpleBlistNode *node);

/**
 * Merges two contacts
 *
 * All of the buddies from source will be moved to target
 *
 * @param source  The contact to merge
 * @param node    The place to merge to (a buddy or contact)
 */
void purple_blist_merge_contact(PurpleContact *source, PurpleBlistNode *node);

/**
 * Returns the highest priority buddy for a given contact.
 *
 * @param contact  The contact
 * @return The highest priority buddy
 */
PurpleBuddy *purple_contact_get_priority_buddy(PurpleContact *contact);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_BLIST_C_)
/**
 * Sets the alias for a contact.
 *
 * @param contact  The contact
 * @param alias    The alias to set, or NULL to unset
 *
 * @deprecated Use purple_blist_alias_contact() instead.
 */
void purple_contact_set_alias(PurpleContact *contact, const char *alias);
#endif

/**
 * Gets the alias for a contact.
 *
 * @param contact  The contact
 * @return  The alias, or NULL if it is not set.
 */
const char *purple_contact_get_alias(PurpleContact *contact);

/**
 * Determines whether an account owns any buddies in a given contact
 *
 * @param contact  The contact to search through.
 * @param account  The account.
 *
 * @return TRUE if there are any buddies from account in the contact, or FALSE otherwise.
 */
gboolean purple_contact_on_account(PurpleContact *contact, PurpleAccount *account);

/**
 * Invalidates the priority buddy so that the next call to
 * purple_contact_get_priority_buddy recomputes it.
 *
 * @param contact  The contact
 */
void purple_contact_invalidate_priority_buddy(PurpleContact *contact);

/**
 * Removes a buddy from the buddy list and frees the memory allocated to it.
 * This doesn't actually try to remove the buddy from the server list.
 *
 * @param buddy   The buddy to be removed
 *
 * @see purple_account_remove_buddy
 */
void purple_blist_remove_buddy(PurpleBuddy *buddy);

/**
 * Removes a contact, and any buddies it contains, and frees the memory
 * allocated to it. This calls purple_blist_remove_buddy and therefore
 * doesn't remove the buddies from the server list.
 *
 * @param contact The contact to be removed
 *
 * @see purple_blist_remove_buddy
 */
void purple_blist_remove_contact(PurpleContact *contact);

/**
 * Removes a chat from the buddy list and frees the memory allocated to it.
 *
 * @param chat   The chat to be removed
 */
void purple_blist_remove_chat(PurpleChat *chat);

/**
 * Removes a group from the buddy list and frees the memory allocated to it and to
 * its children
 *
 * @param group   The group to be removed
 */
void purple_blist_remove_group(PurpleGroup *group);

/**
 * Returns the alias of a buddy.
 *
 * @param buddy   The buddy whose name will be returned.
 * @return        The alias (if set), server alias (if set),
 *                or NULL.
 */
const char *purple_buddy_get_alias_only(PurpleBuddy *buddy);

/**
 * Gets the server alias for a buddy.
 *
 * @param buddy  The buddy whose name will be returned
 * @return  The server alias, or NULL if it is not set.
 */
const char *purple_buddy_get_server_alias(PurpleBuddy *buddy);

/**
 * Returns the correct name to display for a buddy, taking the contact alias
 * into account. In order of precedence: the buddy's alias; the buddy's
 * contact alias; the buddy's server alias; the buddy's user name.
 *
 * @param buddy  The buddy whose name will be returned
 * @return       The appropriate name or alias, or NULL.
 *
 */
const char *purple_buddy_get_contact_alias(PurpleBuddy *buddy);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_BLIST_C_)
/**
 * Returns the correct alias for this user, ignoring server aliases.  Used
 * when a user-recognizable name is required.  In order: buddy's alias; buddy's
 * contact alias; buddy's user name.
 *
 * @param buddy  The buddy whose alias will be returned.
 * @return       The appropriate name or alias.
 * @deprecated   Try purple_buddy_get_alias(), if server aliases are okay.
 */
const char *purple_buddy_get_local_alias(PurpleBuddy *buddy);
#endif

/**
 * Returns the correct name to display for a buddy. In order of precedence:
 * the buddy's alias; the buddy's server alias; the buddy's contact alias;
 * the buddy's user name.
 *
 * @param buddy   The buddy whose name will be returned.
 * @return        The appropriate name or alias, or NULL
 */
const char *purple_buddy_get_alias(PurpleBuddy *buddy);

/**
 * Returns the local alias for the buddy, or @c NULL if none exists.
 *
 * @param buddy  The buddy
 * @return       The local alias for the buddy
 *
 * @since 2.6.0
 */
const char *purple_buddy_get_local_buddy_alias(PurpleBuddy *buddy);

/**
 * Returns the correct name to display for a blist chat.
 *
 * @param chat   The chat whose name will be returned.
 * @return       The alias (if set), or first component value.
 */
const char *purple_chat_get_name(PurpleChat *chat);

/**
 * Finds the buddy struct given a name and an account
 *
 * @param account The account this buddy belongs to
 * @param name    The buddy's name
 * @return        The buddy or NULL if the buddy does not exist
 */
PurpleBuddy *purple_find_buddy(PurpleAccount *account, const char *name);

/**
 * Finds the buddy struct given a name, an account, and a group
 *
 * @param account The account this buddy belongs to
 * @param name    The buddy's name
 * @param group   The group to look in
 * @return        The buddy or NULL if the buddy does not exist in the group
 */
PurpleBuddy *purple_find_buddy_in_group(PurpleAccount *account, const char *name,
		PurpleGroup *group);

/**
 * Finds all PurpleBuddy structs given a name and an account
 *
 * @param account The account this buddy belongs to
 * @param name    The buddy's name (or NULL to return all buddies for the account)
 *
 * @return        A GSList of buddies (which must be freed), or NULL if the buddy doesn't exist
 */
GSList *purple_find_buddies(PurpleAccount *account, const char *name);


/**
 * Finds a group by name
 *
 * @param name    The group's name
 * @return        The group or NULL if the group does not exist
 */
PurpleGroup *purple_find_group(const char *name);

/**
 * Finds a chat by name.
 *
 * @param account The chat's account.
 * @param name    The chat's name.
 *
 * @return The chat, or @c NULL if the chat does not exist.
 */
PurpleChat *purple_blist_find_chat(PurpleAccount *account, const char *name);

/**
 * Returns the group of which the chat is a member.
 *
 * @param chat The chat.
 *
 * @return The parent group, or @c NULL if the chat is not in a group.
 */
PurpleGroup *purple_chat_get_group(PurpleChat *chat);

/**
 * Returns the account the chat belongs to.
 *
 * @param chat  The chat.
 *
 * @return  The account the chat belongs to.
 *
 * @since 2.4.0
 */
PurpleAccount *purple_chat_get_account(PurpleChat *chat);

/**
 * Get a hashtable containing information about a chat.
 *
 * @param chat  The chat.
 *
 * @constreturn  The hashtable.
 *
 * @since 2.4.0
 */
GHashTable *purple_chat_get_components(PurpleChat *chat);

/**
 * Returns the group of which the buddy is a member.
 *
 * @param buddy   The buddy
 * @return        The group or NULL if the buddy is not in a group
 */
PurpleGroup *purple_buddy_get_group(PurpleBuddy *buddy);


/**
 * Returns a list of accounts that have buddies in this group
 *
 * @param g The group
 *
 * @return A GSList of accounts (which must be freed), or NULL if the group
 *         has no accounts.
 */
GSList *purple_group_get_accounts(PurpleGroup *g);

/**
 * Determines whether an account owns any buddies in a given group
 *
 * @param g       The group to search through.
 * @param account The account.
 *
 * @return TRUE if there are any buddies in the group, or FALSE otherwise.
 */
gboolean purple_group_on_account(PurpleGroup *g, PurpleAccount *account);

/**
 * Returns the name of a group.
 *
 * @param group The group.
 *
 * @return The name of the group.
 */
const char *purple_group_get_name(PurpleGroup *group);

/**
 * Called when an account connects.  Tells the UI to update all the
 * buddies.
 *
 * @param account   The account
 */
void purple_blist_add_account(PurpleAccount *account);


/**
 * Called when an account disconnects.  Sets the presence of all the buddies to 0
 * and tells the UI to update them.
 *
 * @param account   The account
 */
void purple_blist_remove_account(PurpleAccount *account);


/**
 * Determines the total size of a group
 *
 * @param group  The group
 * @param offline Count buddies in offline accounts
 * @return The number of buddies in the group
 */
int purple_blist_get_group_size(PurpleGroup *group, gboolean offline);

/**
 * Determines the number of online buddies in a group
 *
 * @param group The group
 * @return The number of online buddies in the group, or 0 if the group is NULL
 */
int purple_blist_get_group_online_count(PurpleGroup *group);

/*@}*/

/****************************************************************************************/
/** @name Buddy list file management API                                                */
/****************************************************************************************/

/**
 * Loads the buddy list from ~/.purple/blist.xml.
 */
void purple_blist_load(void);

/**
 * Schedule a save of the blist.xml file.  This is used by the privacy
 * API whenever the privacy settings are changed.  If you make a change
 * to blist.xml using one of the functions in the buddy list API, then
 * the buddy list is saved automatically, so you should not need to
 * call this.
 */
void purple_blist_schedule_save(void);

/**
 * Requests from the user information needed to add a buddy to the
 * buddy list.
 *
 * @param account  The account the buddy is added to.
 * @param username The username of the buddy.
 * @param group    The name of the group to place the buddy in.
 * @param alias    The optional alias for the buddy.
 */
void purple_blist_request_add_buddy(PurpleAccount *account, const char *username,
								  const char *group, const char *alias);

/**
 * Requests from the user information needed to add a chat to the
 * buddy list.
 *
 * @param account The account the buddy is added to.
 * @param group   The optional group to add the chat to.
 * @param alias   The optional alias for the chat.
 * @param name    The required chat name.
 */
void purple_blist_request_add_chat(PurpleAccount *account, PurpleGroup *group,
								 const char *alias, const char *name);

/**
 * Requests from the user information needed to add a group to the
 * buddy list.
 */
void purple_blist_request_add_group(void);

/**
 * Associates a boolean with a node in the buddy list
 *
 * @param node  The node to associate the data with
 * @param key   The identifier for the data
 * @param value The value to set
 */
void purple_blist_node_set_bool(PurpleBlistNode *node, const char *key, gboolean value);

/**
 * Retrieves a named boolean setting from a node in the buddy list
 *
 * @param node  The node to retrieve the data from
 * @param key   The identifier of the data
 *
 * @return The value, or FALSE if there is no setting
 */
gboolean purple_blist_node_get_bool(PurpleBlistNode *node, const char *key);

/**
 * Associates an integer with a node in the buddy list
 *
 * @param node  The node to associate the data with
 * @param key   The identifier for the data
 * @param value The value to set
 */
void purple_blist_node_set_int(PurpleBlistNode *node, const char *key, int value);

/**
 * Retrieves a named integer setting from a node in the buddy list
 *
 * @param node  The node to retrieve the data from
 * @param key   The identifier of the data
 *
 * @return The value, or 0 if there is no setting
 */
int purple_blist_node_get_int(PurpleBlistNode *node, const char *key);

/**
 * Associates a string with a node in the buddy list
 *
 * @param node  The node to associate the data with
 * @param key   The identifier for the data
 * @param value The value to set
 */
void purple_blist_node_set_string(PurpleBlistNode *node, const char *key,
		const char *value);

/**
 * Retrieves a named string setting from a node in the buddy list
 *
 * @param node  The node to retrieve the data from
 * @param key   The identifier of the data
 *
 * @return The value, or NULL if there is no setting
 */
const char *purple_blist_node_get_string(PurpleBlistNode *node, const char *key);

/**
 * Removes a named setting from a blist node
 *
 * @param node  The node from which to remove the setting
 * @param key   The name of the setting
 */
void purple_blist_node_remove_setting(PurpleBlistNode *node, const char *key);

/**
 * Set the flags for the given node.  Setting a node's flags will overwrite
 * the old flags, so if you want to save them, you must first call
 * purple_blist_node_get_flags and modify that appropriately.
 *
 * @param node  The node on which to set the flags.
 * @param flags The flags to set.  This is a bitmask.
 */
void purple_blist_node_set_flags(PurpleBlistNode *node, PurpleBlistNodeFlags flags);

/**
 * Get the current flags on a given node.
 *
 * @param node The node from which to get the flags.
 *
 * @return The flags on the node.  This is a bitmask.
 */
PurpleBlistNodeFlags purple_blist_node_get_flags(PurpleBlistNode *node);

/**
 * Get the type of a given node.
 *
 * @param node The node.
 *
 * @return The type of the node.
 *
 * @since 2.1.0
 */
PurpleBlistNodeType purple_blist_node_get_type(PurpleBlistNode *node);

/*@}*/

/**
 * Retrieves the extended menu items for a buddy list node.
 * @param n The blist node for which to obtain the extended menu items.
 * @return  A list of PurpleMenuAction items, as harvested by the
 *          blist-node-extended-menu signal.
 */
GList *purple_blist_node_get_extended_menu(PurpleBlistNode *n);

/**************************************************************************/
/** @name UI Registration Functions                                       */
/**************************************************************************/
/*@{*/

/**
 * Sets the UI operations structure to be used for the buddy list.
 *
 * @param ops The ops struct.
 */
void purple_blist_set_ui_ops(PurpleBlistUiOps *ops);

/**
 * Returns the UI operations structure to be used for the buddy list.
 *
 * @return The UI operations structure.
 */
PurpleBlistUiOps *purple_blist_get_ui_ops(void);

/*@}*/

/**************************************************************************/
/** @name Buddy List Subsystem                                            */
/**************************************************************************/
/*@{*/

/**
 * Returns the handle for the buddy list subsystem.
 *
 * @return The buddy list subsystem handle.
 */
void *purple_blist_get_handle(void);

/**
 * Initializes the buddy list subsystem.
 */
void purple_blist_init(void);

/**
 * Uninitializes the buddy list subsystem.
 */
void purple_blist_uninit(void);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_BLIST_H_ */
