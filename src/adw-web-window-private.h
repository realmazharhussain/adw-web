/* adw-web-folder-item-private.h
 *
 * Copyright 2025 Mazhar Hussain
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "adw-web-window.h"

static void
adw_web_window_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec);

static void
adw_web_window_get_property (GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec);

static void
adw_web_window_set_chosen_folder (AdwWebWindow *self,
                                  GFile        *folder);

static void
adw_web_window_set_current_folder (AdwWebWindow *self,
                                   GFile        *folder);

static void
select_folder_cb (GtkButton *button,
                  gpointer   user_data);

static void
select_folder_finish_cb (GObject      *source_object,
                         GAsyncResult *res,
                         gpointer      user_data);

static void
item_clicked_cb (GtkListView *list_view,
                 guint        position,
                 gpointer     user_data);

static void
back_clicked_cb (GtkButton *button,
                 gpointer   user_data);
