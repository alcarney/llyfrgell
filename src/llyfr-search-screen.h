/* llyfr-search-screen.h
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

#ifndef LLYFR_SEARCH_SCREEN_H
#define LLYFR_SEARCH_SCREEN_H

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LLYFR_TYPE_SEARCH_SCREEN (llyfr_search_screen_get_type())

G_DECLARE_FINAL_TYPE (LlyfrSearchScreen, llyfr_search_screen, LLYFR, SEARCH_SCREEN, GtkBox)

LlyfrSearchScreen *llyfr_search_screen_new             (void);

void               llyfr_search_screen_set_application (LlyfrSearchScreen *self,
                                                        GtkApplication *app);

G_END_DECLS

#endif
