/* llyfr-search-page.c
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

#define G_LOG_DOMAIN "llyfr-search-page"

#include "llyfr-search-page.h"

#include "llyfr-search-bar.h"
#include "llyfr-search-result.h"

struct _LlyfrSearchPage
{
  GtkBox              parent_instance;

  GtkSelectionModel  *current_model;
  GtkListItemFactory *current_factory;

  AdwStatusPage      *status_page;
  LlyfrSearchBar     *search_bar;
  GtkScrolledWindow  *results_view;
  GtkListView        *results_list;
};

G_DEFINE_TYPE (LlyfrSearchPage, llyfr_search_page, GTK_TYPE_BOX)

LlyfrSearchPage *
llyfr_search_page_new (void)
{
  return g_object_new (LLYFR_TYPE_SEARCH_PAGE, NULL);
}

void
llyfr_search_page_set_application (LlyfrSearchPage *self,
                                   GtkApplication *app)
{
  llyfr_search_bar_set_application (self->search_bar, app);
}

static void
setup_listitem_cb (GtkListItemFactory *factory,
                   GtkListItem        *list_item)
{
  GtkWidget *filepath = gtk_label_new ("");
  GtkWidget *text_widget = gtk_text_view_new ();
  GtkTextView *text_view = GTK_TEXT_VIEW (text_widget);

  gtk_widget_set_halign (filepath, GTK_ALIGN_START);
  gtk_widget_set_margin_start (filepath, 6);

  gtk_text_view_set_editable (text_view, FALSE);
  gtk_text_view_set_monospace (text_view, TRUE);
  gtk_text_view_set_wrap_mode (text_view, GTK_WRAP_WORD);
  gtk_text_view_set_pixels_below_lines (text_view, 1);
  gtk_text_view_set_left_margin (text_view, 6);
  gtk_text_view_set_top_margin (text_view, 6);
  gtk_text_view_set_bottom_margin (text_view, 6);
  gtk_text_view_set_gutter (text_view, GTK_TEXT_WINDOW_TOP, filepath);

  gtk_widget_set_css_classes (text_widget, (const char* []){"solarized", NULL});

  gtk_list_item_set_child (list_item, text_widget);
}

static void
bind_listitem_cb (GtkListItemFactory *factory,
                  GtkListItem        *list_item)
{

  GtkLabel *filepath;
  GtkWidget *top_gutter;
  GtkTextView *text_view;
  GtkTextBuffer *buffer;
  LlyfrSearchResult *result;

  result = LLYFR_SEARCH_RESULT (gtk_list_item_get_item (list_item));
  buffer = llyfr_search_result_get_text_buffer (result);

  text_view = GTK_TEXT_VIEW (gtk_list_item_get_child (list_item));
  top_gutter = gtk_text_view_get_gutter (text_view, GTK_TEXT_WINDOW_TOP);
  filepath = GTK_LABEL (gtk_widget_get_first_child (top_gutter));

  gtk_label_set_text (filepath, llyfr_search_result_get_filepath (result));
  gtk_text_view_set_buffer (text_view, g_object_ref (buffer));
}

static void
unbind_listitem_cb (GtkListItemFactory *factory,
                  GtkListItem        *list_item)
{
  GtkTextView *text_view;

  text_view = GTK_TEXT_VIEW (gtk_list_item_get_child (list_item));
  gtk_text_view_set_buffer (text_view, NULL);
}

static void
teardown_listitem_cb (GtkListItemFactory *factory,
                      GtkListItem        *list_item)
{
  GtkWidget *text_view =  gtk_list_item_get_child (list_item);

  if (text_view)
    g_object_unref (text_view);
}

static void
search_cb (LlyfrSearchPage *self, GListModel *results, LlyfrSearchBar *search_bar)
{
  g_assert (LLYFR_IS_SEARCH_PAGE (self));
  g_assert (G_IS_LIST_MODEL (results));
  g_assert (LLYFR_IS_SEARCH_BAR (search_bar));

  if (g_list_model_get_n_items (results) == 0) {
    adw_status_page_set_icon_name (self->status_page, "edit-clear");
    adw_status_page_set_title (self->status_page, "No Results");

    gtk_widget_set_visible (GTK_WIDGET (self->results_view), FALSE);
    gtk_widget_set_visible (GTK_WIDGET (self->status_page), TRUE);

    return;
  }

  if (self->current_model)
    g_object_unref (self->current_model);

  if (self->current_factory)
    g_object_unref (self->current_factory);

  self->current_model = GTK_SELECTION_MODEL (gtk_no_selection_new (results));

  self->current_factory = gtk_signal_list_item_factory_new ();
  g_signal_connect (self->current_factory, "setup", G_CALLBACK (setup_listitem_cb), NULL);
  g_signal_connect (self->current_factory, "bind", G_CALLBACK (bind_listitem_cb), NULL);
  g_signal_connect (self->current_factory, "unbind", G_CALLBACK (unbind_listitem_cb), NULL);
  g_signal_connect (self->current_factory, "teardown", G_CALLBACK (teardown_listitem_cb), NULL);

  gtk_list_view_set_model (self->results_list,
                           GTK_SELECTION_MODEL (self->current_model));
  gtk_list_view_set_factory (self->results_list, self->current_factory);

  gtk_widget_set_visible (GTK_WIDGET (self->status_page), FALSE);
  gtk_widget_set_visible (GTK_WIDGET (self->results_view), TRUE);
}

static void
activate_listitem_cb (LlyfrSearchPage *self,
                      guint            position,
                      GtkListView     *list_view,
                      gpointer         unused)
{
  g_assert (LLYFR_IS_SEARCH_PAGE (self));
  g_assert (GTK_IS_LIST_VIEW (list_view));

  g_message ("Activate!");
}

static void
llyfr_search_page_finalize (GObject *object)
{
  LlyfrSearchPage *self = LLYFR_SEARCH_PAGE (object);

  if (self->current_model)
    g_object_unref (self->current_model);

  if (self->current_factory)
    g_object_unref (self->current_factory);

  G_OBJECT_CLASS (llyfr_search_page_parent_class)->finalize (object);
}

static void
llyfr_search_page_class_init (LlyfrSearchPageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  g_type_ensure (LLYFR_TYPE_SEARCH_BAR);

  gtk_widget_class_set_template_from_resource (widget_class, "/io/github/swyddfa/Llyfrgell/gui/llyfr-search-page.ui");
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchPage, search_bar);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchPage, status_page);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchPage, results_view);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchPage, results_list);

  gtk_widget_class_bind_template_callback (widget_class, search_cb);
  gtk_widget_class_bind_template_callback (widget_class, activate_listitem_cb);

  object_class->finalize = llyfr_search_page_finalize;
}

static void
llyfr_search_page_init (LlyfrSearchPage *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
