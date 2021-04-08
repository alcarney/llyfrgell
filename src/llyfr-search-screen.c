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
#include "llyfr-search-screen.h"

struct _LlyfrSearchScreen
{
  GtkBox           parent_instance;

  GtkSearchEntry  *search_entry;
  GtkButton       *context_switcher;
  GtkPopover      *context_popover;
};

G_DEFINE_TYPE (LlyfrSearchScreen, llyfr_search_screen, GTK_TYPE_BOX)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
search_cb (LlyfrSearchScreen *self, GtkSearchEntry *search_entry)
{
  g_message ("Search!");
}

static void
switch_context_cb (LlyfrSearchScreen *self, GtkButton *button)
{
  gtk_popover_popup (self->context_popover);
}

LlyfrSearchScreen *
llyfr_search_screen_new (void)
{
  return g_object_new (LLYFR_TYPE_SEARCH_SCREEN, NULL);
}

static void
context_refresh_cb (LlyfrSearchScreen *self, GListStore *contexts)
{
  GListModel *model = G_LIST_MODEL (contexts);
  g_message ("Found %d contexts!", g_list_model_get_n_items (model));
}

void
llyfr_search_screen_set_application (LlyfrSearchScreen *self,
                                     GtkApplication *app)
{
  LlyfrApplication *application = LLYFR_APPLICATION (app);
  g_signal_connect (application,
                    "context-refresh",
                    G_CALLBACK (context_refresh_cb),
                    NULL);
}

static void
llyfr_search_screen_finalize (GObject *object)
{
  G_OBJECT_CLASS (llyfr_search_screen_parent_class)->finalize (object);
}

static void
llyfr_search_screen_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  LlyfrSearchScreen *self = LLYFR_SEARCH_SCREEN (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
llyfr_search_screen_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  LlyfrSearchScreen *self = LLYFR_SEARCH_SCREEN (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
llyfr_search_screen_class_init (LlyfrSearchScreenClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/io/github/swyddfa/Llyfrgell/llyfr-search-screen.ui");
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, search_entry);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, context_switcher);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, context_popover);

  gtk_widget_class_bind_template_callback (widget_class, search_cb);
  gtk_widget_class_bind_template_callback (widget_class, switch_context_cb);

  object_class->finalize = llyfr_search_screen_finalize;
  object_class->get_property = llyfr_search_screen_get_property;
  object_class->set_property = llyfr_search_screen_set_property;
}

static void
llyfr_search_screen_init (LlyfrSearchScreen *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
