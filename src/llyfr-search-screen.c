/* llyfr-search-screen.c
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

#define G_LOG_DOMAIN "llyfr-search-screen"

#include "llyfr-application.h"
#include "llyfr-search-context.h"
#include "llyfr-search-context-switcher.h"
#include "llyfr-search-screen.h"

struct _LlyfrSearchScreen
{
  GtkBox                           parent_instance;

  LlyfrSearchContext              *current_context;

  GtkSearchEntry                  *search_entry;
  GtkButton                       *search_button;

  GtkLabel                        *context_label;
  GtkButton                       *context_switch_button;
  LlyfrSearchContextSwitcher      *context_switcher;
  GtkPopover                      *context_popover;
};

G_DEFINE_TYPE (LlyfrSearchScreen, llyfr_search_screen, GTK_TYPE_BOX)

static void
search_cb (LlyfrSearchScreen *self, GtkSearchEntry *search_entry)
{
  const char* query;

  g_assert (LLYFR_IS_SEARCH_SCREEN (self));
  g_assert (GTK_IS_SEARCH_ENTRY (search_entry));


  query = gtk_editable_get_text (GTK_EDITABLE (search_entry));

  GError* error = NULL;
  GListModel *model = llyfr_search_context_search (self->current_context,
                                                   query, &error);
  if (model == NULL) {
    g_message ("Error while searching: %s", error->message);
    return;
  }

  g_message ("Found %d results!", g_list_model_get_n_items (model));
  //g_signal_emit(self, signals[RESULTS_AVAILABLE], 0, model);
}

static void
search_changed_cb (LlyfrSearchScreen *self, GtkSearchEntry *search_entry)
{
  g_assert (LLYFR_IS_SEARCH_SCREEN (self));
  g_assert (GTK_IS_SEARCH_ENTRY (search_entry));

  const char *text = gtk_editable_get_text (GTK_EDITABLE (search_entry));
  if (strlen (text) > 0) {
    gtk_widget_set_sensitive (GTK_WIDGET (self->search_button), TRUE);
  } else {
    gtk_widget_set_sensitive (GTK_WIDGET (self->search_button), FALSE);
  }
}

static void
switch_context_cb (LlyfrSearchScreen *self, GtkButton *button)
{
  gtk_popover_popup (self->context_popover);
}

static void
select_cb (LlyfrSearchScreen *self,
           LlyfrSearchContext *context,
           LlyfrSearchContextSwitcher *switcher)
{
  g_assert (LLYFR_IS_SEARCH_SCREEN (self));
  g_assert (LLYFR_IS_SEARCH_CONTEXT (context));
  g_assert (LLYFR_IS_SEARCH_CONTEXT_SWITCHER (switcher));

  if (self->current_context)
    g_object_unref (self->current_context);

  self->current_context = g_object_ref (context);

  gtk_label_set_ellipsize (self->context_label, PANGO_ELLIPSIZE_START);
  gtk_label_set_label (self->context_label,
                       llyfr_search_context_get_directory (self->current_context));

  gtk_popover_popdown (self->context_popover);

  gtk_widget_set_sensitive (GTK_WIDGET (self->search_entry), TRUE);
}

LlyfrSearchScreen *
llyfr_search_screen_new (void)
{
  return g_object_new (LLYFR_TYPE_SEARCH_SCREEN, NULL);
}

void
llyfr_search_screen_set_application (LlyfrSearchScreen *self,
                                     GtkApplication *app)
{
  llyfr_search_context_switcher_set_application (self->context_switcher, app);
}

static void
llyfr_search_screen_finalize (GObject *object)
{
  LlyfrSearchScreen *self = LLYFR_SEARCH_SCREEN (object);

  if (self->current_context)
    g_object_unref (self->current_context);

  G_OBJECT_CLASS (llyfr_search_screen_parent_class)->finalize (object);
}

static void
llyfr_search_screen_class_init (LlyfrSearchScreenClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  g_type_ensure (LLYFR_TYPE_SEARCH_CONTEXT_SWITCHER);

  gtk_widget_class_set_template_from_resource (widget_class, "/io/github/swyddfa/Llyfrgell/llyfr-search-screen.ui");
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, search_entry);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, search_button);

  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, context_label);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, context_popover);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, context_switch_button);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, context_switcher);

  gtk_widget_class_bind_template_callback (widget_class, search_cb);
  gtk_widget_class_bind_template_callback (widget_class, select_cb);
  gtk_widget_class_bind_template_callback (widget_class, search_changed_cb);
  gtk_widget_class_bind_template_callback (widget_class, switch_context_cb);

  object_class->finalize = llyfr_search_screen_finalize;
}

static void
llyfr_search_screen_init (LlyfrSearchScreen *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
