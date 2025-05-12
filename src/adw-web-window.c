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
#include <adwaita.h>

#include "adw-web-window.h"
#include "adw-web-window-private.h"
#include "adw-web-folder-item.h"

struct _AdwWebWindow
{
	AdwApplicationWindow  parent_instance;

	/* Instance fields */
	GtkFileDialog		*file_dialog;
	GFile				*chosen_folder;
	GFile				*current_folder;

	/* Template widgets */
	GtkStack            *stack;
	GtkButton           *back_button;
	GtkButton           *button;
	GtkDirectoryList    *directory_list;
};

typedef enum {
    PROP_CHOSEN_FOLDER = 1,
    PROP_CURRENT_FOLDER,
    N_PROPERTIES
} AdwWebWindowProperty;

static GParamSpec *properties[N_PROPERTIES] = { NULL, };

G_DEFINE_FINAL_TYPE (AdwWebWindow, adw_web_window, ADW_TYPE_APPLICATION_WINDOW)

static void
adw_web_window_class_init (AdwWebWindowClass *klass)
{
	GObjectClass   *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	g_type_ensure (ADW_WEB_TYPE_FOLDER_ITEM);

	object_class->set_property = adw_web_window_set_property;
	object_class->get_property = adw_web_window_get_property;

    properties[PROP_CHOSEN_FOLDER] =
        g_param_spec_object ("chosen-folder",
                             "Chosen folder",
                             "The folder that was chosen by the user",
                             G_TYPE_FILE,
                             G_PARAM_READABLE);

    properties[PROP_CURRENT_FOLDER] =
        g_param_spec_object ("current-folder",
                             "Current folder",
                             "The folder that is currently being shown to the user",
                             G_TYPE_FILE,
                             G_PARAM_READABLE);

    g_object_class_install_properties (object_class,
                                       N_PROPERTIES,
                                       properties);

	gtk_widget_class_set_template_from_resource (widget_class, "/com/example/adw_web/adw-web-window.ui");
	gtk_widget_class_bind_template_child (widget_class, AdwWebWindow, stack);
	gtk_widget_class_bind_template_child (widget_class, AdwWebWindow, back_button);
	gtk_widget_class_bind_template_child (widget_class, AdwWebWindow, button);
	gtk_widget_class_bind_template_child (widget_class, AdwWebWindow, directory_list);
	gtk_widget_class_bind_template_callback (widget_class, select_folder_cb);
	gtk_widget_class_bind_template_callback (widget_class, item_clicked_cb);
	gtk_widget_class_bind_template_callback (widget_class, back_clicked_cb);
}

static void
adw_web_window_init (AdwWebWindow *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));
	self->file_dialog = gtk_file_dialog_new ();
}

static void
adw_web_window_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
    AdwWebWindow *self = ADW_WEB_WINDOW (object);

    switch ((AdwWebWindowProperty) property_id)
        {
        case PROP_CHOSEN_FOLDER:
            g_clear_object (&self->chosen_folder);

            self->chosen_folder = g_value_get_object (value);
            if (self->chosen_folder != NULL) {
                g_object_ref (self->chosen_folder);
            }
            break;

        case PROP_CURRENT_FOLDER:
            g_clear_object (&self->current_folder);

            self->current_folder = g_value_get_object (value);
            if (self->current_folder != NULL) {
                g_object_ref (self->current_folder);
            }
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        }
}

static void
adw_web_window_get_property (GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
    AdwWebWindow *self = ADW_WEB_WINDOW (object);

    switch ((AdwWebWindowProperty) property_id)
        {
        case PROP_CHOSEN_FOLDER:
            g_value_set_object (value, self->chosen_folder);
            break;

        case PROP_CURRENT_FOLDER:
            g_value_set_object (value, self->current_folder);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        }
}

static void
adw_web_window_set_chosen_folder (AdwWebWindow *self,
								  GFile        *folder)
{
    g_clear_object (&self->chosen_folder);

	if (folder != NULL) {
        g_object_ref (folder);
	}

    self->chosen_folder = folder;
    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CHOSEN_FOLDER]);
}

static void
adw_web_window_set_current_folder (AdwWebWindow *self,
								   GFile        *folder)
{
    g_clear_object (&self->current_folder);

	if (folder != NULL) {
        g_object_ref (folder);
	}

    self->current_folder = folder;
    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CURRENT_FOLDER]);

	gtk_widget_set_visible (GTK_WIDGET (self->back_button), folder != NULL);
	gtk_widget_set_sensitive (GTK_WIDGET (self->back_button), !g_file_equal (folder, self->chosen_folder));
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
	AdwWebWindow *self = ADW_WEB_WINDOW (user_data);

	GError *error = NULL;
	GFile *folder = gtk_file_dialog_select_folder_finish (self->file_dialog, res, &error);

	if (error) {
		if (!g_error_matches (error, gtk_dialog_error_quark(), GTK_DIALOG_ERROR_DISMISSED)) {
			const gchar *domain = g_quark_to_string (error->domain);
			g_warning ("%s - %s!\n", domain, error->message);
		}
		g_error_free (error);
		return;
	}

	gtk_stack_set_visible_child_name (self->stack, "files");

	adw_web_window_set_chosen_folder (self, folder);
    adw_web_window_set_current_folder (self, folder);

	g_object_unref (folder);
}

static void
item_clicked_cb (GtkListView *list_view,
                 guint        position,
                 gpointer     user_data)
{
	AdwWebWindow *self;
	GListModel   *model;
	GFileInfo    *file_info;

	self = ADW_WEB_WINDOW (user_data);
	model = G_LIST_MODEL (gtk_list_view_get_model (list_view));
	file_info = G_FILE_INFO (g_list_model_get_item (model, position));

	if (g_file_info_get_file_type (file_info) == G_FILE_TYPE_DIRECTORY) {
		GFile *folder = G_FILE (g_file_info_get_attribute_object (file_info, "standard::file"));
		adw_web_window_set_current_folder (self, folder);
	}
}

static void
back_clicked_cb (GtkButton *button,
                 gpointer   user_data)
{
	AdwWebWindow *self;
	GFile *current, *previous;

	self = ADW_WEB_WINDOW (user_data);
	g_object_get (self, "current-folder", &current, NULL);
	previous = g_file_get_parent (current);

	adw_web_window_set_current_folder (self, previous);
}
