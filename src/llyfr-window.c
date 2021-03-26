/* llyfr-window.c
 *
 * Copyright 2021 Alex Carney
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
 */

#define G_LOG_DOMAIN "llyfr-window"

#include "llyfr-search-bar.h"
#include "llyfr-search-context.h"
#include "llyfr-search-result.h"
#include "llyfr-search-result-view.h"
#include "llyfr-window.h"

struct _LlyfrWindow
{
  GtkApplicationWindow      parent;

  /* Template widgets */
  LlyfrSearchBar            *search_bar;
  GtkListBox                *results_list;
  GtkMenuButton             *menu_button;
  GtkButton                 *add_button;
};

G_DEFINE_TYPE (LlyfrWindow, llyfr_window, GTK_TYPE_APPLICATION_WINDOW)

static void
results_available_cb (LlyfrSearchBar* search_bar, GListModel* results, LlyfrWindow* self)
{
  LlyfrSearchResult *current_result;
  LlyfrSearchResultView *result_view;
  guint num_results;

  num_results = g_list_model_get_n_items (results);
  g_message ("Found %d results!", num_results);

  for (guint i = 0; i < num_results; i++) {
    current_result = LLYFR_SEARCH_RESULT (g_list_model_get_item (results, i));

    result_view = llyfr_search_result_view_new ();
    llyfr_search_result_view_set_result (result_view, current_result);

    gtk_list_box_append (self->results_list, GTK_WIDGET (result_view));
  }
}

static void
llyfr_window_class_init (LlyfrWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  // Why is this needed?
  g_type_ensure(LLYFR_TYPE_SEARCH_BAR);

  gtk_widget_class_set_template_from_resource (widget_class, "/io/github/swyddfa/Llyfrgell/ui/window.ui");
  gtk_widget_class_bind_template_child (widget_class, LlyfrWindow, search_bar);
  gtk_widget_class_bind_template_child (widget_class, LlyfrWindow, results_list);
  gtk_widget_class_bind_template_child (widget_class, LlyfrWindow, menu_button);
  gtk_widget_class_bind_template_child (widget_class, LlyfrWindow, add_button);

}

static void
llyfr_window_init (LlyfrWindow *self)
{
  GtkBuilder* builder;
  GMenuModel* app_menu;

  gtk_widget_init_template (GTK_WIDGET (self));

  builder = gtk_builder_new_from_resource ("/io/github/swyddfa/Llyfrgell/ui/menus.ui");
  app_menu = G_MENU_MODEL (gtk_builder_get_object (builder, "app_menu"));
  gtk_menu_button_set_menu_model (self->menu_button, app_menu);

  g_signal_connect (self->search_bar,
                    "results-available",
                    G_CALLBACK (results_available_cb),
                    self);

  g_object_unref (builder);
}

LlyfrWindow*
llyfr_window_new (GtkApplication *app)
{
  return g_object_new (LLYFR_TYPE_WINDOW,
		                   "application", app,
		                   NULL);
}
