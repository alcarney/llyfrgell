/* llyfrgell-window.h
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
 */

#ifndef LLYFR_WINDOW_H
#define LLYFR_WINDOW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LLYFR_TYPE_WINDOW (llyfr_window_get_type())

G_DECLARE_FINAL_TYPE (LlyfrWindow, llyfr_window, LLYFR, WINDOW, GtkApplicationWindow)

LlyfrWindow* llyfr_window_new (GtkApplication* app);

G_END_DECLS

#endif /* LLYFR_WINDOW_H */
