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
#include "llyfr-search-match.h"

struct _LlyfrSearchResult
{
  GObject  parent_instance;

  gchar   *filepath;
  GList   *matches;
};

G_DEFINE_TYPE (LlyfrSearchResult, llyfr_search_result, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_FILEPATH,
  LAST_PROP
};

// Parsing helpers.
static void        parse_check_type        (JsonReader *reader,
                                            const char* type);

static const char* parse_match_filepath    (JsonReader *reader);

static void        parse_match_highlight   (JsonReader *reader,
                                            gint64     *start,
                                            gint64     *end);

static gint64      parse_match_line_number (JsonReader *reader);

static const char* parse_match_text        (JsonReader *reader);

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
  parse_check_type (reader, "begin");

  const char *filepath = parse_match_filepath (reader);
  return llyfr_search_result_new (filepath);
}

void
llyfr_search_result_add_match (LlyfrSearchResult *result,
                               JsonNode *node)
{
  g_autoptr (JsonReader) reader = json_reader_new (node);
  parse_check_type (reader, "match");

  const char *line = parse_match_text (reader);
  gint64 line_number = parse_match_line_number (reader);

  LlyfrSearchMatch *match = llyfr_search_match_new (line_number, line);

  g_assert (json_reader_read_member (reader, "data"));
  g_assert (json_reader_read_member (reader, "submatches"));
  g_assert (json_reader_is_array (reader));

  for (gint i = 0; i < json_reader_count_elements (reader); i++) {
    gint64 start, end;

    json_reader_read_element (reader, i);
    parse_match_highlight (reader, &start, &end);
    json_reader_end_element (reader);

    llyfr_search_match_add_highlight (match, start, end);
  }

  result->matches = g_list_prepend (result->matches, match);
}

void
llyfr_search_result_end (LlyfrSearchResult *result)
{
  result->matches = g_list_reverse (result->matches);
}

const gchar*
llyfr_search_result_get_filepath (LlyfrSearchResult *self)
{
  return self->filepath ? self->filepath : "";
}

void
llyfr_search_result_set_filepath (LlyfrSearchResult *result,
                                  const gchar *filepath)
{
  g_clear_pointer (&result->filepath, g_free);
  result->filepath = g_strdup (filepath);
}

GList*
llyfr_search_result_get_matches (LlyfrSearchResult *result)
{
  return result->matches;
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

static void
llyfr_search_result_finalize (GObject *object)
{
  LlyfrSearchResult *self = LLYFR_SEARCH_RESULT (object);

  g_free (self->filepath);
  g_list_free_full (self->matches, g_object_unref);

  G_OBJECT_CLASS (llyfr_search_result_parent_class)->finalize (object);
}

static void
llyfr_search_result_class_init (LlyfrSearchResultClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = llyfr_search_result_get_property;
  object_class->set_property = llyfr_search_result_set_property;
  object_class->finalize = llyfr_search_result_finalize;

  g_object_class_install_property (object_class,
                                   PROP_FILEPATH,
                                   g_param_spec_string ("filepath",
                                                        "Filepath",
                                                        "Filepath that contains one or more search matches",
                                                        NULL,
                                                        G_PARAM_READWRITE));
}

static void
llyfr_search_result_init (LlyfrSearchResult *self)
{

}

static void
parse_check_type (JsonReader *reader, const char *expected)
{
  g_assert (json_reader_read_member (reader, "type"));
  const char *actual = json_reader_get_string_value (reader);

  g_assert (g_strcmp0 (actual, expected) == 0);
  json_reader_end_member (reader);
}

static const char*
parse_match_filepath (JsonReader *reader)
{
  g_assert (json_reader_read_member (reader, "data"));
  g_assert (json_reader_read_member (reader, "path"));
  g_assert (json_reader_read_member (reader, "text"));

  const char *filepath = json_reader_get_string_value (reader);

  json_reader_end_member (reader);
  json_reader_end_member (reader);
  json_reader_end_member (reader);

  return filepath;
}

static void parse_match_highlight (JsonReader *reader,
                                   gint64     *start,
                                   gint64     *end)
{
  g_assert (json_reader_is_object (reader));
  g_assert (json_reader_read_member (reader, "start"));

  *start = json_reader_get_int_value (reader);
  json_reader_end_member (reader);

  g_assert (json_reader_read_member (reader, "end"));
  *end = json_reader_get_int_value (reader);
  json_reader_end_member (reader);
}

static gint64
parse_match_line_number (JsonReader *reader)
{
  g_assert (json_reader_read_member (reader, "data"));
  g_assert (json_reader_read_member (reader, "line_number"));

  gint64 line_number = json_reader_get_int_value (reader);

  json_reader_end_member (reader);
  json_reader_end_member (reader);

  return line_number;
}

static const char*
parse_match_text (JsonReader *reader)
{
  g_assert (json_reader_read_member (reader, "data"));
  g_assert (json_reader_read_member (reader, "lines"));
  g_assert (json_reader_read_member (reader, "text"));

  const char* text = json_reader_get_string_value (reader);

  json_reader_end_member (reader);
  json_reader_end_member (reader);
  json_reader_end_member (reader);

  return text;
}
