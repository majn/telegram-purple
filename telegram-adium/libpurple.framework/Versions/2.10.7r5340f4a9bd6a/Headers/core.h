/**
 * @file core.h Startup and shutdown of libpurple
 * @defgroup core libpurple
 * @see @ref core-signals
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

/*! @mainpage Pidgin/Finch/libpurple API Documentation
 *
 * <a href="group__core.html">libpurple</a> is intended to be the core of an IM
 * program.  <a href="group__pidgin.html">Pidgin</a> is a GTK+ frontend
 * to libpurple, and <a href="group__finch.html">Finch</a> is an ncurses
 * frontend built using <a href="group__gnt.html">libgnt</a>
 * (GLib Ncurses Toolkit).
 */

#ifndef _PURPLE_CORE_H_
#define _PURPLE_CORE_H_

typedef struct PurpleCore PurpleCore;

/** Callbacks that fire at different points of the initialization and teardown
 *  of libpurple, along with a hook to return descriptive information about the
 *  UI.
 */
typedef struct
{
	/** Called just after the preferences subsystem is initialized; the UI
	 *  could use this callback to add some preferences it needs to be in
	 *  place when other subsystems are initialized.
	 */
	void (*ui_prefs_init)(void);
	/** Called just after the debug subsystem is initialized, but before
	 *  just about every other component's initialization.  The UI should
	 *  use this hook to call purple_debug_set_ui_ops() so that debugging
	 *  information for other components can be logged during their
	 *  initialization.
	 */
	void (*debug_ui_init)(void);
	/** Called after all of libpurple has been initialized.  The UI should
	 *  use this hook to set all other necessary UiOps structures.
	 *
	 *  @see @ref ui-ops
	 */
	void (*ui_init)(void);
	/** Called after most of libpurple has been uninitialized. */
	void (*quit)(void);

	/** Called by purple_core_get_ui_info(); should return the information
	 *  documented there.
	 */
	GHashTable* (*get_ui_info)(void);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
} PurpleCoreUiOps;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the core of purple.
 *
 * This will setup preferences for all the core subsystems.
 *
 * @param ui The ID of the UI using the core. This should be a
 *           unique ID, registered with the purple team.
 *
 * @return @c TRUE if successful, or @c FALSE otherwise.
 */
gboolean purple_core_init(const char *ui);

/**
 * Quits the core of purple, which, depending on the UI, may quit the
 * application using the purple core.
 */
void purple_core_quit(void);

/**
 * <p>
 * Calls purple_core_quit().  This can be used as the function
 * passed to purple_timeout_add() when you want to shutdown Purple
 * in a specified amount of time.  When shutting down Purple
 * from a plugin, you must use this instead of purple_core_quit();
 * for an immediate exit, use a timeout value of 0:
 * </p>
 *
 * <code>purple_timeout_add(0, purple_core_quitcb, NULL);</code>
 *
 * <p>
 * This is ensures that code from your plugin is not being
 * executed when purple_core_quit() is called.  If the plugin
 * called purple_core_quit() directly, you would get a core dump
 * after purple_core_quit() executes and control returns to your
 * plugin because purple_core_quit() frees all plugins.
 * </p>
 */
gboolean purple_core_quit_cb(gpointer unused);

/**
 * Returns the version of the core library.
 *
 * @return The version of the core library.
 */
const char *purple_core_get_version(void);

/**
 * Returns the ID of the UI that is using the core, as passed to
 * purple_core_init().
 *
 * @return The ID of the UI that is currently using the core.
 */
const char *purple_core_get_ui(void);

/**
 * Returns a handle to the purple core.
 *
 * This is used to connect to @ref core-signals "core signals".
 */
PurpleCore *purple_get_core(void);

/**
 * Sets the UI ops for the core.
 *
 * @param ops A UI ops structure for the core.
 */
void purple_core_set_ui_ops(PurpleCoreUiOps *ops);

/**
 * Returns the UI ops for the core.
 *
 * @return The core's UI ops structure.
 */
PurpleCoreUiOps *purple_core_get_ui_ops(void);

/**
 * Migrates from <tt>.gaim</tt> to <tt>.purple</tt>.
 *
 * UIs <strong>must not</strong> call this if they have been told to use a
 * custom user directory.
 *
 * @return A boolean indicating success or migration failure. On failure,
 *         the application must display an error to the user and then exit.
 */
gboolean purple_core_migrate(void);

/**
 * Ensures that only one instance is running.  If libpurple is built with D-Bus
 * support, this checks if another process owns the libpurple bus name and if
 * so whether that process is using the same configuration directory as this
 * process.
 *
 * @return @c TRUE if this is the first instance of libpurple running;
 *         @c FALSE if there is another instance running.
 *
 * @since 2.1.0
 */
gboolean purple_core_ensure_single_instance(void);

/**
 * Returns a hash table containing various information about the UI.  The
 * following well-known entries may be in the table (along with any others the
 * UI might choose to include):
 *
 * <dl>
 *   <dt><tt>name</tt></dt>
 *   <dd>the user-readable name for the UI.</dd>
 *
 *   <dt><tt>version</tt></dt>
 *   <dd>a user-readable description of the current version of the UI.</dd>
 *
 *   <dt><tt>website</tt></dt>
 *   <dd>the UI's website, such as http://pidgin.im.</dd>
 *
 *   <dt><tt>dev_website</tt></dt>
 *   <dd>the UI's development/support website, such as http://developer.pidgin.im.</dd>
 *
 *   <dt><tt>client_type</tt></dt>
 *   <dd>the type of UI. Possible values include 'pc', 'console', 'phone',
 *       'handheld', 'web', and 'bot'. These values are compared
 *       programmatically and should not be localized.</dd>
 *
 * </dl>
 *
 * @return A GHashTable with strings for keys and values.  This
 * hash table must not be freed and should not be modified.
 *
 * @since 2.1.0
 *
 */
GHashTable* purple_core_get_ui_info(void);

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_CORE_H_ */

/*

                                                  /===-
                                                `//"\\   """"`---.___.-""
             ______-==|                         | |  \\           _-"`
       __--"""  ,-/-==\\                        | |   `\        ,'
    _-"       /'    |  \\            ___         / /      \      /
  .'        /       |   \\         /"   "\    /' /        \   /'
 /  ____  /         |    \`\.__/-""  D O   \_/'  /          \/'
/-'"    """""---__  |     "-/"   O G     R   /'        _--"`
                  \_|      /   R    __--_  t ),   __--""
                    '""--_/  T   _-"_>--<_\ h '-" \
                   {\__--_/}    / \\__>--<__\ e B  \
                   /'   (_/  _-"  | |__>--<__|   U  |
                  |   _/) )-"     | |__>--<__|  R   |
                  / /" ,_/       / /__>---<__/ N    |
                 o-o _//        /-"_>---<__-" I    /
                 (^("          /"_>---<__-  N   _-"
                ,/|           /__>--<__/  A  _-"
             ,//('(          |__>--<__|  T  /                  .----_
            ( ( '))          |__>--<__|    |                 /' _---_"\
         `-)) )) (           |__>--<__|  O |               /'  /     "\`\
        ,/,'//( (             \__>--<__\  R \            /'  //        ||
      ,( ( ((, ))              "-__>--<_"-_  "--____---"' _/'/        /'
    `"/  )` ) ,/|                 "-_">--<_/-__       __-" _/
  ._-"//( )/ )) `                    ""-'_/_/ /"""""""__--"
   ;'( ')/ ,)(                              """"""""""
  ' ') '( (/
    '   '  `

*/
