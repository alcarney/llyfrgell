/* llyfr-search-bar.c
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

#include "llyfr-search-bar.h"


struct _LlyfrSearchBar
{
  GtkBox          parent;

  GtkButton      *context_switcher;
  GtkSearchEntry *search_entry;
  GtkPopover     *popover;
};

G_DEFINE_TYPE (LlyfrSearchBar, llyfr_search_bar, GTK_TYPE_BOX)

enum
{
  RESULTS_AVAILABLE,
  N_SIGNALS
};

static guint signals[N_SIGNALS] = { 0, };

static void
context_clicked_cb (LlyfrSearchBar *self, GtkButton *button)
{
  gtk_popover_popup (self->popover);
}

static void
search_cb (LlyfrSearchBar *self, GtkSearchEntry *search_entry)
{
  const char* query;
  LlyfrSearchContext *context;

  context = llyfr_search_context_new ("/home/alex/Code/gnome-builder");
  query = gtk_editable_get_text (GTK_EDITABLE (search_entry));

  GError* error = NULL;
  GListModel *model = llyfr_search_context_search (context, query, &error);
  if (model == NULL)
    {
      g_message ("Error while searching: %s", error->message);
      return;
    }

  g_signal_emit(self, signals[RESULTS_AVAILABLE], 0, model);
}

static void
llyfr_search_bar_class_init (LlyfrSearchBarClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/io/github/swyddfa/Llyfrgell/ui/search_bar.ui");
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchBar, context_switcher);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchBar, search_entry);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchBar, popover);

  gtk_widget_class_bind_template_callback (widget_class, context_clicked_cb);
  gtk_widget_class_bind_template_callback (widget_class, search_cb);

  signals[RESULTS_AVAILABLE] = g_signal_new ("results-available",
                                             LLYFR_TYPE_SEARCH_BAR,
                                             G_SIGNAL_RUN_LAST,
                                             0,
                                             NULL,
                                             NULL,
                                             NULL,
                                             G_TYPE_NONE,
                                             1,
                                             G_TYPE_LIST_MODEL);
}

static void
llyfr_search_bar_init (LlyfrSearchBar *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

LlyfrSearchBar*
llyfr_search_bar_new (void)
{
  return g_object_new (LLYFR_TYPE_SEARCH_BAR,
                       NULL);
}
