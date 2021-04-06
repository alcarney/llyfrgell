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

#include "llyfr-search-match.h"
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
make_line_numbers ()
{
  GtkWidget *container = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_margin_end (container, 6);
  gtk_widget_set_margin_top (container, 6);
  gtk_widget_set_margin_start (container, 6);

  return container;
}

static void
add_line_number (GtkWidget* container, gint64 line_number)
{
    char label[32];
    sprintf (label, "%ld", line_number);

    GtkWidget *line = gtk_label_new (label);
    gtk_widget_set_halign (line, GTK_ALIGN_END);

    gtk_box_append (GTK_BOX (container), line);
}

void
llyfr_search_result_view_set_result (LlyfrSearchResultView *view,
                                     LlyfrSearchResult *result)
{
  GtkTextBuffer *buffer;
  GtkTextTag *highlighted;
  GtkWidget *line_numbers;

  const gchar *filepath;
  GList *matches;

  filepath = llyfr_search_result_get_filepath (result);
  gtk_label_set_text (view->filepath_label, filepath);

  buffer = gtk_text_view_get_buffer (view->text_view);
  highlighted = gtk_text_buffer_create_tag (buffer, "highlighed",
                                            "background", "green",
                                            "foreground", "white",
                                            NULL);

  matches = llyfr_search_result_get_matches (result);

  line_numbers = make_line_numbers ();
  gtk_text_view_set_gutter (view->text_view,
                            GTK_TEXT_WINDOW_LEFT,
                            line_numbers);

  for (; matches != NULL; matches = matches->next)
  {
    GtkTextIter start, end;
    GArray *highlights;

    LlyfrSearchMatch *match = matches->data;
    gint64 line_number = llyfr_search_match_get_line_number (match);
    const gchar *text = llyfr_search_match_get_text (match);

    gtk_text_buffer_insert_at_cursor (buffer, text, -1);
    gtk_text_buffer_insert_at_cursor (buffer, "\n", -1);
    add_line_number (line_numbers, line_number);

    highlights = llyfr_search_match_get_highlights (match);
    if (highlights == NULL || highlights->len == 0)
      continue;

    for (guint i = 0; i < highlights->len; i += 2) {
      gtk_text_buffer_get_end_iter (buffer, &start);
      gtk_text_iter_backward_line(&start);
      end = start;

      gint64 start_offset = g_array_index (highlights, gint64, i);
      gint64 end_offset = g_array_index (highlights, gint64, i + 1);

      gtk_text_iter_forward_chars (&start, start_offset);
      gtk_text_iter_forward_chars (&end, end_offset);

      gtk_text_buffer_apply_tag (buffer, highlighted, &start, &end);
    }
  }

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
