/**
 * @file plugin.h Plugin API
 * @ingroup core
 * @see @ref plugin-signals
 * @see @ref plugin-ids
 * @see @ref plugin-i18n
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
#ifndef _PURPLE_PLUGIN_H_
#define _PURPLE_PLUGIN_H_

#include <glib.h>
#include <gmodule.h>
#include "signals.h"
#include "value.h"

/** @copydoc _PurplePlugin */
typedef struct _PurplePlugin           PurplePlugin;
/** @copydoc _PurplePluginInfo */
typedef struct _PurplePluginInfo       PurplePluginInfo;
/** @copydoc _PurplePluginUiInfo */
typedef struct _PurplePluginUiInfo     PurplePluginUiInfo;
/** @copydoc _PurplePluginLoaderInfo */
typedef struct _PurplePluginLoaderInfo PurplePluginLoaderInfo;

/** @copydoc _PurplePluginAction */
typedef struct _PurplePluginAction     PurplePluginAction;

typedef int PurplePluginPriority; /**< Plugin priority. */

#include "pluginpref.h"

/**
 * Plugin types.
 */
typedef enum
{
	PURPLE_PLUGIN_UNKNOWN  = -1,  /**< Unknown type.    */
	PURPLE_PLUGIN_STANDARD = 0,   /**< Standard plugin. */
	PURPLE_PLUGIN_LOADER,         /**< Loader plugin.   */
	PURPLE_PLUGIN_PROTOCOL        /**< Protocol plugin. */

} PurplePluginType;

#define PURPLE_PRIORITY_DEFAULT     0
#define PURPLE_PRIORITY_HIGHEST  9999
#define PURPLE_PRIORITY_LOWEST  -9999

#define PURPLE_PLUGIN_FLAG_INVISIBLE 0x01

#define PURPLE_PLUGIN_MAGIC 5 /* once we hit 6.0.0 I think we can remove this */

/**
 * Detailed information about a plugin.
 *
 * This is used in the version 2.0 API and up.
 */
struct _PurplePluginInfo
{
	unsigned int magic;
	unsigned int major_version;
	unsigned int minor_version;
	PurplePluginType type;
	char *ui_requirement;
	unsigned long flags;
	GList *dependencies;
	PurplePluginPriority priority;

	char *id;
	char *name;
	char *version;
	char *summary;
	char *description;
	char *author;
	char *homepage;

	/**
	 * If a plugin defines a 'load' function, and it returns FALSE,
	 * then the plugin will not be loaded.
	 */
	gboolean (*load)(PurplePlugin *plugin);
	gboolean (*unload)(PurplePlugin *plugin);
	void (*destroy)(PurplePlugin *plugin);

	void *ui_info; /**< Used only by UI-specific plugins to build a preference screen with a custom UI */
	void *extra_info;
	PurplePluginUiInfo *prefs_info; /**< Used by any plugin to display preferences.  If #ui_info has been specified, this will be ignored. */

	/**
	 * This callback has a different use depending on whether this
	 * plugin type is PURPLE_PLUGIN_STANDARD or PURPLE_PLUGIN_PROTOCOL.
	 *
	 * If PURPLE_PLUGIN_STANDARD then the list of actions will show up
	 * in the Tools menu, under a submenu with the name of the plugin.
	 * context will be NULL.
	 *
	 * If PURPLE_PLUGIN_PROTOCOL then the list of actions will show up
	 * in the Accounts menu, under a submenu with the name of the
	 * account.  context will be set to the PurpleConnection for that
	 * account.  This callback will only be called for online accounts.
	 */
	GList *(*actions)(PurplePlugin *plugin, gpointer context);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

/**
 * Extra information for loader plugins.
 */
struct _PurplePluginLoaderInfo
{
	GList *exts;

	gboolean (*probe)(PurplePlugin *plugin);
	gboolean (*load)(PurplePlugin *plugin);
	gboolean (*unload)(PurplePlugin *plugin);
	void     (*destroy)(PurplePlugin *plugin);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

/**
 * A plugin handle.
 */
struct _PurplePlugin
{
	gboolean native_plugin;                /**< Native C plugin.          */
	gboolean loaded;                       /**< The loaded state.         */
	void *handle;                          /**< The module handle.        */
	char *path;                            /**< The path to the plugin.   */
	PurplePluginInfo *info;                  /**< The plugin information.   */
	char *error;
	void *ipc_data;                        /**< IPC data.                 */
	void *extra;                           /**< Plugin-specific data.     */
	gboolean unloadable;                   /**< Unloadable                */
	GList *dependent_plugins;              /**< Plugins depending on this */

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

#define PURPLE_PLUGIN_LOADER_INFO(plugin) \
	((PurplePluginLoaderInfo *)(plugin)->info->extra_info)

struct _PurplePluginUiInfo {
	PurplePluginPrefFrame *(*get_plugin_pref_frame)(PurplePlugin *plugin);

	int page_num;                                         /**< Reserved */
	PurplePluginPrefFrame *frame;                           /**< Reserved */

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

#define PURPLE_PLUGIN_HAS_PREF_FRAME(plugin) \
	((plugin)->info != NULL && (plugin)->info->prefs_info != NULL)

#define PURPLE_PLUGIN_UI_INFO(plugin) \
	((PurplePluginUiInfo*)(plugin)->info->prefs_info)


/**
 * The structure used in the actions member of PurplePluginInfo
 */
struct _PurplePluginAction {
	char *label;
	void (*callback)(PurplePluginAction *);

	/** set to the owning plugin */
	PurplePlugin *plugin;

	/** NULL for plugin actions menu, set to the PurpleConnection for
	    account actions menu */
	gpointer context;

	gpointer user_data;
};

#define PURPLE_PLUGIN_HAS_ACTIONS(plugin) \
	((plugin)->info != NULL && (plugin)->info->actions != NULL)

#define PURPLE_PLUGIN_ACTIONS(plugin, context) \
	(PURPLE_PLUGIN_HAS_ACTIONS(plugin)? \
	(plugin)->info->actions(plugin, context): NULL)


/**
 * Handles the initialization of modules.
 */
#if !defined(PURPLE_PLUGINS) || defined(PURPLE_STATIC_PRPL)
# define _FUNC_NAME(x) purple_init_##x##_plugin
# define PURPLE_INIT_PLUGIN(pluginname, initfunc, plugininfo) \
	gboolean _FUNC_NAME(pluginname)(void);\
	gboolean _FUNC_NAME(pluginname)(void) { \
		PurplePlugin *plugin = purple_plugin_new(TRUE, NULL); \
		plugin->info = &(plugininfo); \
		initfunc((plugin)); \
		purple_plugin_load((plugin)); \
		return purple_plugin_register(plugin); \
	}
#else /* PURPLE_PLUGINS  && !PURPLE_STATIC_PRPL */
# define PURPLE_INIT_PLUGIN(pluginname, initfunc, plugininfo) \
	G_MODULE_EXPORT gboolean purple_init_plugin(PurplePlugin *plugin); \
	G_MODULE_EXPORT gboolean purple_init_plugin(PurplePlugin *plugin) { \
		plugin->info = &(plugininfo); \
		initfunc((plugin)); \
		return purple_plugin_register(plugin); \
	}
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Plugin API                                                      */
/**************************************************************************/
/*@{*/

/**
 * Creates a new plugin structure.
 *
 * @param native Whether or not the plugin is native.
 * @param path   The path to the plugin, or @c NULL if statically compiled.
 *
 * @return A new PurplePlugin structure.
 */
PurplePlugin *purple_plugin_new(gboolean native, const char *path);

/**
 * Probes a plugin, retrieving the information on it and adding it to the
 * list of available plugins.
 *
 * @param filename The plugin's filename.
 *
 * @return The plugin handle.
 *
 * @see purple_plugin_load()
 * @see purple_plugin_destroy()
 */
PurplePlugin *purple_plugin_probe(const char *filename);

/**
 * Registers a plugin and prepares it for loading.
 *
 * This shouldn't be called by anything but the internal module code.
 * Plugins should use the PURPLE_INIT_PLUGIN() macro to register themselves
 * with the core.
 *
 * @param plugin The plugin to register.
 *
 * @return @c TRUE if the plugin was registered successfully.  Otherwise
 *         @c FALSE is returned (this happens if the plugin does not contain
 *         the necessary information).
 */
gboolean purple_plugin_register(PurplePlugin *plugin);

/**
 * Attempts to load a previously probed plugin.
 *
 * @param plugin The plugin to load.
 *
 * @return @c TRUE if successful, or @c FALSE otherwise.
 *
 * @see purple_plugin_reload()
 * @see purple_plugin_unload()
 */
gboolean purple_plugin_load(PurplePlugin *plugin);

/**
 * Unloads the specified plugin.
 *
 * @param plugin The plugin handle.
 *
 * @return @c TRUE if successful, or @c FALSE otherwise.
 *
 * @see purple_plugin_load()
 * @see purple_plugin_reload()
 */
gboolean purple_plugin_unload(PurplePlugin *plugin);

/**
 * Disable a plugin.
 *
 * This function adds the plugin to a list of plugins to "disable at the next
 * startup" by excluding said plugins from the list of plugins to save.  The
 * UI needs to call purple_plugins_save_loaded() after calling this for it
 * to have any effect.
 *
 * @since 2.3.0
 */
void purple_plugin_disable(PurplePlugin *plugin);

/**
 * Reloads a plugin.
 *
 * @param plugin The old plugin handle.
 *
 * @return @c TRUE if successful, or @c FALSE otherwise.
 *
 * @see purple_plugin_load()
 * @see purple_plugin_unload()
 */
gboolean purple_plugin_reload(PurplePlugin *plugin);

/**
 * Unloads a plugin and destroys the structure from memory.
 *
 * @param plugin The plugin handle.
 */
void purple_plugin_destroy(PurplePlugin *plugin);

/**
 * Returns whether or not a plugin is currently loaded.
 *
 * @param plugin The plugin.
 *
 * @return @c TRUE if loaded, or @c FALSE otherwise.
 */
gboolean purple_plugin_is_loaded(const PurplePlugin *plugin);

/**
 * Returns whether or not a plugin is unloadable.
 *
 * If this returns @c TRUE, the plugin is guaranteed to not
 * be loadable. However, a return value of @c FALSE does not
 * guarantee the plugin is loadable.
 *
 * @param plugin The plugin.
 *
 * @return @c TRUE if the plugin is known to be unloadable,\
 *         @c FALSE otherwise
 */
gboolean purple_plugin_is_unloadable(const PurplePlugin *plugin);

/**
 * Returns a plugin's id.
 *
 * @param plugin The plugin.
 *
 * @return The plugin's id.
 */
const gchar *purple_plugin_get_id(const PurplePlugin *plugin);

/**
 * Returns a plugin's name.
 *
 * @param plugin The plugin.
 *
 * @return THe name of the plugin, or @c NULL.
 */
const gchar *purple_plugin_get_name(const PurplePlugin *plugin);

/**
 * Returns a plugin's version.
 *
 * @param plugin The plugin.
 *
 * @return The plugin's version or @c NULL.
 */
const gchar *purple_plugin_get_version(const PurplePlugin *plugin);

/**
 * Returns a plugin's summary.
 *
 * @param plugin The plugin.
 *
 * @return The plugin's summary.
 */
const gchar *purple_plugin_get_summary(const PurplePlugin *plugin);

/**
 * Returns a plugin's description.
 *
 * @param plugin The plugin.
 *
 * @return The plugin's description.
 */
const gchar *purple_plugin_get_description(const PurplePlugin *plugin);

/**
 * Returns a plugin's author.
 *
 * @param plugin The plugin.
 *
 * @return The plugin's author.
 */
const gchar *purple_plugin_get_author(const PurplePlugin *plugin);

/**
 * Returns a plugin's homepage.
 *
 * @param plugin The plugin.
 *
 * @return The plugin's homepage.
 */
const gchar *purple_plugin_get_homepage(const PurplePlugin *plugin);

/*@}*/

/**************************************************************************/
/** @name Plugin IPC API                                                  */
/**************************************************************************/
/*@{*/

/**
 * Registers an IPC command in a plugin.
 *
 * @param plugin     The plugin to register the command with.
 * @param command    The name of the command.
 * @param func       The function to execute.
 * @param marshal    The marshalling function.
 * @param ret_value  The return value type.
 * @param num_params The number of parameters.
 * @param ...        The parameter types.
 *
 * @return TRUE if the function was registered successfully, or
 *         FALSE otherwise.
 */
gboolean purple_plugin_ipc_register(PurplePlugin *plugin, const char *command,
								  PurpleCallback func,
								  PurpleSignalMarshalFunc marshal,
								  PurpleValue *ret_value, int num_params, ...);

/**
 * Unregisters an IPC command in a plugin.
 *
 * @param plugin  The plugin to unregister the command from.
 * @param command The name of the command.
 */
void purple_plugin_ipc_unregister(PurplePlugin *plugin, const char *command);

/**
 * Unregisters all IPC commands in a plugin.
 *
 * @param plugin The plugin to unregister the commands from.
 */
void purple_plugin_ipc_unregister_all(PurplePlugin *plugin);

/**
 * Returns a list of value types used for an IPC command.
 *
 * @param plugin     The plugin.
 * @param command    The name of the command.
 * @param ret_value  The returned return value.
 * @param num_params The returned number of parameters.
 * @param params     The returned list of parameters.
 *
 * @return TRUE if the command was found, or FALSE otherwise.
 */
gboolean purple_plugin_ipc_get_params(PurplePlugin *plugin, const char *command,
									PurpleValue **ret_value, int *num_params,
									PurpleValue ***params);

/**
 * Executes an IPC command.
 *
 * @param plugin  The plugin to execute the command on.
 * @param command The name of the command.
 * @param ok      TRUE if the call was successful, or FALSE otherwise.
 * @param ...     The parameters to pass.
 *
 * @return The return value, which will be NULL if the command doesn't
 *         return a value.
 */
void *purple_plugin_ipc_call(PurplePlugin *plugin, const char *command,
						   gboolean *ok, ...);

/*@}*/

/**************************************************************************/
/** @name Plugins API                                                     */
/**************************************************************************/
/*@{*/

/**
 * Add a new directory to search for plugins
 *
 * @param path The new search path.
 */
void purple_plugins_add_search_path(const char *path);

/**
 * Returns a list of plugin search paths.
 *
 * @constreturn A list of searched paths.
 *
 * @since 2.6.0
 */
GList *purple_plugins_get_search_paths(void);

/**
 * Unloads all loaded plugins.
 */
void purple_plugins_unload_all(void);

/**
 * Unloads all plugins of a specific type.
 */
void purple_plugins_unload(PurplePluginType type);

/**
 * Destroys all registered plugins.
 */
void purple_plugins_destroy_all(void);

/**
 * Saves the list of loaded plugins to the specified preference key
 *
 * @param key The preference key to save the list of plugins to.
 */
void purple_plugins_save_loaded(const char *key);

/**
 * Attempts to load all the plugins in the specified preference key
 * that were loaded when purple last quit.
 *
 * @param key The preference key containing the list of plugins.
 */
void purple_plugins_load_saved(const char *key);

/**
 * Probes for plugins in the registered module paths.
 *
 * @param ext The extension type to probe for, or @c NULL for all.
 *
 * @see purple_plugin_set_probe_path()
 */
void purple_plugins_probe(const char *ext);

/**
 * Returns whether or not plugin support is enabled.
 *
 * @return TRUE if plugin support is enabled, or FALSE otherwise.
 */
gboolean purple_plugins_enabled(void);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_PLUGIN_C_)
/**
 * Registers a function that will be called when probing is finished.
 *
 * @param func The callback function.
 * @param data Data to pass to the callback.
 * @deprecated If you need this, ask for a plugin-probe signal to be added.
 */
void purple_plugins_register_probe_notify_cb(void (*func)(void *), void *data);
#endif

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_PLUGIN_C_)
/**
 * Unregisters a function that would be called when probing is finished.
 *
 * @param func The callback function.
 * @deprecated If you need this, ask for a plugin-probe signal to be added.
 */
void purple_plugins_unregister_probe_notify_cb(void (*func)(void *));
#endif

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_PLUGIN_C_)
/**
 * Registers a function that will be called when a plugin is loaded.
 *
 * @param func The callback function.
 * @param data Data to pass to the callback.
 * @deprecated Use the plugin-load signal instead.
 */
void purple_plugins_register_load_notify_cb(void (*func)(PurplePlugin *, void *),
										  void *data);
#endif

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_PLUGIN_C_)
/**
 * Unregisters a function that would be called when a plugin is loaded.
 *
 * @param func The callback function.
 * @deprecated Use the plugin-load signal instead.
 */
void purple_plugins_unregister_load_notify_cb(void (*func)(PurplePlugin *, void *));
#endif

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_PLUGIN_C_)
/**
 * Registers a function that will be called when a plugin is unloaded.
 *
 * @param func The callback function.
 * @param data Data to pass to the callback.
 * @deprecated Use the plugin-unload signal instead.
 */
void purple_plugins_register_unload_notify_cb(void (*func)(PurplePlugin *, void *),
											void *data);
#endif

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_PLUGIN_C_)
/**
 * Unregisters a function that would be called when a plugin is unloaded.
 *
 * @param func The callback function.
 * @deprecated Use the plugin-unload signal instead.
 */
void purple_plugins_unregister_unload_notify_cb(void (*func)(PurplePlugin *,
														   void *));
#endif

/**
 * Finds a plugin with the specified name.
 *
 * @param name The plugin name.
 *
 * @return The plugin if found, or @c NULL if not found.
 */
PurplePlugin *purple_plugins_find_with_name(const char *name);

/**
 * Finds a plugin with the specified filename (filename with a path).
 *
 * @param filename The plugin filename.
 *
 * @return The plugin if found, or @c NULL if not found.
 */
PurplePlugin *purple_plugins_find_with_filename(const char *filename);

/**
 * Finds a plugin with the specified basename (filename without a path).
 *
 * @param basename The plugin basename.
 *
 * @return The plugin if found, or @c NULL if not found.
 */
PurplePlugin *purple_plugins_find_with_basename(const char *basename);

/**
 * Finds a plugin with the specified plugin ID.
 *
 * @param id The plugin ID.
 *
 * @return The plugin if found, or @c NULL if not found.
 */
PurplePlugin *purple_plugins_find_with_id(const char *id);

/**
 * Returns a list of all loaded plugins.
 *
 * @constreturn A list of all loaded plugins.
 */
GList *purple_plugins_get_loaded(void);

/**
 * Returns a list of all valid protocol plugins.  A protocol
 * plugin is considered invalid if it does not contain the call
 * to the PURPLE_INIT_PLUGIN() macro, or if it was compiled
 * against an incompatable API version.
 *
 * @constreturn A list of all protocol plugins.
 */
GList *purple_plugins_get_protocols(void);

/**
 * Returns a list of all plugins, whether loaded or not.
 *
 * @constreturn A list of all plugins.
 */
GList *purple_plugins_get_all(void);

/*@}*/

/**************************************************************************/
/** @name Plugins SubSytem API                                            */
/**************************************************************************/
/*@{*/

/**
 * Returns the plugin subsystem handle.
 *
 * @return The plugin sybsystem handle.
 */
void *purple_plugins_get_handle(void);

/**
 * Initializes the plugin subsystem
 */
void purple_plugins_init(void);

/**
 * Uninitializes the plugin subsystem
 */
void purple_plugins_uninit(void);

/*@}*/

/**
 * Allocates and returns a new PurplePluginAction.
 *
 * @param label    The description of the action to show to the user.
 * @param callback The callback to call when the user selects this action.
 */
PurplePluginAction *purple_plugin_action_new(const char* label, void (*callback)(PurplePluginAction *));

/**
 * Frees a PurplePluginAction
 *
 * @param action The PurplePluginAction to free.
 */
void purple_plugin_action_free(PurplePluginAction *action);

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_PLUGIN_H_ */
