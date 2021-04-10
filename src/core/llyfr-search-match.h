/* llyfr-search-match.h
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

#ifndef LLYFR_SEARCH_MATCH_H
#define LLYFR_SEARCH_MATCH_H

#include <glib-object.h>

G_BEGIN_DECLS

#define LLYFR_TYPE_SEARCH_MATCH (llyfr_search_match_get_type())

G_DECLARE_FINAL_TYPE (LlyfrSearchMatch, llyfr_search_match, LLYFR, SEARCH_MATCH, GObject)

LlyfrSearchMatch *llyfr_search_match_new             (gint64 line_number,
                                                      const gchar *text);

void              llyfr_search_match_add_highlight   (LlyfrSearchMatch *match,
                                                      gint64 start,
                                                      gint64 end);

GArray*           llyfr_search_match_get_highlights  (LlyfrSearchMatch *match);

gint64            llyfr_search_match_get_line_number (LlyfrSearchMatch *match);

void              llyfr_search_match_set_line_number (LlyfrSearchMatch *match,
                                                      gint64 line_number);

const gchar*      llyfr_search_match_get_text        (LlyfrSearchMatch *match);

void              llyfr_search_match_set_text        (LlyfrSearchMatch *match,
                                                      const gchar *text);

G_END_DECLS

#endif /* LLYFR_SEARCH_MATCH_H */

