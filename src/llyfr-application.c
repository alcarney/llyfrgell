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

#include "llyfr-application.h"
#include "llyfr-window.h"


struct _LlyfrApplication
{
  GtkApplication application;

  GtkWindow      *window;
};

static void llyfr_application_quit (GSimpleAction *simple,
                                    GVariant      *parameter,
                                    gpointer      user_data);

G_DEFINE_TYPE (LlyfrApplication, llyfr_application, GTK_TYPE_APPLICATION)

static const GActionEntry llyfr_application_entries[] = {
    { .name = "quit", .activate = llyfr_application_quit }
};

static void
llyfr_application_quit (GSimpleAction *simple,
                        GVariant      *parameter,
                        gpointer      user_data)
{
  LlyfrApplication *self = LLYFR_APPLICATION (user_data);
  gtk_window_destroy (self->window);
}


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
}

static void
llyfr_application_class_init (LlyfrApplicationClass *klass)
{
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  application_class->activate = llyfr_application_activate;
  application_class->startup = llyfr_application_startup;
}

static void
llyfr_application_init (LlyfrApplication *self)
{

}
