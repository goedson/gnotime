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

#ifndef GTT_NOTES_AREA_H
#define GTT_NOTES_AREA_H

#include <gnome.h>

typedef struct NotesArea_s NotesArea;

NotesArea * notes_area_new (void);

/* returns the vpaned widget at the top of the notes area heirarchy */
GtkWidget * notes_area_get_widget (NotesArea *na);

/* add the ctree widget to the appropriate location */
void notes_area_add_ctree (NotesArea *na, GtkWidget *ctree);

#endif /* GTT_NOTES_AREA_H */
