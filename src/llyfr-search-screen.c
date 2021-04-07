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

#include "llyfr-search-screen.h"

struct _LlyfrSearchScreen
{
  GtkBox           parent_instance;

  GtkSearchEntry  *search_entry;
  GtkButton       *context_switcher;
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
  g_message ("Context!");
}

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
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, search_entry);
  gtk_widget_class_bind_template_child (widget_class, LlyfrSearchScreen, context_switcher);

  gtk_widget_class_bind_template_callback (widget_class, search_cb);
  gtk_widget_class_bind_template_callback (widget_class, switch_context_cb);

  object_class->finalize = llyfr_search_screen_finalize;
  object_class->get_property = llyfr_search_screen_get_property;
  object_class->set_property = llyfr_search_screen_set_property;
}

static void
populate_search_contexts_cb (GObject      *object,
                             GAsyncResult *result,
                             gpointer      user_data)
{
  LlyfrSearchScreen *self = LLYFR_SEARCH_SCREEN (user_data);
  g_autoptr(GSubprocess) process = G_SUBPROCESS (object);
  g_autoptr(GError) error = NULL;
  g_autofree gchar *stdout_buf = NULL;

  g_message ("Finishing subprocess");

  if (!g_subprocess_communicate_utf8_finish (process, result, &stdout_buf, NULL, &error)) {
    gchar *message = error != NULL ? error->message : "Unable to finish process";
    g_message ("%s", message);

    return;
  }

  g_message ("%s", stdout_buf);
}

static void
llyfr_search_screen_init (LlyfrSearchScreen *self)
{
  g_autoptr(GError) error = NULL;
  GSubprocess *process = NULL;
  const gchar *home_dir = g_get_home_dir ();

  gtk_widget_init_template (GTK_WIDGET (self));

  process = g_subprocess_new (G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error,
                              "flatpak-spawn", "--host",
                              "find", home_dir,
                              "-iname", ".git",
                              "-type", "d",
                              NULL);

  if (process == NULL) {
    gchar *message = error != NULL ? error->message : "Unable to create process";
    g_message ("%s", message);
    return;
  }

  g_message ("Spawning find subprocess");
  g_subprocess_communicate_async (process, NULL, NULL,
                                  populate_search_contexts_cb, self);

  g_message ("Search screen init finished");
}
