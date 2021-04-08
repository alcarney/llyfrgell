/* llyfr-search-context-switcher.c
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
#define G_LOG_DOMAIN "llyfr-search-context-switcher"

#include "llyfr-application.h"
#include "llyfr-search-context.h"
#include "llyfr-search-context-switcher.h"

struct _LlyfrSearchContextSwitcher
{
  GtkBox                   parent_instance;

  GtkSelectionModel       *current_model;
  GtkListItemFactory      *current_factory;

  GtkListView             *context_list;
  GtkSearchEntry          *filter_entry;
  GtkButton               *find_repo_button;
  GtkSpinner              *find_repo_spinner;
};

G_DEFINE_TYPE (LlyfrSearchContextSwitcher, llyfr_search_context_switcher, GTK_TYPE_BOX)

enum {
  SIGNAL_SELECT,
  N_SIGNALS
};

static guint signals[N_SIGNALS] = {0, };

LlyfrSearchContextSwitcher*
llyfr_search_context_switcher_new (void)
{
  return g_object_new (LLYFR_TYPE_SEARCH_CONTEXT_SWITCHER, NULL);
}

static void
setup_listitem_cb (GtkListItemFactory *factory,
                   GtkListItem        *list_item)
{
  GtkWidget *label = gtk_label_new ("");
  gtk_widget_set_halign (GTK_WIDGET (label), GTK_ALIGN_START);

  gtk_list_item_set_child (list_item, label);
}

static void
bind_listitem_cb (GtkListItemFactory *factory,
                  GtkListItem        *list_item)
{
  GtkWidget *label;
  LlyfrSearchContext *context;

  label = gtk_list_item_get_child (list_item);
  context = LLYFR_SEARCH_CONTEXT (gtk_list_item_get_item (list_item));

  gtk_label_set_label (GTK_LABEL (label),
                       llyfr_search_context_get_directory (context));
}

static void
activate_listitem_cb (LlyfrSearchContextSwitcher *self,
                      guint                       position,
                      GtkListView                *list_view,
                      gpointer                    unused)
{
  GListModel *model;
  LlyfrSearchContext *context;

  g_assert (LLYFR_IS_SEARCH_CONTEXT_SWITCHER (self));
  g_assert (GTK_IS_LIST_VIEW (list_view));

  model = G_LIST_MODEL (gtk_list_view_get_model (list_view));
  context = g_list_model_get_item (model, position);

  g_signal_emit (self, signals[SIGNAL_SELECT], 0, context);
}

static void
context_refresh_cb (LlyfrApplication           *app,
                    GListStore                 *contexts,
                    LlyfrSearchContextSwitcher *self)
{

  GListModel *model;

  g_assert (LLYFR_IS_APPLICATION (app));
  g_assert (LLYFR_IS_SEARCH_CONTEXT_SWITCHER (self));

  model = G_LIST_MODEL (g_object_ref (contexts));
  g_debug ("Found %d contexts", g_list_model_get_n_items (model));

  if (self->current_model)
    g_object_unref (self->current_model);

  self->current_model = GTK_SELECTION_MODEL (gtk_single_selection_new (model));

  if (self->current_factory)
    g_object_unref (self->current_factory);

  self->current_factory = gtk_signal_list_item_factory_new ();
  g_signal_connect (self->current_factory, "setup", G_CALLBACK (setup_listitem_cb), NULL);
  g_signal_connect (self->current_factory, "bind", G_CALLBACK (bind_listitem_cb), NULL);

  gtk_list_view_set_model (self->context_list,
                           GTK_SELECTION_MODEL (self->current_model));
  gtk_list_view_set_factory (self->context_list, self->current_factory);
  gtk_widget_set_visible (GTK_WIDGET (self->context_list), TRUE);

  gtk_spinner_stop (self->find_repo_spinner);
  gtk_widget_set_visible (GTK_WIDGET (self->find_repo_spinner), FALSE);
}

void
llyfr_search_context_switcher_set_application (LlyfrSearchContextSwitcher *self,
                                               GtkApplication             *app)
{
  LlyfrApplication *application = LLYFR_APPLICATION (app);
  g_signal_connect (application,
                    "context-refresh",
                    G_CALLBACK (context_refresh_cb),
                    self);
}

static void
find_repos_cb (LlyfrSearchContextSwitcher *self, GtkButton *button)
{
  gtk_widget_set_visible (GTK_WIDGET (self->find_repo_spinner), TRUE);
  gtk_spinner_start (self->find_repo_spinner);
}

static void
llyfr_search_context_switcher_finalize (GObject *object)
{
  LlyfrSearchContextSwitcher *self = LLYFR_SEARCH_CONTEXT_SWITCHER (object);

  g_object_unref (self->current_model);
  g_object_unref (self->current_factory);

  G_OBJECT_CLASS (llyfr_search_context_switcher_parent_class)->finalize (object);
}

static void
llyfr_search_context_switcher_class_init (LlyfrSearchContextSwitcherClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/io/github/swyddfa/Llyfrgell/gui/llyfr-search-context-switcher.ui");
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchContextSwitcher, context_list);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchContextSwitcher, filter_entry);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchContextSwitcher, find_repo_button);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchContextSwitcher, find_repo_spinner);

  gtk_widget_class_bind_template_callback (widget_class, find_repos_cb);
  gtk_widget_class_bind_template_callback (widget_class, activate_listitem_cb);

  object_class->finalize = llyfr_search_context_switcher_finalize;

  signals[SIGNAL_SELECT] = g_signal_new ("select",
                                         LLYFR_TYPE_SEARCH_CONTEXT_SWITCHER,
                                         G_SIGNAL_RUN_LAST,
                                         0,
                                         NULL,
                                         NULL,
                                         NULL,
                                         G_TYPE_NONE,
                                         1,
                                         LLYFR_TYPE_SEARCH_CONTEXT);
}

static void
llyfr_search_context_switcher_init (LlyfrSearchContextSwitcher *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
