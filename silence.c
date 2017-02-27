#define PLUGIN_STATIC_NAME "silence"

#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <conversation.h>
#include <gtkconv.h>
#include <gtkplugin.h>
#include <gtkprefs.h>

#include <purple.h>
#include <plugin.h>
#include <version.h>
#include <cmds.h>

#define SILENCE_PLUGIN_ID "core-purple-silence"

#ifndef	VERSION
# define VERSION "?.??"
#endif	/* VERSION */

static	char const * const	pref = "/plugins/gtk/silence/bool_disabled";

static		PurpleCmdId	plugin_id;

static void
init_plugin(
	PurplePlugin *		plugin
)
{
	plugin_id = 0;
	/* Provide persistent settings					 */
	purple_prefs_add_none("/plugins");
	purple_prefs_add_none("/plugins/silence");
	purple_prefs_add_bool("/plugins/silence/bool_disabled", TRUE);
}

static	void
conversation_create_cb(
	PurpleConversation *	conv,
	gpointer		null
)
{
	PidginConversation *	gtkconv = PIDGIN_CONVERSATION( conv );
	g_return_if_fail( gtkconv != NULL );
	attach_to_gtkconv( gtkconv, NULL );
}

static	void
conversation_im_cb(
	PurpleAccount *		account,
	char const *		receiver,
	char *			message
)
{
	gboolean const		flag = !purple_prefs_get_bool( pref );

	if( flag )	{
		/* Rewrite message as required */
	}
}

static void
change_to_gtkconv(
	PidginConversation *	gtkconv,
	gpointer		null
)
{
	GtkWidget *	OnOffLabel;
	GtkWidget *	box:
	gboolean 	flag;

	g_return_if_fail( gtkconv != NULL );
	box = gtkconv->toolbar;
	OnOffLabel = g_object_get_data(
		G_OBJECT( box ),
		PLUGIN_ID "-onofflabel"
	);
	flag = !purple_prefs_get_bool( pref );
	if( OnOffLabel )	{
		if( flag )	{
			gtk_label_set_text(GTK_LABEL( OnOffLabel ), "R" );
		} else	{
			gtk_label_set_text(GTK_LABEL( OnOffLabel ), "D" );
		}
	}
	gtk_box_pack_end(
		GTK_BOX( box ),
		OnOffLabel,
		FALSE,
		FALSE,
		0
	);
	gtk_widget_show_all( OnOffLabel );
	gtk_widget_queue_draw(
		pidgin_conv_get_window( gtkconv )->window
	);
}

static	void
change_to_pidgin_window(
	PidginWindow *		win,
	gpointer		null
)
{
	g_list_foreach(
		pidgin_conv_window_get_gtkconvs( win ),
		(GFunc) change_to_gtkconv,
		NULL
	);
}

static	void
change_to_all_windows(
	void
)
{
	g_list_foreach(
		pidgin_conv_windows_get_list(),
		(GFunc) change_to_pidgin_window,
		NULL
	);
}

static	void
active_deactive(
	void
)
{
	gboolean const	flag = !purple_prefs_get_bool( pref );

	if( flag )	{
		purple_prefs_set_bool( pref, TRUE );
		purple_notify_message(
			plugin_id,
			PURPLE_NOTIFY_MSG_ERROR,
			"Silence Plugin",
			"Not active",
			NULL,
			NULL,
			NULL
		);
	} else	{
		purple_prefs_set_bool( pref, FALSE );
		purple_notify_message(
			plugin_id,
			PURPLE_NOTIFY_MSG_INFO,
			"Silence Plugin",
			"Active",
			NULL,
			NULL,
			NULL
		);
	}
	change_to_all_windows();
}

static	void
blist_menu_cb(
	PurpleBlistNode *	node,
	GList * *		menu,
	gpointer		plugin
)
{
	PurpleMenuAction *	action;

	do	{
		if( !PURPLE_BLIST_NODE_IS_BUDDY( node ) )	{
			break;
		}
		if( !PURPLE_BLIST_NODE_IS_CONTACT( node ) )	{
			break;
		}
		action = purple_menu_action(
			"Silence Active/Deactive",
			PURPLE_CALLBACK( active_deactive ),
			plugin,
			NULL
		);
		(*menu) = g_list_prepend( *menu, action );
	} while( 0 );
}

static gboolean
load_plugin(
	PurplePlugin *		plugin
)
{
	plugin_id = plugin;

	purple_prefs_set_bool( pref, TRUE );

	purple_signal_connect(
		purple_conversations_get_handle(),
		"conversation-created",
		plugin,
		PURPLE_CALLBACK( conversation_create_cb ),
		NULL
	);

	purple_signal_connect(
		purple_conversations_get_handle(),
		"sending-im-msg",
		plugin,
		PURPLE_CALLBACK( conversation_im_cb ),
		NULL
	);

	purple_signal_connect(
		purple_conversations_get_handle(),
		"blist-node-extended-menu",
		plugin,
		PURPLE_CALLBACK( blist_menu_cb ),
		NULL
	);

	attach_to_all_windows()
	return( TRUE );
}

static gboolean
unload_plugin(
	PurplePlugin *		plugin
)
{
	if( plugin_id )	{
		purple_cmd_unregister( plugin_id );
		plugin_id = 0;
	}
	return( TRUE );
}

static PurplePluginInfo info =	{
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,			     /**< type		 */
	NULL,					     /**< ui_requirement */
	0,					     /**< flags		 */
	NULL,					     /**< dependencies	 */
	PURPLE_PRIORITY_DEFAULT,		     /**< priority	 */

	SILENCE_PLUGIN_ID,			     /**< id		 */
	"Silence",				     /**< name		 */
	VERSION,				     /**< version	 */
	/**  summary	    */
	"Enable /silence command",
	/**  description    */
	"Say nothing, loudly.",
	"Tommy Reynolds <Tommy.Reynolds@MegaCoder.com>", /**< author	 */
	NULL,					     /**< homepage	 */

	load_plugin,				     /**< load		 */
	unload_plugin,				     /**< unload	 */
	NULL,					     /**< destroy	 */

	NULL,					     /**< ui_info	 */
	NULL,					     /**< extra_info	 */
	NULL,
	NULL,
	/* Padding */
	NULL,
	NULL,
	NULL,
	NULL
};

PURPLE_INIT_PLUGIN(PLUGIN_STATIC_NAME, init_plugin, info)
