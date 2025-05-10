/* adw-web-window.c
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

#include "config.h"

#include "adw-web-window.h"

static void
select_folder_cb (GtkButton*, gpointer);

static void
select_folder_finish_cb (GObject*, GAsyncResult*, gpointer);

struct _AdwWebWindow
{
	AdwApplicationWindow  parent_instance;

	/* Instance fields */
	GtkFileDialog		*file_dialog;

	/* Template widgets */
	GtkStack            *stack;
	GtkButton           *button;
};

G_DEFINE_FINAL_TYPE (AdwWebWindow, adw_web_window, ADW_TYPE_APPLICATION_WINDOW)

static void
adw_web_window_class_init (AdwWebWindowClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gtk_widget_class_set_template_from_resource (widget_class, "/com/example/adw_web/adw-web-window.ui");
	gtk_widget_class_bind_template_child (widget_class, AdwWebWindow, stack);
	gtk_widget_class_bind_template_child (widget_class, AdwWebWindow, button);
	gtk_widget_class_bind_template_callback (widget_class, select_folder_cb);
}

static void
adw_web_window_init (AdwWebWindow *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));
	self->file_dialog = gtk_file_dialog_new ();
}

static void
select_folder_cb (GtkButton    *button,
				  gpointer 		user_data)
{
	AdwWebWindow *window = ADW_WEB_WINDOW (user_data);
	gtk_file_dialog_select_folder (window->file_dialog, GTK_WINDOW (window), NULL, select_folder_finish_cb, user_data);
}

static void
select_folder_finish_cb (GObject 		*source_object,
						 GAsyncResult 	*res,
						 gpointer 		 user_data)
{
	AdwWebWindow *window = ADW_WEB_WINDOW (user_data);

	GError *error = NULL;
	GFile *folder = gtk_file_dialog_select_folder_finish (window->file_dialog, res, &error);
	char *folder_path = NULL;

	if (error) {
		if (!g_error_matches (error, gtk_dialog_error_quark(), GTK_DIALOG_ERROR_DISMISSED)) {
			const gchar *domain = g_quark_to_string (error->domain);
			g_print ("%s - %s!\n", domain, error->message);
		}
		g_error_free (error);
		return;
	}

	folder_path = g_file_get_path (folder);
	g_print ("Folder selected: %s!\n", folder_path);

	g_free (folder_path);
	g_object_unref (folder);
}
