/* llyfr-search-result.c
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

#define G_LOG_DOMAIN "llyfr-search-result"

#include "llyfr-search-result.h"

struct _LlyfrSearchResult
{
  GObject  parent_instance;

  gchar *filepath;
  GList *lines;

};

G_DEFINE_TYPE (LlyfrSearchResult, llyfr_search_result, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_FILEPATH,
  LAST_PROP
};

LlyfrSearchResult*
llyfr_search_result_new (const char* filepath)
{
  return g_object_new (LLYFR_TYPE_SEARCH_RESULT,
                       "filepath", filepath,
                       NULL);
}

LlyfrSearchResult*
llyfr_search_result_new_from_json (JsonNode* node)
{
  g_autoptr (JsonReader) reader = json_reader_new (node);

  json_reader_read_member (reader, "type");
  const char *type = json_reader_get_string_value (reader);
  g_assert (g_strcmp0 (type, "begin") == 0);

  json_reader_end_member (reader);
  g_assert (json_reader_read_member (reader, "data"));
  g_assert (json_reader_read_member (reader, "path"));
  g_assert (json_reader_read_member (reader, "text"));

  const char *filepath = json_reader_get_string_value (reader);
  return llyfr_search_result_new (filepath);
}

void
llyfr_search_result_add_match (LlyfrSearchResult *result,
                               JsonNode *node)
{
  g_autoptr (JsonReader) reader = json_reader_new (node);

  json_reader_read_member (reader, "type");
  const char *type = json_reader_get_string_value (reader);
  g_assert (g_strcmp0 (type, "match") == 0);

  json_reader_end_member (reader);
  g_assert (json_reader_read_member (reader, "data"));
  g_assert (json_reader_read_member (reader, "lines"));
  g_assert (json_reader_read_member (reader, "text"));

  const char *line = json_reader_get_string_value (reader);
  result->lines = g_list_prepend (result->lines, g_strchomp (g_strdup (line)));
}

void
llyfr_search_result_end (LlyfrSearchResult *result)
{
  result->lines = g_list_reverse (result->lines);
}

const gchar*
llyfr_search_result_get_filepath (LlyfrSearchResult *self)
{
  return self->filepath ? self->filepath : "";
}

void
llyfr_search_result_set_filepath (LlyfrSearchResult *result,
                                  const gchar* filepath)
{
  g_clear_pointer (&result->filepath, g_free);
  result->filepath = g_strdup(filepath);
}

GList*
llyfr_search_result_get_lines (LlyfrSearchResult *result)
{
  return result->lines;
}

static void
llyfr_search_result_get_property (GObject    *object,
                                  guint      prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  LlyfrSearchResult *self = LLYFR_SEARCH_RESULT (object);

  switch (prop_id)
    {
    case PROP_FILEPATH:
      g_value_set_string (value, llyfr_search_result_get_filepath (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
llyfr_search_result_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  LlyfrSearchResult *self = LLYFR_SEARCH_RESULT (object);

  switch (prop_id)
    {
    case PROP_FILEPATH:
      llyfr_search_result_set_filepath (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

void
llyfr_search_result_class_init (LlyfrSearchResultClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = llyfr_search_result_get_property;
  object_class->set_property = llyfr_search_result_set_property;

  g_object_class_install_property (object_class,
                                   PROP_FILEPATH,
                                   g_param_spec_string ("filepath",
                                                        "Filepath",
                                                        "Filepath that contains one or more search matches",
                                                        NULL,
                                                        G_PARAM_READWRITE));
}

void
llyfr_search_result_init (LlyfrSearchResult *self)
{
  self->filepath = NULL;
  self->lines = NULL;
}

