/*   Notes Area display of project notes for GTimeTracker
 *   Copyright (C) 2003 Linas Vepstas <linas@linas.org>
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

#include "config.h"

#include <glade/glade.h>
#include <gnome.h>

#include "proj.h"
#include "notes-area.h"
#include "util.h"

struct NotesArea_s
{
	GladeXML *gtxml;
	GtkPaned *vpane;   /* top level pane */
	GtkContainer *ctree_holder;   /* scrolled widget that holds ctree */

	GtkEntry *proj_title;
	GtkEntry *proj_desc;

	GttProject *proj;
};

/* ============================================================== */

static void
proj_title_changed (GtkEntry *entry, NotesArea *na)
{
	const char * str;
	if (NULL == na->proj) return;
	
	str = gtk_entry_get_text (entry);
	gtt_project_set_title (na->proj, str);
}


/* ============================================================== */

static void
proj_desc_changed (GtkEntry *entry, NotesArea *na)
{
	const char * str;
	if (NULL == na->proj) return;
	
	str = gtk_entry_get_text (entry);
	gtt_project_set_desc (na->proj, str);
}


/* ============================================================== */

#define CONNECT_ENTRY(GLADE_NAME,OBJ_FIELD,CB)  {                 \
	dlg->OBJ_FIELD = GTK_ENTRY(glade_xml_get_widget (gtxml, GLADE_NAME)); \
	g_signal_connect (G_OBJECT (dlg->OBJ_FIELD), "changed",        \
	                G_CALLBACK (CB), dlg);                         \
}


NotesArea *
notes_area_new (void)
{
	NotesArea *dlg;
	GladeXML *gtxml;

	dlg = g_new0 (NotesArea, 1);

	gtxml = gtt_glade_xml_new ("glade/notes.glade", "top window");
	dlg->gtxml = gtxml;
	
	dlg->vpane = GTK_PANED(glade_xml_get_widget (gtxml, "notes vpane"));
	dlg->ctree_holder = GTK_CONTAINER(glade_xml_get_widget (gtxml, "ctree holder"));

	CONNECT_ENTRY ("proj title entry", proj_title, proj_title_changed);
	CONNECT_ENTRY ("proj desc entry", proj_desc,  proj_desc_changed);
	
	gtk_widget_show (GTK_WIDGET(dlg->vpane));

	dlg->proj = NULL;

	return dlg;
}

/* ============================================================== */

static void
notes_area_do_set_project (NotesArea *na, GttProject *proj)
{
	const char * str;
	
	if (!na) return;

	if (!proj) return; // xxx should clear fields instead

	na->proj = proj;
	
	str = gtt_project_get_title (proj);
	gtk_entry_set_text (na->proj_title, str);
	
	str = gtt_project_get_desc (proj);
	gtk_entry_set_text (na->proj_desc, str);
}

/* ============================================================== */

static void
redraw (GttProject *prj, gpointer data)
{
	NotesArea *na = data;
	notes_area_do_set_project (na, prj);
}

/* ============================================================== */

void
notes_area_set_project (NotesArea *na, GttProject *proj)
{
	gtt_project_remove_notifier (na->proj, redraw, na);
	notes_area_do_set_project (na, proj);
	gtt_project_add_notifier (proj, redraw, na);
}

/* ============================================================== */

GtkWidget *
notes_area_get_widget (NotesArea *nadlg)
{
	if (!nadlg) return NULL;
	return GTK_WIDGET(nadlg->vpane);
}

void 
notes_area_add_ctree (NotesArea *nadlg, GtkWidget *ctree)
{
	if (!nadlg) return;

	gtk_container_add (nadlg->ctree_holder, ctree);
	gtk_widget_show_all (GTK_WIDGET(nadlg->ctree_holder));
}

/* ========================= END OF FILE ======================== */

