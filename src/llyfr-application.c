/* llyfr-application.c
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
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#define G_LOG_DOMAIN "llyfr-application"

#include "llyfr-application.h"
#include "llyfr-search-context.h"
#include "llyfr-window.h"


struct _LlyfrApplication
{
  GtkApplication  application;

  GListStore     *search_contexts;

  GtkWindow      *window;
};

G_DEFINE_TYPE (LlyfrApplication, llyfr_application, GTK_TYPE_APPLICATION)

enum
{
  SIGNAL_CONTEXT_REFRESH,
  N_SIGNALS
};

static guint signals[N_SIGNALS] = {0, };

static void
llyfr_application_quit (GSimpleAction *simple,
                        GVariant      *parameter,
                        gpointer       user_data)
{
  LlyfrApplication *self = LLYFR_APPLICATION (user_data);
  gtk_window_destroy (self->window);
}

static void
populate_search_contexts_cb (GObject      *object,
                             GAsyncResult *result,
                             gpointer      user_data)
{
  LlyfrApplication *self = LLYFR_APPLICATION (user_data);
  g_autoptr(GSubprocess) process = G_SUBPROCESS (object);
  g_autoptr(GInputStream) instream = NULL;
  g_autoptr(GDataInputStream) stream = NULL;
  g_autoptr(GError) error = NULL;
  g_autofree gchar *stdout_buf = NULL;

  char* filepath = NULL;
  gsize length = 0;

  if (!g_subprocess_communicate_utf8_finish (process, result, &stdout_buf, NULL, &error)) {
    gchar *message = error != NULL ? error->message : "Unable to finish process";
    g_message ("%s", message);

    return;
  }

  if (!self->search_contexts)
    self->search_contexts = g_list_store_new (LLYFR_TYPE_SEARCH_CONTEXT);

  g_list_store_remove_all (self->search_contexts);

  instream = g_memory_input_stream_new_from_data (stdout_buf, -1, NULL);
  stream = g_data_input_stream_new (instream);

  while ((filepath = g_data_input_stream_read_line_utf8 (stream, &length, NULL, &error))) {
    g_autoptr(LlyfrSearchContext) search_context = NULL;

    g_debug ("%s", filepath);
    search_context = llyfr_search_context_new (filepath);
    g_list_store_append (self->search_contexts, search_context);

    g_free (filepath);
  }

  // TODO: Save this list of contexts somewhere.

  g_signal_emit (self, signals[SIGNAL_CONTEXT_REFRESH], 0, self->search_contexts);
}

static void
llyfr_application_scan_git_repos (GSimpleAction *simple,
                                  GVariant      *parameter,
                                  gpointer       user_data)
{
  LlyfrApplication *self = LLYFR_APPLICATION (user_data);
  g_autoptr(GError) error = NULL;
  GSubprocess *process = NULL;
  const gchar *home_dir = g_get_home_dir ();

  process = g_subprocess_new (G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error,
                              "flatpak-spawn", "--host",
                              "find", home_dir,
                              "-iname", ".git",
                              "-type", "d",
                              "-exec", "bash", "-c",
                              "printf \"%s\n\" ${0:0: -5}", "{}", ";",
                              NULL);

  if (process == NULL) {
    gchar *message = error != NULL ? error->message : "Unable to create process";
    g_message ("%s", message);
    return;
  }

  g_subprocess_communicate_async (process, NULL, NULL,
                                  populate_search_contexts_cb, self);
}

static const GActionEntry llyfr_application_entries[] = {
    { .name = "scan-git-repos", .activate = llyfr_application_scan_git_repos },
    { .name = "quit",           .activate = llyfr_application_quit }
};

LlyfrApplication*
llyfr_application_new (void)
{
  return g_object_new (LLYFR_TYPE_APPLICATION,
                       "application-id", "io.github.swyddfa.Llyfrgell",
                       "flags", G_APPLICATION_FLAGS_NONE,
                       NULL);
}

static void
llyfr_application_activate (GApplication *application)
{
  LlyfrApplication *self = LLYFR_APPLICATION (application);
  gtk_window_present (GTK_WINDOW (self->window));
}

static void
llyfr_application_startup (GApplication *application)
{
  GtkCssProvider *provider;
  GtkApplication *gtk_app = GTK_APPLICATION (application);
  LlyfrApplication *self = LLYFR_APPLICATION (application);


  adw_init ();
  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   llyfr_application_entries,
                                   G_N_ELEMENTS (llyfr_application_entries),
                                   self);

  G_APPLICATION_CLASS (llyfr_application_parent_class)->startup (application);

  provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_resource (provider, "/io/github/swyddfa/Llyfrgell/ui/llyfrgell.css");
  gtk_style_context_add_provider_for_display (gdk_display_get_default (),
                                             GTK_STYLE_PROVIDER (provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  self->window = GTK_WINDOW (llyfr_window_new (gtk_app));

  // TODO: Look to see if there's an existing list of contexts we can load from
  // somewhere.
}

static void
llyfr_application_finalize (GObject *object)
{
  LlyfrApplication *self = LLYFR_APPLICATION (object);

  g_clear_object (&self->search_contexts);

  G_OBJECT_CLASS (llyfr_application_parent_class)->finalize (object);
}

static void
llyfr_application_class_init (LlyfrApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  application_class->activate = llyfr_application_activate;
  application_class->startup = llyfr_application_startup;

  object_class->finalize = llyfr_application_finalize;

  signals[SIGNAL_CONTEXT_REFRESH] = g_signal_new ("context-refresh",
                                                  LLYFR_TYPE_APPLICATION,
                                                  G_SIGNAL_RUN_LAST,
                                                  0,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  G_TYPE_NONE,
                                                  1,
                                                  G_TYPE_LIST_STORE);
}

static void
llyfr_application_init (LlyfrApplication *self)
{

}
