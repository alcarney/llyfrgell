/* llyfr-search-match.c
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

#define G_LOG_DOMAIN "llyfr-search-match"

#include "llyfr-search-match.h"

struct _LlyfrSearchMatch
{
  GObject  parent_instance;

  gchar   *text;
  gint64  line_number;
};

G_DEFINE_TYPE (LlyfrSearchMatch, llyfr_search_match, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_LINENUMBER,
  PROP_TEXT,
  LAST_PROP
};

LlyfrSearchMatch*
llyfr_search_match_new (gint64 line_number,
                        const gchar* text)
{
  return g_object_new (LLYFR_TYPE_SEARCH_MATCH,
                       "line-number", line_number,
                       "text", text,
                       NULL);
}

gint64
llyfr_search_match_get_line_number (LlyfrSearchMatch *match)
{
  return match->line_number;
}

void
llyfr_search_match_set_line_number (LlyfrSearchMatch *match,
                                    gint64 line_number)
{
  match->line_number = line_number;
}

void
llyfr_search_match_set_text (LlyfrSearchMatch *match,
                             const gchar *text)
{
  g_clear_pointer (&match->text, g_free);
  match->text = g_strchomp (g_strdup (text));
}

const gchar*
llyfr_search_match_get_text (LlyfrSearchMatch *match)
{
  return match->text ? match->text : "";
}

static void
llyfr_search_match_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  LlyfrSearchMatch *self = LLYFR_SEARCH_MATCH (object);

  switch (prop_id)
    {
    case PROP_LINENUMBER:
      g_value_set_int64 (value, llyfr_search_match_get_line_number (self));
      break;

    case PROP_TEXT:
      g_value_set_string (value, llyfr_search_match_get_text (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
llyfr_search_match_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  LlyfrSearchMatch *self = LLYFR_SEARCH_MATCH (object);

  switch (prop_id)
    {
    case PROP_LINENUMBER:
      llyfr_search_match_set_line_number (self, g_value_get_int64 (value));
      break;

    case PROP_TEXT:
      llyfr_search_match_set_text (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
llyfr_search_match_finalize (GObject *object)
{
  LlyfrSearchMatch *self = LLYFR_SEARCH_MATCH (object);
  g_free (self->text);

  G_OBJECT_CLASS (llyfr_search_match_parent_class)->finalize (object);
}

static void
llyfr_search_match_class_init (LlyfrSearchMatchClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = llyfr_search_match_get_property;
  object_class->set_property = llyfr_search_match_set_property;
  object_class->finalize = llyfr_search_match_finalize;

  g_object_class_install_property (object_class,
                                   PROP_LINENUMBER,
                                   g_param_spec_int64 ("line-number",
                                                       "line-number",
                                                       "The line's number",
                                                       -1,
                                                       G_MAXINT64,
                                                       -1,
                                                       G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_TEXT,
                                   g_param_spec_string ("text",
                                                        "text",
                                                        "A line of text.",
                                                        NULL,
                                                        G_PARAM_READWRITE));
}


static void
llyfr_search_match_init (LlyfrSearchMatch *self)
{

}                                   
