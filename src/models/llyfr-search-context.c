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

#define G_LOG_DOMAIN "llyfr-search-context"

#include "llyfr-search-context.h"
#include "llyfr-search-result.h"

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

static gboolean
llyfr_search_context_do_rg_search (LlyfrSearchContext *context,
                                   const gchar *query,
                                   char **output,
                                   GError **error)
{
  GSubprocess *process;
  const gchar *search_directory = llyfr_search_context_get_directory (context);

  g_assert (search_directory != NULL);
  process = g_subprocess_new (G_SUBPROCESS_FLAGS_STDOUT_PIPE, error,
                              "flatpak-spawn", "--host",
                              "rg", "--json",
                              query, search_directory,
                              NULL);

  if (process == NULL)
    return FALSE;

  return g_subprocess_communicate_utf8 (process, NULL, NULL, output, NULL, error);
}

static JsonNode *
llyfr_search_context_parse_object (char*    line,
                                   gsize    length,
                                   GError **error)
{
  g_autoptr (JsonParser) parser = json_parser_new ();
  JsonNode *root = NULL;

  if (!json_parser_load_from_data (parser, line, length, error))
    return NULL;

  root = json_parser_get_root (parser);
  if (root == NULL || !JSON_NODE_HOLDS_OBJECT (root))
    {
      g_set_error (error,
                   JSON_PARSER_ERROR,
                   JSON_PARSER_ERROR_INVALID_DATA,
                   "Expected JSON Object Got: '%s'", line);
      return NULL;
    }

  return json_parser_steal_root (parser);
}

GListModel* llyfr_search_context_search (LlyfrSearchContext *context,
                                         const gchar* query,
                                         GError **error)
{
  g_autoptr(GInputStream) instream = NULL;
  g_autoptr(GDataInputStream) stream = NULL;

  LlyfrSearchResult* current_result = NULL;
  char* line = NULL;
  char* output = NULL;
  gsize length = 0;
  GListStore* results;

  if (!llyfr_search_context_do_rg_search (context, query, &output, error))
    return NULL;

  results = g_list_store_new (LLYFR_TYPE_SEARCH_RESULT);
  instream = g_memory_input_stream_new_from_data (output, -1, g_free);
  stream = g_data_input_stream_new (instream);

  while ((line = g_data_input_stream_read_line (stream, &length, NULL, error)) != NULL)
    {
      g_autoptr (JsonReader) reader = NULL;
      g_autoptr(JsonNode) node = NULL;

      g_debug ("%s", line);
      node = llyfr_search_context_parse_object (line, length, error);
      if (node == NULL)
        {
          g_message ("Unable to parse line '%s'\n%s", line, (*error)->message);
          g_clear_error (error);
          continue;
        }

      reader = json_reader_new (node);

      json_reader_read_member (reader, "type");
      const char* type = json_reader_get_string_value (reader);

      if (g_strcmp0 (type, "begin") == 0)
        {
          current_result = llyfr_search_result_new_from_json (node);
          continue;
        }

      if (g_strcmp0 (type, "match") == 0)
        {
          llyfr_search_result_add_match (current_result, node);
        }

      if (g_strcmp0 (type, "end") == 0)
        {
          llyfr_search_result_end (current_result);
          g_list_store_append (results, current_result);
          continue;
        }

      g_debug ("Unhandled type: %s", type);
    }

  return G_LIST_MODEL(results);
}

const gchar*
llyfr_search_context_get_directory (LlyfrSearchContext *context)
{
  LlyfrSearchContextPrivate *priv = llyfr_search_context_get_instance_private (context);

  return priv->directory ? priv->directory : "";
}

void
llyfr_search_context_set_directory (LlyfrSearchContext *context,
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

void
llyfr_search_context_class_init (LlyfrSearchContextClass *klass)
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

void
llyfr_search_context_init (LlyfrSearchContext *self)
{
  LlyfrSearchContextPrivate *priv = llyfr_search_context_get_instance_private (self);

  priv->directory = NULL;
}
