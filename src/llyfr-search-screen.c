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

#include "llyfr-search-screen.h"

struct _LlyfrSearchScreen
{
  GtkBox          parent_instance;
};

G_DEFINE_TYPE (LlyfrSearchScreen, llyfr_search_screen, GTK_TYPE_BOX)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];


LlyfrSearchScreen *
llyfr_search_screen_new (void)
{
  return g_object_new (LLYFR_TYPE_SEARCH_SCREEN, NULL);
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

  object_class->finalize = llyfr_search_screen_finalize;
  object_class->get_property = llyfr_search_screen_get_property;
  object_class->set_property = llyfr_search_screen_set_property;
}

static void
llyfr_search_screen_init (LlyfrSearchScreen *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
