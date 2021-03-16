/* llyfr-search-context.c
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

#include "llyfr-search-context.h"

typedef struct
{
  gchar          *directory;
} LlyfrSearchContextPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (LlyfrSearchContext, llyfr_search_context, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_DIRECTORY,
  LAST_PROP
};

LlyfrSearchContext* llyfr_search_context_new (char* directory)
{
  return g_object_new (LLYFR_TYPE_SEARCH_CONTEXT,
                       "directory", directory,
                       NULL);
}

void llyfr_search_context_search (LlyfrSearchContext *context,
                                  const gchar* query)
{
  GSubprocess *process;
  GError *error = NULL;

  char* line = NULL;
  gsize length = 0;
  GInputStream *stdout_pipe;
  GDataInputStream *data_stream;

  const gchar* search_directory = llyfr_search_context_get_directory (context);

  g_assert (search_directory != NULL);
  process = g_subprocess_new (G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error,
                              "flatpak-spawn", "--host",
                              "rg", "--json",
                              query, search_directory,
                              NULL);

  if (process == NULL)
    {
      g_message ("Unable to create subprocess");
      if (error != NULL)
        {
          g_message ("%s", error->message);
        }
      return;
    }

  stdout_pipe = g_subprocess_get_stdout_pipe (process);
  data_stream = g_data_input_stream_new (stdout_pipe);

  while ((line = g_data_input_stream_read_line (data_stream, &length, NULL, &error)) != NULL)
    {
      g_message ("Read %ld: %s", length, line);
    }
}

const gchar* llyfr_search_context_get_directory (LlyfrSearchContext *context)
{
  LlyfrSearchContextPrivate *priv = llyfr_search_context_get_instance_private (context);

  return priv->directory ? priv->directory : "";
}

void llyfr_search_context_set_directory (LlyfrSearchContext *context,
                                         const gchar *directory)
{
  LlyfrSearchContextPrivate *priv = llyfr_search_context_get_instance_private (context);

  g_clear_pointer (&priv->directory, g_free);
  priv->directory = g_strdup (directory);
}

static void
llyfr_search_context_get_property (GObject    *object,
                                   guint      prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  LlyfrSearchContext *self = LLYFR_SEARCH_CONTEXT (object);

  switch (prop_id)
    {
    case PROP_DIRECTORY:
      g_value_set_string (value, llyfr_search_context_get_directory (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
llyfr_search_context_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec    *pspec)
{
  LlyfrSearchContext *self = LLYFR_SEARCH_CONTEXT (object);

  switch (prop_id)
    {
    case PROP_DIRECTORY:
      llyfr_search_context_set_directory (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

void llyfr_search_context_class_init (LlyfrSearchContextClass *klass)
{

  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = llyfr_search_context_get_property;
  object_class->set_property = llyfr_search_context_set_property;

  g_object_class_install_property (object_class,
                                   PROP_DIRECTORY,
                                   g_param_spec_string ("directory",
                                                        "Search directory",
                                                        "Directory in which to search",
                                                        NULL,
                                                        G_PARAM_READWRITE));

}

void llyfr_search_context_init (LlyfrSearchContext *self)
{
  LlyfrSearchContextPrivate *priv = llyfr_search_context_get_instance_private (self);

  priv->directory = NULL;
}
