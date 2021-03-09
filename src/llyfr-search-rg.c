/* llyfr-search-rg.c
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
#define G_LOG_DOMAIN "llyfr-search-rg"

#include "llyfr-search-rg.h"

void llyfr_search_rg(const gchar* query)
{
  GSubprocess* process;
  GError* error = NULL;

  char* line = NULL;
  gsize length = 0;
  GInputStream* stdout_pipe;
  GDataInputStream* data_stream;

  g_message("%s", query);

  process = g_subprocess_new (G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error,
                              "flatpak-spawn", "--host",
                              "rg", "--json",
                              query, "/home/alex/Code/gnome-builder",
                              NULL);

  if (process == NULL) {

    g_message ("Unable to create subprocess");

    if (error != NULL) {
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
