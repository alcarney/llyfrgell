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

#include "llyfr-search-rg.h"
#include "llyfr-window.h"

struct _LlyfrWindow
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkHeaderBar        *header_bar;
  GtkButton           *search_context;
  GtkSearchEntry      *search_entry;
  GtkMenuButton       *menu_button;
  GtkButton           *add_button;
};

G_DEFINE_TYPE (LlyfrWindow, llyfr_window, GTK_TYPE_APPLICATION_WINDOW)

static void
search_activated_cb (GtkSearchEntry *self)
{
  gtk_entry_progress_pulse (GTK_ENTRY (self));
  const gchar* query = gtk_editable_get_text (GTK_EDITABLE (self));

  llyfr_search_rg (query);
}

static void
llyfr_window_class_init (LlyfrWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/io/github/swyddfa/Llyfrgell/ui/window.ui");
  gtk_widget_class_bind_template_child (widget_class, LlyfrWindow, header_bar);
  gtk_widget_class_bind_template_child (widget_class, LlyfrWindow, search_context);
  gtk_widget_class_bind_template_child (widget_class, LlyfrWindow, search_entry);
  gtk_widget_class_bind_template_child (widget_class, LlyfrWindow, menu_button);
  gtk_widget_class_bind_template_child (widget_class, LlyfrWindow, add_button);

  gtk_widget_class_bind_template_callback (widget_class, search_activated_cb);
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
}

LlyfrWindow*
llyfr_window_new (GtkApplication *app)
{
  return g_object_new (LLYFR_TYPE_WINDOW,
		                   "application", app,
		                   NULL);
}
