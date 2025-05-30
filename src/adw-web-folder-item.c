/* adw-web-folder-item.c
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

#include "adw-web-folder-item.h"
#include "adw-web-folder-item-private.h"

struct _AdwWebFolderItem
{
    AdwBin parent_instance;

    /* Properties */
    GFileInfo   *file_info;
    gchar       *icon_name;
    gchar       *label_text;


	/* Template widgets */
	GtkImage    *image;
	GtkLabel    *label;
};

typedef enum {
    PROP_FILE_INFO = 1,
    PROP_ICON_NAME,
    PROP_LABEL,
    N_PROPERTIES
} AdwWebFolderItemProperty;

static GParamSpec *properties[N_PROPERTIES] = { NULL, };

G_DEFINE_FINAL_TYPE (AdwWebFolderItem, adw_web_folder_item, ADW_TYPE_BIN)

AdwWebFolderItem *
adw_web_folder_item_new (GFileInfo *file_info)
{
    return g_object_new (ADW_WEB_TYPE_FOLDER_ITEM,
                         "file-info", file_info,
                         NULL);
}

static void
adw_web_folder_item_class_init (AdwWebFolderItemClass *klass)
{
    GObjectClass   *object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    object_class->set_property = adw_web_folder_item_set_property;
    object_class->get_property = adw_web_folder_item_get_property;

    properties[PROP_FILE_INFO] =
        g_param_spec_object ("file-info",
                             "FileInfo",
                             "The FileInfo to display",
                             G_TYPE_FILE_INFO,
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    properties[PROP_ICON_NAME] =
        g_param_spec_string ("icon-name",
                             "Icon name",
                             "Name of the icon",
                             NULL,
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    properties[PROP_LABEL] =
        g_param_spec_string ("label",
                             "Label",
                             "Name of the file/directory",
                             NULL,
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    g_object_class_install_properties (object_class,
                                       N_PROPERTIES,
                                       properties);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/example/adw_web/adw-web-folder-item.ui");
    gtk_widget_class_bind_template_child (widget_class, AdwWebFolderItem, image);
    gtk_widget_class_bind_template_child (widget_class, AdwWebFolderItem, label);
}

static void
adw_web_folder_item_init (AdwWebFolderItem *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));
}

static void
adw_web_folder_item_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    AdwWebFolderItem *self = ADW_WEB_FOLDER_ITEM (object);

    switch ((AdwWebFolderItemProperty) property_id)
        {
        case PROP_FILE_INFO:
            g_clear_object (&self->file_info);

            self->file_info = g_value_get_object (value);
            if (self->file_info != NULL) {
                self->file_info = g_object_ref (self->file_info);
                adw_web_folder_item_update_ui (self);
            }
            break;

        case PROP_ICON_NAME:
            g_free (self->icon_name);
            self->icon_name = g_value_dup_string (value);
            break;

        case PROP_LABEL:
            g_free (self->label_text);
            self->label_text = g_value_dup_string (value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        }
}

static void
adw_web_folder_item_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
    AdwWebFolderItem *self = ADW_WEB_FOLDER_ITEM (object);

    switch ((AdwWebFolderItemProperty) property_id)
        {
        case PROP_FILE_INFO:
            g_value_set_object (value, self->file_info);
            break;

        case PROP_ICON_NAME:
            g_value_set_string (value, self->icon_name);
            break;

        case PROP_LABEL:
            g_value_set_string (value, self->label_text);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        }
}

static void
adw_web_folder_item_update_ui    (AdwWebFolderItem *self)
{
    if (self->file_info != NULL) {
        GFile      *file;
        GFileInfo  *file_info;
        const char *icon_name;
        const char *file_name;

        // existance of standard::file attribute is guarenteed by GtkDirectoryList
        file = G_FILE (g_file_info_get_attribute_object (self->file_info, "standard::file"));
        // `FileInfo`s created by GtkDirectoryList are not populated with some core standard::* attributes
        file_info = g_file_query_info (file, "standard::", G_FILE_QUERY_INFO_NONE, NULL, NULL);

        if (file_info == NULL) {
            file_info = self->file_info;
        }

        file_name = g_file_info_get_name (file_info);
        icon_name = g_content_type_get_generic_icon_name (g_file_info_get_content_type (file_info));

        g_object_set (self,
                      "icon-name", icon_name,
                      "label", file_name,
                      NULL);
    }
}
