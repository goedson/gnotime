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

#include "notes-area.h"
#include "util.h"

struct NotesArea_s
{
	GladeXML *gtxml;
	GtkPaned *vpane;   /* top level pane */
	GtkContainer *ctree_holder;   /* scrolled widget that holds ctree */

	GtkEntry *proj_title;

};

/* ============================================================== */

static void
entry_changed (GtkEntry *entry, NotesArea *na)
{
	printf ("duude text \n");
}


/* ============================================================== */

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


	dlg->proj_title = GTK_ENTRY(glade_xml_get_widget (gtxml, "proj title entry"));
	g_signal_connect (G_OBJECT (dlg->proj_title), "changed",
	                G_CALLBACK (entry_changed), &dlg);
	
	gtk_widget_show (GTK_WIDGET(dlg->vpane));

	return dlg;
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

