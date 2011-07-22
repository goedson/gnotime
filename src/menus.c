/*   Application menubar menu layout for GTimeTracker
 *   Copyright (C) 1997,98 Eckehard Berns
 *   Copyright (C) 2002, 2003 Linas Vepstas <linas@linas.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <config.h>
#include <string.h>
#include <stdlib.h>

#include "app.h"
#include "export.h"
#include "gtt.h"
#include "journal.h"
#include "menucmd.h"
#include "menus.h"
#include "plug-in.h"
#include "timer.h"



static GtkMenu *popup_menu;



/* Normal items */

static const GtkActionEntry entries[] = {
		{"FileMenu", NULL, "_File"},
		{"ProjectsMenu", NULL, "_Projects"},
		{"SettingsMenu", NULL, "_Settings"},
		{"ReportsMenu", NULL, "_Reports"},
		{"TimerMenu", NULL, "_Timer"},
		{"HelpMenu", NULL, "_Help"},

		// File menu actions
		{"ExportTasks", NULL, "_Export Tasks", "<control>E", "Export tasks to file", G_CALLBACK (export_file_picker)},
		{"ExportProjects", NULL, "Export _Projects", "<control>P", "Export projects to file", G_CALLBACK (export_file_picker)},
		{"Quit", GTK_STOCK_QUIT, "_Quit", "<control>Q", "Exit the program", G_CALLBACK (app_quit)},

		// Projects menu actions
		{"NewProject", GTK_STOCK_NEW, "_New ...", "<control>N", "Create a new project", G_CALLBACK (new_project)},
		{"CutProject", GTK_STOCK_CUT, "Cu_t", "<control>D", "Delete the selected project", G_CALLBACK (cut_project)},
		{"CopyProject", GTK_STOCK_COPY, "_Copy", "<control>F", "Copy the selected project", G_CALLBACK (copy_project)},
		{"PasteProject", GTK_STOCK_PASTE, "_Paste", "<control>G", "Paste the previously copied project", G_CALLBACK (paste_project)},
		{"EditTimes", GTK_STOCK_EDIT, "Edit _Times", NULL, "Edit the time interval associated with this project", G_CALLBACK (menu_howto_edit_times)},
		{"ProjectProperties",GTK_STOCK_PROPERTIES, "_Properties", "Edit project properties", G_CALLBACK (menu_properties)},


		// Settings menu actions
		{"Preferences", GTK_STOCK_PREFERENCES, "Prefere_nces", NULL, "Edit application preferences", G_CALLBACK (menu_options)},

		// Reports menu actions
		{"ReportJournal", NULL, "_Journal...", NULL, "Show the journal for this project", G_CALLBACK(show_journal_report)},
		{"ReportActivity", NULL, "_Activity...", NULL, "Show the journal together with the timestamps for this project", G_CALLBACK(show_activity_report)},
		{"ReportDaily", NULL, "_Daily...", NULL, "Show the total time spent on a project, day by day", G_CALLBACK(show_daily_report)},
		{"ReportStatus", NULL, "_Status...", NULL, "Show the project descriptions and notes.", G_CALLBACK(show_status_report)},
		{"ReportTodo", NULL, "_To Do...", NULL, "Show a sample to-do list", G_CALLBACK(show_todo_report)},
		{"ReportInvoice", NULL, "_Invoice...", NULL, "Show a sample invoice for this project", G_CALLBACK(show_invoice_report)},
		{"ReportQuery", NULL, "_Query...", NULL, "Run a sample Query Generator", G_CALLBACK(show_query_report)},
		{"ReportPrimer", NULL, "_Primer...", NULL, "Show a sample introductory primer for designing custom reports", G_CALLBACK(show_primer_report)},
		{"ReportAddNew", NULL, "_New Report...", NULL, "Define a path to a new GnoTime ghtml report file", G_CALLBACK(new_report)},
		{"ReportEditMenu", NULL, "_Edit Reports...", NULL, "Edit the entries in the Reports pulldown menu (this menu)", G_CALLBACK(report_menu_edit)},

		// Timer menu actions
		{"TimerStart", GTK_STOCK_MEDIA_PLAY, "St_art", "<control>S", "Start the timer running", G_CALLBACK (menu_start_timer)},
		{"TimerStop", GTK_STOCK_MEDIA_STOP, "Sto_p", "<control>S", "Stop the timer", G_CALLBACK (menu_start_timer)},

		// Help menu actions
		{"HelpContents", GTK_STOCK_HELP, "_Contents", "F1", "Help contents",NULL},
		{"HelpAbout", GTK_STOCK_ABOUT, "_About", NULL, "About Gnotime",G_CALLBACK (about_box)},

		// Popup menu actions
		{"EditTimes", NULL, "Edit _Times", NULL, "Edit the time interval associated with this project", G_CALLBACK (menu_howto_edit_times)},
		{"NewDiaryEntry", NULL, "_New Diary Entry", NULL, "Change the current task for this project", G_CALLBACK (new_task_ui)},
		{"EditDiaryEntry", NULL, "_Edit Diary Entry", NULL, "Edit task header for this project", G_CALLBACK (edit_task_ui)},

};

/* Toggle items */

static const GtkToggleActionEntry toggle_entries [] = {
	{"TimerToggle", NULL, "_Timer Running", "<control>T", "Toggle the timer", G_CALLBACK(menu_toggle_timer), FALSE}
};



static const char * ui_description =
	"<ui>"
	"<menubar name='MainMenu'>"
	"    <menu action='FileMenu'>"
	"        <menuitem action='ExportTasks'/>"
	"        <menuitem action='ExportProjects'/>"
	"        <menuitem action='Quit'/>"
	"    </menu>"
	"    <menu action='ProjectsMenu'>"
	"        <menuitem action='NewProject'/>"
	"        <separator/>"
	"        <menuitem action='CutProject'/>"
	"        <menuitem action='CopyProject'/>"
	"        <menuitem action='PasteProject'/>"
	"        <separator/>"
	"        <menuitem action='EditTimes'/>"
	"        <menuitem action='ProjectProperties'/>"
	"    </menu>"
	"    <menu action='SettingsMenu'>"
	"        <menuitem action='Preferences'/>"
	"    </menu>"
	"    <menu action='ReportsMenu'>"
	"        <menuitem action='ReportJournal'/>"
	"        <menuitem action='ReportActivity'/>"
	"        <menuitem action='ReportDaily'/>"
	"        <menuitem action='ReportStatus'/>"
	"        <menuitem action='ReportTodo'/>"
	"        <menuitem action='ReportInvoice'/>"
	"        <menuitem action='ReportQuery'/>"
	"        <menuitem action='ReportPrimer'/>"
	"        <menuitem action='ReportAddNew'/>"
	"        <menuitem action='ReportEditMenu'/>"
	"    </menu>"
	"    <menu action='TimerMenu'>"
	"        <menuitem action='TimerStart'/>"
	"        <menuitem action='TimerStop'/>"
	"        <menuitem action='TimerToggle'/>"
	"    </menu>"
	"    <menu action='HelpMenu'>"
	"        <menuitem action='HelpContents'/>"
	"        <menuitem action='HelpAbout'/>"
	"    </menu>"
	"</menubar>"
	"<popup name='PopupMenu'>"
	"    <menuitem action='ReportActivity'/>"
	"    <menuitem action='EditTimes'>"
	"    <menuitem action='NewDiaryEntry'/>"
	"    <menuitem action='EditDiaryEntry'/>"
	"    <separator/>"
	"    <menuitem action='CutProject'/>"
	"    <menuitem action='CopyProject'/>"
	"    <menuitem action='PasteProject'/>"
	"    <separator/>"
	"    <menuitem action='ProjectProperties'/>"
	"</popup>"
	"</ui>"
;



/*
static GnomeUIInfo menu_main_file[] = {
	{GNOME_APP_UI_ITEM, N_("_Export Tasks"), NULL,
		export_file_picker,  TAB_DELIM_EXPORT, NULL,
		GNOME_APP_PIXMAP_STOCK, GTK_STOCK_SAVE,
		'E', GDK_CONTROL_MASK, NULL},
	{GNOME_APP_UI_ITEM, N_("Export _Projects"), NULL,
		export_file_picker, TODO_EXPORT, NULL,
		GNOME_APP_PIXMAP_STOCK, GTK_STOCK_SAVE,
		'P', GDK_CONTROL_MASK, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EXIT_ITEM(app_quit,NULL),
	GNOMEUIINFO_END
};

*/
/* Insert an item with a stock icon and a user data pointer */
/*
#define GNOMEUIINFO_ITEM_STOCK_DATA(label, tooltip, callback, user_data, stock_id) \
   { GNOME_APP_UI_ITEM, label, tooltip, (gpointer)callback, user_data, NULL, \
     GNOME_APP_PIXMAP_STOCK, stock_id, 0, (GdkModifierType) 0, NULL }

static GnomeUIInfo menu_main_projects[] = {
  GNOMEUIINFO_MENU_NEW_ITEM(N_("_New ..."), NULL,
				  new_project, NULL),
	GNOMEUIINFO_SEPARATOR,
#define MENU_PROJECTS_CUT_POS 2
  {
    GNOME_APP_UI_ITEM,
    N_("Cu_t"),
    N_("Delete the selected project"),
    cut_project,
    NULL,
    NULL,
    GNOME_APP_PIXMAP_STOCK,
    GNOME_STOCK_PIXMAP_CUT,
    'D',
    GDK_CONTROL_MASK,
    NULL
  },
#define MENU_PROJECTS_COPY_POS 3
  {
    GNOME_APP_UI_ITEM,
    N_("_Copy"),
    N_("Copy the selected project"),
    copy_project,
    NULL,
    NULL,
    GNOME_APP_PIXMAP_STOCK,
    GNOME_STOCK_PIXMAP_COPY,
    'F',
    GDK_CONTROL_MASK,
    NULL
  },
#define MENU_PROJECTS_PASTE_POS 4
  {
    GNOME_APP_UI_ITEM,
    N_("_Paste"),
    N_("Paste the previously copied project"),
    paste_project,
    NULL,
    NULL,
    GNOME_APP_PIXMAP_STOCK,
    GNOME_STOCK_PIXMAP_PASTE,
    'G',
    GDK_CONTROL_MASK,
    NULL
  },
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Edit _Times"),
		N_("Edit the time interval associated with this project"),
			       menu_howto_edit_times,
			       GNOME_STOCK_BLANK),
#define MENU_PROJECTS_PROP_POS 7
	GNOMEUIINFO_MENU_PROPERTIES_ITEM(menu_properties,NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo menu_main_settings[] = {
	GNOMEUIINFO_MENU_PREFERENCES_ITEM(menu_options,NULL),
	GNOMEUIINFO_END
};


static GnomeUIInfo menu_main_reports[] = {
	GNOMEUIINFO_ITEM_STOCK_DATA(N_("_Journal..."),
		N_("Show the journal for this project"),
			       show_report, JOURNAL_REPORT,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK_DATA(N_("_Activity..."),
		N_("Show the journal together with the timestamps for this project"),
			       show_report, ACTIVITY_REPORT,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK_DATA(N_("_Daily..."),
		N_("Show the total time spent on a project, day by day"),
			       show_report, DAILY_REPORT,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK_DATA(N_("_Status..."),
		N_("Show the project descriptions and notes."),
			       show_report, STATUS_REPORT,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK_DATA(N_("_To Do..."),
		N_("Show a sample to-do list"),
			       show_report, TODO_REPORT,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK_DATA(N_("_Invoice..."),
		N_("Show a sample invoice for this project"),
			       show_report, INVOICE_REPORT,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK_DATA(N_("_Query..."),
		N_("Run a sample Query Generator"),
			       show_report, QUERY_REPORT,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK_DATA(N_("_Primer..."),
		N_("Show a sample introductory primer for designing custom reports"),
			       show_report, PRIMER_REPORT,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK(N_("_New Report..."),
		N_("Define a path to a new GnoTime ghtml report file"),
			       new_report,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK(N_("_Edit Reports..."),
		N_("Edit the entries in the Reports pulldown menu (this menu)"),
			       report_menu_edit,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};



static GnomeUIInfo menu_main_timer[] = {
#define MENU_TIMER_START_POS 0
	{
    GNOME_APP_UI_ITEM,
    N_("St_art"),
    N_("Start the timer running"),
    menu_start_timer,
    NULL,
    NULL,
    GNOME_APP_PIXMAP_STOCK,
    GNOME_STOCK_TIMER,
    'S',
    GDK_CONTROL_MASK,
    NULL
  },
#define MENU_TIMER_STOP_POS 1
  {
    GNOME_APP_UI_ITEM,
    N_("Sto_p"),
    N_("Stop the timer"),
    menu_stop_timer,
    NULL,
    NULL,
    GNOME_APP_PIXMAP_STOCK,
    GNOME_STOCK_TIMER_STOP,
    'W',
    GDK_CONTROL_MASK,
    NULL
  },
#define MENU_TIMER_TOGGLE_POS 2
  {
    GNOME_APP_UI_TOGGLEITEM,
    N_("_Timer Running"),
    NULL,
    menu_toggle_timer,
    NULL,
    NULL,
    GNOME_APP_PIXMAP_NONE,
    NULL,
    'T',
    GDK_CONTROL_MASK,
    NULL
  },
  GNOMEUIINFO_END
};


static GnomeUIInfo menu_main_help[] = {
	GNOMEUIINFO_HELP("gnotime"),
	GNOMEUIINFO_MENU_ABOUT_ITEM(about_box,NULL),
	GNOMEUIINFO_END
};


static GnomeUIInfo menu_main[] = {
	GNOMEUIINFO_MENU_FILE_TREE(menu_main_file),
	GNOMEUIINFO_SUBTREE(N_("_Projects"), menu_main_projects),
	GNOMEUIINFO_MENU_SETTINGS_TREE(menu_main_settings),
	GNOMEUIINFO_SUBTREE(N_("_Reports"), menu_main_reports),
	GNOMEUIINFO_SUBTREE(N_("_Timer"), menu_main_timer),
	GNOMEUIINFO_MENU_HELP_TREE(menu_main_help),
	GNOMEUIINFO_END
};



static GnomeUIInfo menu_popup[] = {
#define MENU_POPUP_JNL_POS 0
	GNOMEUIINFO_ITEM_STOCK_DATA(N_("_Activity..."),
		N_("Show the timesheet journal for this project"),
			       show_report, ACTIVITY_REPORT,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK(N_("Edit _Times"),
		N_("Edit the time interval associated with this project"),
			       menu_howto_edit_times,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK(N_("_New Diary Entry"),
		N_("Change the current task for this project"),
			       new_task_ui,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_ITEM_STOCK(N_("_Edit Diary Entry"),
		N_("Edit task header for this project"),
			       edit_task_ui,
			       GNOME_STOCK_BLANK),
	GNOMEUIINFO_SEPARATOR,
#define MENU_POPUP_CUT_POS 5
	GNOMEUIINFO_MENU_CUT_ITEM(cut_project,NULL),
#define MENU_POPUP_COPY_POS 6
	GNOMEUIINFO_MENU_COPY_ITEM(copy_project,NULL),
#define MENU_POPUP_PASTE_POS 7
	GNOMEUIINFO_MENU_PASTE_ITEM(paste_project,NULL),
	GNOMEUIINFO_SEPARATOR,
#define MENU_POPUP_PROP_POS 9
	GNOMEUIINFO_MENU_PROPERTIES_ITEM(menu_properties,NULL),
	GNOMEUIINFO_END
};

*/


GtkMenuShell *
menus_get_popup(void)
{
	return GTK_MENU_SHELL (popup_menu);
}



GtkWidget *
menus_create(GtkWindow *window)
{
	GtkActionGroup *action_group;
	GtkWidget      *menubar;
	GtkAccelGroup  *accel_group;
	GtkUIManager   *ui_manager;
	GError         *error;



	action_group = gtk_action_group_new ("MenuActions");
	gtk_action_group_add_actions (action_group, entries, G_N_ELEMENTS (entries), window);
	gtk_action_group_add_toggle_actions (action_group, toggle_entries, G_N_ELEMENTS (toggle_entries), window);

	ui_manager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);

	accel_group = gtk_ui_manager_get_accel_group (ui_manager);
	gtk_window_add_accel_group (window, accel_group);

	error = NULL;

	if (!gtk_ui_manager_add_ui_from_string (ui_manager, ui_description, -1, &error))
	{
		g_message ("building menus failed: %s", error->message);
		g_error_free (error);
		exit (1);
	}

	menubar = gtk_ui_manager_get_widget (ui_manager, "/MainMenu");
	popup_menu = GTK_MENU (gtk_ui_manager_get_widget (ui_manager, "/PopupMenu"));

	return menubar;
}

/* Global: the user-defined reports pull-down menu */
static GnomeUIInfo *reports_menu = NULL;

GnomeUIInfo *
gtt_get_reports_menu (void)
{
	return (reports_menu);
}

void
gtt_set_reports_menu (GnomeApp *app, GnomeUIInfo *new_menus)
{
	int i;
	char * path;

	/* Build the i18n menu path ... */
	/* (is this right ??? or is this pre-i18n ???) */
	path = g_strdup_printf ("%s/<Separator>", _("Reports"));

	/* If there are old menu items, remove them and free them. */
	if (reports_menu)
	{
		int nreports;
		for (i=0; GNOME_APP_UI_ENDOFINFO != reports_menu[i].type; i++) {}
		nreports = i;
		gnome_app_remove_menu_range (app, path, 1, nreports);

		if (new_menus != reports_menu)
		{
			for (i=0; i<nreports; i++)
			{
				// XXX can't free this, since 'append' recycles old pointers !!
				// there's probably a minor memory leak here ...
				// gtt_plugin_free(reports_menu[i].user_data);
			}
			g_free (reports_menu);
		}
	}

	/* Now install the new menu items. */
	reports_menu = new_menus;
	if (!reports_menu)
	{
		reports_menu = g_new0 (GnomeUIInfo, 1);
		reports_menu[0].type = GNOME_APP_UI_ENDOFINFO;
	}

	/* fixup */
	for (i=0; GNOME_APP_UI_ENDOFINFO != reports_menu[i].type; i++)
	{
		reports_menu[i].moreinfo = invoke_report;
	}
	gnome_app_insert_menus (app, path, reports_menu);
}

/* ============================================================ */
/* Slide a new menu entry into first place */

void
gtt_reports_menu_prepend_entry (GnomeApp *app, GnomeUIInfo *new_entry)
{
	int i, nitems;
	GnomeUIInfo * current_sysmenu, *new_sysmenu;

	current_sysmenu = gtt_get_reports_menu ();
	for (i=0; GNOME_APP_UI_ENDOFINFO != current_sysmenu[i].type; i++) {}
	nitems = i+1;

	new_sysmenu = g_new0 (GnomeUIInfo, nitems+1);
	new_sysmenu[0] = *new_entry;

	memcpy (&new_sysmenu[1], current_sysmenu, nitems*sizeof(GnomeUIInfo));
	gtt_set_reports_menu (app, new_sysmenu);
}

/* ============================================================ */

void
menus_add_plugins (GnomeApp *app)
{
	gtt_set_reports_menu (app, reports_menu);
}


void
menu_set_states(void)
{
	GtkCheckMenuItem *mi;

	if (!menu_main_timer[MENU_TIMER_START_POS].widget) return;
	gtk_widget_set_sensitive(menu_main_timer[MENU_TIMER_TOGGLE_POS].widget,
				 1);
	mi = GTK_CHECK_MENU_ITEM(menu_main_timer[MENU_TIMER_TOGGLE_POS].widget);
	gtk_check_menu_item_set_active (mi, timer_is_running());

	/* XXX would be nice to change this menu entry to say
	 * 'timer stopped' when the timer is stopped.  But don't
	 * know how to change the menu label in gtk */

	gtk_widget_set_sensitive(menu_main_timer[MENU_TIMER_START_POS].widget,
				 (FALSE == timer_is_running()) );
	gtk_widget_set_sensitive(menu_main_timer[MENU_TIMER_STOP_POS].widget,
				 (timer_is_running()) );
	gtk_widget_set_sensitive(menu_main_projects[MENU_PROJECTS_PASTE_POS].widget,
				 (have_cutted_project()) );

	if (menu_popup[MENU_POPUP_CUT_POS].widget)
	{
		gtk_widget_set_sensitive(menu_popup[MENU_POPUP_PASTE_POS].widget,
				 (have_cutted_project()) );
	}
}

/* ======================= END OF FILE ===================== */

