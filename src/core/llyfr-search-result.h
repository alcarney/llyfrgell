/* llyfr-search-result.h
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

#ifndef LLYFR_SEARCH_RESULT_H
#define LLYFR_SEARCH_RESULT_H

#include <glib.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

#define LLYFR_TYPE_SEARCH_RESULT (llyfr_search_result_get_type())

G_DECLARE_FINAL_TYPE (LlyfrSearchResult, llyfr_search_result, LLYFR, SEARCH_RESULT, GObject)

LlyfrSearchResult* llyfr_search_result_new              (const char *filepath);

LlyfrSearchResult* llyfr_search_result_new_from_json    (JsonNode *node);

void               llyfr_search_result_add_match        (LlyfrSearchResult *result,
                                                         JsonNode *node);

GList*             llyfr_search_result_get_matches      (LlyfrSearchResult *result);

void               llyfr_search_result_end              (LlyfrSearchResult *result);

const gchar       *llyfr_search_result_get_filepath     (LlyfrSearchResult *result);

void               llyfr_search_result_set_filepath     (LlyfrSearchResult *result,
                                                         const gchar* filepath);

G_END_DECLS

#endif /* LLYFR_SEARCH_RESULT_H */
