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
	GtkPaned *vpane;
	GtkContainer *ctree_holder;

	GtkEntry *proj_title;

};

/* ============================================================== */

static void
entry_insert_wrapper (GtkEntry *entry, const char *str, NotesArea *na)
{
	printf ("duude insert text %s\n", str);
}

static void
entry_delete_wrapper (GtkEntry *entry, GtkDeleteType t, gint cnt, NotesArea *na)
{
	printf ("duude delete text\n");
}

static void
entry_paste_wrapper (GtkEntry *entry, NotesArea *na)
{
	printf ("duude paste\n");
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
	dlg->ctree_holder = GTK_CONTAINER(glade_xml_get_widget (gtxml, "ctree viewport"));


	dlg->proj_title = GTK_ENTRY(glade_xml_get_widget (gtxml, "proj title entry"));
	g_signal_connect (G_OBJECT (dlg->proj_title), "insert_at_cursor",
	                G_CALLBACK (entry_insert_wrapper), &dlg);
	g_signal_connect (G_OBJECT (dlg->proj_title), "delete_from_cursor",
	                G_CALLBACK (entry_delete_wrapper), &dlg);
	g_signal_connect (G_OBJECT (dlg->proj_title), "paste_clipboard",
	                G_CALLBACK (entry_paste_wrapper), &dlg);
	
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

	/* For some reason, the way paned works, I can't add to the pane
	 * any more after glade has put something in there. So we put
	 * a scrolled window inside a scrolled window.  XXX there must
	 * be a better way */
	/* gtk_paned_pack1 (nadlg->vpane, ctree, TRUE, TRUE); */
	gtk_container_add (nadlg->ctree_holder, ctree);
}

/* ========================= END OF FILE ======================== */

