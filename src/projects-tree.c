/*********************************************************************
 *                
 * Copyright (C) 2007,  Goedson Teixeira Paixao
 *                
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *                
 * Filename:      projects-tree.c
 * Author:        Goedson Teixeira Paixao <goedson@debian.org>
 * Description:   Implementation of the ProjectsTree component for
 *                Gnotime
 *                
 * Created at:    Thu Nov 22 18:23:49 2007
 * Modified at:   Sat Dec  1 17:56:41 2007
 * Modified by:   Goedson Teixeira Paixao <goedson@debian.org>
 ********************************************************************/

#include <glib/gi18n.h>


#include "projects-tree.h"
#include "timer.h"

#define GTT_PROJECTS_TREE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTT_TYPE_PROJECTS_TREE, GttProjectsTreePrivate))

static void gtt_projects_tree_finalize (GObject *obj);


typedef struct _ColumnDefinition {
	gchar            *name;
	gint             model_column;
	GtkCellRenderer  *renderer;
	gchar            *value_property_name;
	gchar            *label;
} ColumnDefinition;

/* Columns for the model */
typedef enum {
	/* data columns */
	TIME_EVER_COLUMN,
	TIME_YEAR_COLUMN,
	TIME_MONTH_COLUMN,
	TIME_WEEK_COLUMN,
	TIME_LASTWEEK_COLUMN,
	TIME_YESTERDAY_COLUMN,
	TIME_TODAY_COLUMN,
	TIME_TASK_COLUMN,
	TITLE_COLUMN,
	DESCRIPTION_COLUMN,
	TASK_COLUMN,
	ESTIMATED_START_COLUMN,
	ESTIMATED_END_COLUMN,
	DUE_DATE_COLUMN,
	SIZING_COLUMN,
	PERCENT_COLUMN,
	URGENCY_COLUMN,
	IMPORTANCE_COLUMN,
	STATUS_COLUMN,
	/* row configuration colums */
	BACKGROUND_COLOR_COLUMN,        /* Custom background color */
	WEIGHT_COLUMN,
	/* pointer to the project structure */
	GTT_PROJECT_COLUMN,
	/* total number of columns in model */
	NCOLS
} ModelColumn;

#define N_VIEWABLE_COLS (NCOLS - 3)

typedef struct _GttProjectsTreePrivate GttProjectsTreePrivate;



struct _GttProjectsTreePrivate {
	GtkCellRenderer  *text_renderer;
	GtkCellRenderer  *date_renderer;
	GtkCellRenderer  *time_renderer;
	GtkCellRenderer  *progress_renderer;
	gchar            *active_bgcolor;
	gboolean         show_seconds;
	gboolean         highlight_active;
	ColumnDefinition column_definitions[N_VIEWABLE_COLS];
	GTree            *row_references;
};



static void
gtt_projects_tree_create_model (GttProjectsTree *gpt)
{
	GtkTreeStore *tree_model;
	tree_model = gtk_tree_store_new (NCOLS,
									 G_TYPE_STRING,      /* TIME_EVER_COLUMN */
									 G_TYPE_STRING,      /* TIME_YEAR_COLUMN */
									 G_TYPE_STRING,      /* TIME_MONTH_COLUMN */
									 G_TYPE_STRING,      /* TIME_WEEK_COLUMN */
									 G_TYPE_STRING,      /* TIME_LASTWEEK_COLUMN */
									 G_TYPE_STRING,      /* TIME_YESTERDAY_COLUMN */
									 G_TYPE_STRING,      /* TIME_TODAY_COLUMN */
									 G_TYPE_STRING,      /* TIME_TASK_COLUMN */
									 G_TYPE_STRING,      /* TITLE_COLUMN */
									 G_TYPE_STRING,      /* DESCRIPTION_COLUMN */
									 G_TYPE_STRING,      /* TASK_COLUMN */
									 G_TYPE_STRING,      /* ESTIMATED_START_COLUMN */
									 G_TYPE_STRING,      /* ESTIMATED_END_COLUMN */
									 G_TYPE_STRING,      /* DUE_DATE_COLUMN */
									 G_TYPE_INT,      /* SIZING_COLUMN */
									 G_TYPE_INT,      /* PERCENT_COLUMN */
									 G_TYPE_STRING,      /* URGENCY_COLUMN */
									 G_TYPE_STRING,      /* IMPORTANCE_COLUMN */
									 G_TYPE_STRING,      /* STATUS_COLUMN */
									 G_TYPE_STRING,  /* BACKGROUND_COLOR_COLUMN */
									 G_TYPE_INT,
									 G_TYPE_POINTER   /* GTT_POINTER_COLUMN */
		);
	gtk_tree_view_set_model (GTK_TREE_VIEW (gpt), GTK_TREE_MODEL (tree_model));
}


G_DEFINE_TYPE(GttProjectsTree, gtt_projects_tree, GTK_TYPE_TREE_VIEW)


/* GDataCompareFunc for use in the row_references GTree */
static gint
project_cmp (gconstpointer a, gconstpointer b, gpointer user_data)
{
	GttProject *prj_a = (GttProject *) a;
	GttProject *prj_b = (GttProject *) b;
	return gtt_project_get_id (prj_a) - gtt_project_get_id (prj_b);
}

static void
gtt_projects_tree_init (GttProjectsTree* gpt)
{

	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);


	/* default properties values */
	priv->active_bgcolor = g_strdup("green");
	priv->show_seconds = TRUE;
	priv->highlight_active = TRUE;

	/* references to the rows */

	priv->row_references = g_tree_new_full (project_cmp, NULL, NULL, gtk_tree_row_reference_free);

	/* cell renderers user to render the tree */
	priv->text_renderer = gtk_cell_renderer_text_new ();
	g_object_ref (priv->text_renderer);

	priv->date_renderer = gtk_cell_renderer_text_new ();
	g_object_set (priv->date_renderer, "xalign", 0.5, NULL);
	g_object_ref (priv->date_renderer);

	priv->time_renderer = gtk_cell_renderer_text_new ();
	g_object_set (priv->time_renderer, "xalign", 0.5, NULL);
	g_object_ref (priv->time_renderer);

	priv->progress_renderer = gtk_cell_renderer_progress_new ();
	g_object_ref (priv->progress_renderer);


	/* Definition of the possible data columns to be displayed */
	priv->column_definitions[0].name = "time_ever";
	priv->column_definitions[0].model_column = TIME_EVER_COLUMN;
	priv->column_definitions[0].renderer = priv->time_renderer;
	priv->column_definitions[0].value_property_name = "text";
	priv->column_definitions[0].label = _("Total");

	priv->column_definitions[1].name = "time_year";
	priv->column_definitions[1].model_column = TIME_YEAR_COLUMN;
	priv->column_definitions[1].renderer = priv->time_renderer;
	priv->column_definitions[1].value_property_name = "text";
	priv->column_definitions[1].label = _("Year");

	priv->column_definitions[2].name = "time_month";
	priv->column_definitions[2].model_column = TIME_MONTH_COLUMN;
	priv->column_definitions[2].renderer = priv->time_renderer;
	priv->column_definitions[2].value_property_name = "text";
	priv->column_definitions[2].label = _("Month");

	priv->column_definitions[3].name = "time_week";
	priv->column_definitions[3].model_column = TIME_WEEK_COLUMN;
	priv->column_definitions[3].renderer = priv->time_renderer;
	priv->column_definitions[3].value_property_name = "text";
	priv->column_definitions[3].label = _("Week");

	priv->column_definitions[4].name = "time_lastweek";
	priv->column_definitions[4].model_column = TIME_LASTWEEK_COLUMN;
	priv->column_definitions[4].renderer = priv->time_renderer;
	priv->column_definitions[4].value_property_name = "text";
	priv->column_definitions[4].label = _("Last Week");

	priv->column_definitions[5].name = "time_yesterday";
	priv->column_definitions[5].model_column = TIME_YESTERDAY_COLUMN;
	priv->column_definitions[5].renderer = priv->time_renderer;
	priv->column_definitions[5].value_property_name = "text";
	priv->column_definitions[5].label = _("Yesterday");

	priv->column_definitions[6].name = "time_today";
	priv->column_definitions[6].model_column = TIME_TODAY_COLUMN;
	priv->column_definitions[6].renderer = priv->time_renderer;
	priv->column_definitions[6].value_property_name = "text";
	priv->column_definitions[6].label = _("Today");

	priv->column_definitions[7].name = "time_task";
	priv->column_definitions[7].model_column = TIME_TASK_COLUMN;
	priv->column_definitions[7].renderer = priv->time_renderer;
	priv->column_definitions[7].value_property_name = "text";
	priv->column_definitions[7].label = _("Entry");

	priv->column_definitions[8].name = "title";
	priv->column_definitions[8].model_column = TITLE_COLUMN;
	priv->column_definitions[8].renderer = priv->text_renderer;
	priv->column_definitions[8].value_property_name = "text";
	priv->column_definitions[8].label = _("Title");

	priv->column_definitions[9].name = "description";
	priv->column_definitions[9].model_column = TITLE_COLUMN;
	priv->column_definitions[9].renderer = priv->text_renderer;
	priv->column_definitions[9].value_property_name = "text";
	priv->column_definitions[9].label = _("Description");

	priv->column_definitions[10].name = "task";
	priv->column_definitions[10].model_column = TASK_COLUMN;
	priv->column_definitions[10].renderer = priv->text_renderer;
	priv->column_definitions[10].value_property_name = "text";
	priv->column_definitions[10].label = _("Diary Entry");

	priv->column_definitions[11].name = "estimated_start";
	priv->column_definitions[11].model_column = ESTIMATED_START_COLUMN;
	priv->column_definitions[11].renderer = priv->date_renderer;
	priv->column_definitions[11].value_property_name = "text";
	priv->column_definitions[11].label = _("Start");

	priv->column_definitions[12].name = "estimated_end";
	priv->column_definitions[12].model_column = ESTIMATED_END_COLUMN;
	priv->column_definitions[12].renderer = priv->date_renderer;
	priv->column_definitions[12].value_property_name = "text";
	priv->column_definitions[12].label = _("End");

	priv->column_definitions[13].name = "due_date";
	priv->column_definitions[13].model_column = DUE_DATE_COLUMN;
	priv->column_definitions[13].renderer = priv->date_renderer;
	priv->column_definitions[13].value_property_name = "text";
	priv->column_definitions[13].label = _("Due");

	priv->column_definitions[14].name = "sizing";
	priv->column_definitions[14].model_column = SIZING_COLUMN;
	priv->column_definitions[14].renderer = priv->date_renderer;
	priv->column_definitions[14].value_property_name = "text";
	priv->column_definitions[14].label = _("Size");

	priv->column_definitions[15].name = "percent_done";
	priv->column_definitions[15].model_column = PERCENT_COLUMN;
	priv->column_definitions[15].renderer = priv->progress_renderer;
	priv->column_definitions[15].value_property_name = "value";
	priv->column_definitions[15].label = _("Done");
	
	priv->column_definitions[16].name = "urgency";
	priv->column_definitions[16].model_column = URGENCY_COLUMN;
	priv->column_definitions[16].renderer = priv->text_renderer;
	priv->column_definitions[16].value_property_name = "text";
	priv->column_definitions[16].label = _("Urgency");
	
	priv->column_definitions[17].name = "importance";
	priv->column_definitions[17].model_column = IMPORTANCE_COLUMN;
	priv->column_definitions[17].renderer = priv->text_renderer;
	priv->column_definitions[17].value_property_name = "text";
	priv->column_definitions[17].label = _("Importance");
	
	priv->column_definitions[18].name = "status";
	priv->column_definitions[18].model_column = STATUS_COLUMN;
	priv->column_definitions[18].renderer = priv->text_renderer;
	priv->column_definitions[18].value_property_name = "text";
	priv->column_definitions[18].label = _("Status");

	gtt_projects_tree_create_model (gpt);
	gtt_projects_tree_set_visible_columns (gpt, NULL);

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (gpt), TRUE);
	gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (gpt), TRUE);
	gtk_tree_view_set_show_expanders (GTK_TREE_VIEW (gpt), TRUE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (gpt), TRUE);
	gtk_tree_view_set_reorderable (GTK_TREE_VIEW (gpt), TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW (gpt), TRUE);
}

static void
gtt_projects_tree_finalize (GObject *obj)
{
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (obj);
	g_object_unref (priv->text_renderer);
	g_object_unref (priv->date_renderer);
	g_object_unref (priv->time_renderer);
	g_object_unref (priv->progress_renderer);
	g_tree_destroy (priv->row_references);
}

GttProjectsTree *
gtt_projects_tree_new (void)
{
	return g_object_new (gtt_projects_tree_get_type(), NULL);
}

static void
gtt_projects_tree_class_init (GttProjectsTreeClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = gtt_projects_tree_finalize;
	g_type_class_add_private (object_class, sizeof (GttProjectsTreePrivate));
}

static void
gtt_projects_tree_set_time_value (GttProjectsTree *gpt, GtkTreeStore *tree_model, GtkTreeIter *iter, gint column, gint value)
{

	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);
	char buff[13];
	memset(buff, 0, 13);
	/* Format time and set column value */
	if (value == 0)
	{
		buff[0] = '-';
	}
	else
	{
		if (priv->show_seconds)
		{
			sprintf (buff, "%02d:%02d:%02d", value / 3600, (value / 60) % 60, value % 60);
		}
		else
		{
			sprintf (buff, "%02d:%02d", value / 3600, (value / 60) % 60);
		}
	}
	gtk_tree_store_set (tree_model, iter, column, buff, -1);
}


static void
gtt_projects_tree_set_date_value (GttProjectsTree *gpt, GtkTreeStore *tree_model, GtkTreeIter *iter, gint column, time_t value)
{
	gchar buff[100];
	memset(buff, 0, 100);
	if (value > -1)
	{
		strftime (buff, 100, "%x", localtime(&value));
	}
	else
	{
		buff[0] = '-';
	}
	gtk_tree_store_set (tree_model, iter, column, buff, -1);
}


static void
gtt_projects_tree_set_project_times (GttProjectsTree *gpt, GtkTreeStore *tree_model, GttProject *prj, GtkTreeIter *iter)
{
	gtt_projects_tree_set_time_value (gpt, tree_model, iter, TIME_EVER_COLUMN, gtt_project_get_secs_ever (prj));
	gtt_projects_tree_set_time_value (gpt, tree_model, iter, TIME_YEAR_COLUMN, gtt_project_get_secs_year (prj));
	gtt_projects_tree_set_time_value (gpt, tree_model, iter, TIME_MONTH_COLUMN, gtt_project_get_secs_month (prj));
	gtt_projects_tree_set_time_value (gpt, tree_model, iter, TIME_WEEK_COLUMN, gtt_project_get_secs_week (prj));
	gtt_projects_tree_set_time_value (gpt, tree_model, iter, TIME_LASTWEEK_COLUMN, gtt_project_get_secs_lastweek (prj));
	gtt_projects_tree_set_time_value (gpt, tree_model, iter, TIME_YESTERDAY_COLUMN, gtt_project_get_secs_yesterday (prj));
	gtt_projects_tree_set_time_value (gpt, tree_model, iter, TIME_TODAY_COLUMN, gtt_project_get_secs_day (prj));
	gtt_projects_tree_set_time_value (gpt, tree_model, iter, TIME_TASK_COLUMN, gtt_project_get_secs_current (prj));
}


static void
gtt_projects_tree_set_project_dates (GttProjectsTree *gpt, GtkTreeStore *tree_model, GttProject *prj, GtkTreeIter *iter)
{
	gtt_projects_tree_set_date_value (gpt, tree_model, iter, ESTIMATED_START_COLUMN, gtt_project_get_estimated_start (prj));
	gtt_projects_tree_set_date_value (gpt, tree_model, iter, ESTIMATED_END_COLUMN, gtt_project_get_estimated_end (prj));
	gtt_projects_tree_set_date_value (gpt, tree_model, iter, DUE_DATE_COLUMN, gtt_project_get_due_date (prj));
}

static void
gtt_projects_tree_set_style (GttProjectsTree *gpt, GtkTreeStore *tree_model, GttProject *prj, GtkTreeIter *iter)
{
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);
	PangoWeight weight = PANGO_WEIGHT_NORMAL;
	gchar *bgcolor = NULL;

	if (priv->highlight_active)
	{
		if (timer_project_is_running (prj))
		{
			bgcolor = priv->active_bgcolor;
			weight = PANGO_WEIGHT_BOLD;
		}
	}
	gtk_tree_store_set (tree_model,
						iter,
						BACKGROUND_COLOR_COLUMN, bgcolor,
						WEIGHT_COLUMN, weight,
						-1);
}

static void
gtt_projects_tree_set_project_urgency (GttProjectsTree *gpt, GtkTreeStore *tree_model, GttProject *prj, GtkTreeIter *iter)
{
	gchar *value;
	switch (gtt_project_get_urgency (prj)) {
	case GTT_UNDEFINED: value = "-"; break;
	case GTT_LOW:    value = _("Low"); break;
	case GTT_MEDIUM: value = _("Med"); break;
	case GTT_HIGH:   value = _("High"); break;
	}


	gtk_tree_store_set (tree_model,
						iter,
						URGENCY_COLUMN, value,
						-1);
}

static void
gtt_projects_tree_set_project_importance (GttProjectsTree *gpt, GtkTreeStore *tree_model, GttProject *prj, GtkTreeIter *iter)
{
	gchar *value;
	switch (gtt_project_get_importance (prj)) {
	case GTT_UNDEFINED: value = "-"; break;
	case GTT_LOW:    value = _("Low"); break;
	case GTT_MEDIUM: value = _("Med"); break;
	case GTT_HIGH:   value = _("High"); break;
	}

	gtk_tree_store_set (tree_model,
						iter,
						IMPORTANCE_COLUMN, value,
						-1);
}

static void
gtt_projects_tree_set_project_status (GttProjectsTree *gpt, GtkTreeStore *tree_model, GttProject *prj, GtkTreeIter *iter)
{

	gchar *value;
	switch (gtt_project_get_status (prj)) {
	case GTT_NO_STATUS:   value = "-"; break;
	case GTT_NOT_STARTED: value = _("Not Started"); break;
	case GTT_IN_PROGRESS: value = _("In Progress"); break;
	case GTT_ON_HOLD:     value = _("On Hold"); break;
	case GTT_CANCELLED:   value = _("Cancelled"); break;
	case GTT_COMPLETED:   value = _("Completed"); break;
	}

	gtk_tree_store_set (tree_model,
						iter,
						STATUS_COLUMN, value,
						-1);
}

static void
gtt_projects_tree_set_project_data (GttProjectsTree *gpt, GtkTreeStore *tree_model, GttProject *prj, GtkTreeIter *iter)
{
	gtk_tree_store_set (tree_model,
						iter,
						TITLE_COLUMN, gtt_project_get_title (prj),
						DESCRIPTION_COLUMN, gtt_project_get_desc (prj),
						TASK_COLUMN, gtt_task_get_memo (gtt_project_get_first_task (prj)),
						SIZING_COLUMN, gtt_project_get_sizing (prj),
						PERCENT_COLUMN, gtt_project_get_percent_complete (prj),
						GTT_PROJECT_COLUMN, prj,
						-1);
	gtt_projects_tree_set_project_times (gpt, tree_model, prj, iter);
	gtt_projects_tree_set_project_dates (gpt, tree_model, prj, iter);
	gtt_projects_tree_set_style (gpt, tree_model, prj, iter);
	gtt_projects_tree_set_project_urgency (gpt, tree_model, prj, iter);
	gtt_projects_tree_set_project_importance (gpt, tree_model, prj, iter);
	gtt_projects_tree_set_project_status (gpt, tree_model, prj, iter);
}

static void
gtt_projects_tree_add_project (GttProjectsTree *gpt, GtkTreeStore *tree_model, GttProject *prj, GtkTreeIter *parent, GtkTreePath *path, gboolean recursive)
{
	GtkTreeIter iter;
	GList *node;
	GtkTreePath *child_path;
	GtkTreeRowReference *row_reference;
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);
	
	gtk_tree_store_append (tree_model, &iter, parent);
	gtt_projects_tree_set_project_data (gpt, tree_model, prj, &iter);
	row_reference = gtk_tree_row_reference_new ( GTK_TREE_MODEL(tree_model), path);
	g_tree_insert (priv->row_references, prj, row_reference);
	if (recursive)
	{
		child_path = gtk_tree_path_copy (path);
		gtk_tree_path_down (child_path);
		for (node = gtt_project_get_children (prj); node; node = node->next, gtk_tree_path_next (child_path))
		{
			GttProject *sub_prj = node->data;
			gtt_projects_tree_add_project(gpt, tree_model, sub_prj, &iter, child_path, recursive);
		}
			gtk_tree_path_free (child_path);
	}
}


static void
gtt_projects_tree_populate_tree_store (GttProjectsTree *gpt,
									   GtkTreeStore *tree_model,
									   GList *prj_list,
									   gboolean recursive)
{
	GList *node;
	GtkTreePath *path = gtk_tree_path_new_first ();
	if (prj_list)
	{
		for (node = prj_list; node; node = node->next, gtk_tree_path_next (path))
		{
			GttProject *prj = node->data;
			gtt_projects_tree_add_project(gpt, tree_model, prj, NULL, path, recursive);
		}
	}
	gtk_tree_path_free (path);

}

void
gtt_projects_tree_populate (GttProjectsTree *proj_tree,
							GList *plist,
							gboolean recursive)
{
	GtkTreeStore *tree_model = GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (proj_tree)));

	gtk_tree_store_clear (tree_model);
	gtt_projects_tree_populate_tree_store (proj_tree, tree_model, plist, recursive);
	gtk_tree_view_set_model (GTK_TREE_VIEW (proj_tree), GTK_TREE_MODEL(tree_model));
}


static void
gtt_projects_tree_add_column (GttProjectsTree *project_tree, gchar *column_name)
{

	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (project_tree);
	int index;
	GtkTreeViewColumn *column;

	for (index = 0; index < N_VIEWABLE_COLS; index ++)
	{
		if (!strcmp (column_name, priv->column_definitions[index].name))
		{
			ColumnDefinition *c = &(priv->column_definitions[index]);
			column = gtk_tree_view_column_new_with_attributes (c->label,
															   c->renderer,
															   c->value_property_name, c->model_column,
															   "cell-background", BACKGROUND_COLOR_COLUMN,
															   NULL);
			if (GTK_IS_CELL_RENDERER_TEXT (c->renderer))
			{
				gtk_tree_view_column_add_attribute (column, c->renderer, "weight", WEIGHT_COLUMN);
			}
			gtk_tree_view_column_set_resizable (column, TRUE);
			gtk_tree_view_append_column (GTK_TREE_VIEW (project_tree), column);
			if (!strcmp(c->name, "title"))
			{
				gtk_tree_view_set_expander_column (GTK_TREE_VIEW (project_tree), column);
			}
			return;
		}
	}
	g_warning ("Illegal column name '%s' requested", column_name);
}

void
gtt_projects_tree_set_visible_columns (GttProjectsTree *project_tree,
									   GList *columns)
{
	GtkTreeViewColumn *column = NULL;
	GtkTreeView * tree_view = GTK_TREE_VIEW (project_tree);
	GList *p;

	/* remove all columns */
	while ((column = gtk_tree_view_get_column (tree_view, 0)))
	{
		gtk_tree_view_remove_column (tree_view, column);
	}


	/* The title column is mandatory. If it's not on the list, 
	   we add it as the first column. */

	if (!g_list_find_custom (columns, "title", strcmp))
	{
		gtt_projects_tree_add_column (project_tree, "title");
	}


	for (p = columns; p; p = p->next)
	{
		gtt_projects_tree_add_column (project_tree, p->data);
	}
}

static void
gtt_projects_tree_update_row_data (GttProjectsTree *gpt, GttProject *prj, GtkTreeRowReference *row_ref)
{
	g_return_if_fail (gtk_tree_row_reference_valid (row_ref));
	GtkTreePath *path = gtk_tree_row_reference_get_path (row_ref);
	if (path)
	{
		GtkTreeStore *tree_model = GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (gpt)));
		
		GtkTreeIter iter;
		if (gtk_tree_model_get_iter (GTK_TREE_MODEL (tree_model), &iter, path))
		{
			gtt_projects_tree_set_project_data (gpt, tree_model, prj, &iter);
		}
		else
		{
			gchar *path_str = gtk_tree_path_to_string (path);
			g_warning ("Invalid path %s", path_str);
			g_free (path_str);
		}
		gtk_tree_path_free (path);
	}
}


void
gtt_projects_tree_update_project_data (GttProjectsTree *gpt, GttProject *prj)
{
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);
	GtkTreeRowReference *row_ref = (GtkTreeRowReference *) g_tree_lookup (priv->row_references, prj);
	
	if (row_ref)
	{
		gtt_projects_tree_update_row_data (gpt, prj, row_ref);
	}
	else
	{
		g_warning ("Updating non-existant project");
	}
}


static gboolean
update_row_data (gpointer key, gpointer value, gpointer data)
{
	GttProject *prj = (GttProject *) key;
	GtkTreeRowReference *row_ref = (GtkTreeRowReference *) value;
	GttProjectsTree *gpt = GTT_PROJECTS_TREE (data);

	gtt_projects_tree_update_row_data (gpt, prj, row_ref);
	return FALSE;
}

void
gtt_projects_tree_update_all_rows (GttProjectsTree *gpt)
{
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);

	g_tree_foreach (priv->row_references, update_row_data, gpt);

}

void
gtt_projects_tree_set_active_bgcolor (GttProjectsTree *gpt, gchar *color)
{
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);

	if (priv->active_bgcolor)
	{
		g_free (priv->active_bgcolor);
	}

	if (color)
	{
		priv->active_bgcolor = g_strdup (color);
	}
	else
	{
		priv->active_bgcolor = NULL;
	}

	gtt_projects_tree_update_all_rows (gpt);

}

gchar *
gtt_projects_tree_get_active_bgcolor (GttProjectsTree *gpt)
{
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);
	return priv->active_bgcolor;
}

void
gtt_projects_tree_set_show_seconds (GttProjectsTree *gpt, gboolean show_seconds)
{
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);
	priv->show_seconds = show_seconds;
	gtt_projects_tree_update_all_rows (gpt);
}

gboolean
gtt_projects_tree_get_show_seconds (GttProjectsTree *gpt)
{
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);
	return priv->show_seconds;
}

void
gtt_projects_tree_set_highlight_active (GttProjectsTree *gpt, gboolean highlight_active)
{
		GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);
		priv->highlight_active = highlight_active;
		gtt_projects_tree_update_all_rows (gpt);
}

gboolean
gtt_projects_tree_get_highlight_active (GttProjectsTree *gpt)
{
	GttProjectsTreePrivate *priv = GTT_PROJECTS_TREE_GET_PRIVATE (gpt);
	return priv->highlight_active;
}


GttProject *
gtt_projects_tree_get_selected_project (GttProjectsTree *gpt)
{
	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (gpt));
		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (gpt));
	GList *list = gtk_tree_selection_get_selected_rows(selection, NULL);
	GttProject *prj = NULL;
	if (list)
	{
		GtkTreePath *path = (GtkTreePath *)list->data;
		GtkTreeIter iter;
		if (gtk_tree_model_get_iter (model, &iter, path))
		{
			gtk_tree_model_get (model, &iter, GTT_PROJECT_COLUMN, &prj, -1);
		}
		g_list_foreach (list, gtk_tree_path_free, NULL);
		g_list_free (list);
	}
	return prj;
}
