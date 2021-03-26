/* llyfr-search-result-view.c
 *
 * Copyright 2021 Alex Carney <alcarneyme@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define G_LOG_DOMAIN "llyfr-search-result-view"

#include "llyfr-search-result.h"
#include "llyfr-search-result-view.h"

struct _LlyfrSearchResultView
{
  GtkBox         parent;

  /* Template widgets */
  GtkLabel      *filepath_label;
  GtkTextView   *text_view;

};

G_DEFINE_TYPE (LlyfrSearchResultView, llyfr_search_result_view, GTK_TYPE_BOX)

LlyfrSearchResultView*
llyfr_search_result_view_new (void)
{
  return g_object_new (LLYFR_TYPE_SEARCH_RESULT_VIEW, NULL);
}


static GtkWidget*
llyfr_search_result_view_get_line_numbers ()
{
  GtkWidget *container = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_margin_end (container, 6);
  gtk_widget_set_margin_top (container, 12);

  for (int i = 1; i < 6; i++) {
    char label[8];
    sprintf (label, "%d", i*i*i);

    GtkWidget *line = gtk_label_new (label);
    gtk_widget_set_halign (line, GTK_ALIGN_END);

    gtk_box_append (GTK_BOX (container), line);
  }

  return container;
}

void
llyfr_search_result_view_set_result (LlyfrSearchResultView *view,
                                     LlyfrSearchResult *result)
{
  GtkTextBuffer *buffer;
  GtkWidget *line_numbers;

  const gchar *filepath;
  GList *lines;

  filepath = llyfr_search_result_get_filepath (result);
  gtk_label_set_text (view->filepath_label, filepath);

  buffer = gtk_text_view_get_buffer (view->text_view);

  lines = llyfr_search_result_get_lines (result);

  for (int i = 0; lines != NULL; lines = lines->next)
  {
    if (i > 0) {
      gtk_text_buffer_insert_at_cursor (buffer, "\n", -1);
    } else {
      i++;
    }

    gtk_text_buffer_insert_at_cursor (buffer, lines->data, -1);
  }

  line_numbers = llyfr_search_result_view_get_line_numbers ();
  gtk_text_view_set_gutter (view->text_view, GTK_TEXT_WINDOW_LEFT, line_numbers);
}


static void
llyfr_search_result_view_class_init (LlyfrSearchResultViewClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/io/github/swyddfa/Llyfrgell/ui/result_view.ui");
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchResultView, filepath_label);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchResultView, text_view);
}

static void
llyfr_search_result_view_init (LlyfrSearchResultView *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}