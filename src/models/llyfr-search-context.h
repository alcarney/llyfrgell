/* llyfr-search-context.h
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

#ifndef LLYFR_SEARCH_CONTEXT_H
#define LLYFR_SEARCH_CONTEXT_H

#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

#define LLYFR_TYPE_SEARCH_CONTEXT (llyfr_search_context_get_type())

G_DECLARE_DERIVABLE_TYPE (LlyfrSearchContext, llyfr_search_context, LLYFR, SEARCH_CONTEXT, GObject)

struct _LlyfrSearchContextClass
{
  GObjectClass parent;
};

LlyfrSearchContext* llyfr_search_context_new           (char* directory);

const gchar*        llyfr_search_context_get_directory (LlyfrSearchContext *context);

void                llyfr_search_context_set_directory (LlyfrSearchContext *context,
                                                        const gchar *directory);

GListModel*         llyfr_search_context_search        (LlyfrSearchContext *context,
                                                        const gchar* query,
                                                        GError **error);

G_END_DECLS

#endif /* LLYFR_SEARCH_CONTEXT_H */
