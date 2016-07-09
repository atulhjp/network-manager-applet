/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager Connection editor -- Connection editor for NetworkManager
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Atul Anand <atulhjp@gmail.com>
 */

#include "nm-default.h"

#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "page-proxy.h"
#include "nm-connection-editor.h"

G_DEFINE_TYPE (CEPageProxy, ce_page_proxy, CE_TYPE_PAGE)

#define CE_PAGE_PROXY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CE_TYPE_PAGE_PROXY, CEPageProxyPrivate))

typedef struct {
	NMSettingProxy *setting;

	/* Method */
	GtkComboBox *method;

	/* HTTP Proxy */
	GtkWidget *http_proxy_label;
	GtkEntry *http_proxy;
	GtkSpinButton *http_port;
	GtkCheckButton *http_default;

	/* SSL Proxy */
	GtkWidget *ssl_proxy_label;
	GtkEntry *ssl_proxy;
	GtkSpinButton *ssl_port;

	/* FTP Proxy */
	GtkWidget *ftp_proxy_label;
	GtkEntry *ftp_proxy;
	GtkSpinButton *ftp_port;

	/* SOCKS Proxy */
	GtkWidget *socks_proxy_label;
	GtkEntry *socks_proxy;
	GtkSpinButton *socks_port;
	GtkCheckButton *socks_version_5;

	/* PAC Script */
	GtkWidget *pac_script_label;
	GtkFileChooser *pac_script;

	/* NO PROXY FOR */
	GtkWidget *no_proxy_for_label;
	GtkEntry *no_proxy_for;

	/* PAC URL */
	GtkWidget *pac_url_label;
	GtkEntry *pac_url;
} CEPageProxyPrivate;

#define PROXY_METHOD_NONE    0
#define PROXY_METHOD_AUTO    1
#define PROXY_METHOD_MANUAL  2

static void
proxy_private_init (CEPageProxy *self)
{
	CEPageProxyPrivate *priv = CE_PAGE_PROXY_GET_PRIVATE (self);
	GtkBuilder *builder;

	builder = CE_PAGE (self)->builder;

	priv->method = GTK_COMBO_BOX (gtk_builder_get_object (builder, "proxy_method"));

	priv->http_proxy_label = GTK_WIDGET (gtk_builder_get_object (builder, "proxy_http_label"));
	priv->http_proxy = GTK_ENTRY (gtk_builder_get_object (builder, "proxy_http_entry"));
	priv->http_port = GTK_SPIN_BUTTON (gtk_builder_get_object (builder, "proxy_http_port_spin"));
	priv->http_default = GTK_CHECK_BUTTON (gtk_builder_get_object (builder, "proxy_http_default_checkbutton"));

	priv->ssl_proxy_label = GTK_WIDGET (gtk_builder_get_object (builder, "proxy_ssl_label"));
	priv->ssl_proxy = GTK_ENTRY (gtk_builder_get_object (builder, "proxy_ssl_entry"));
	priv->ssl_port = GTK_SPIN_BUTTON (gtk_builder_get_object (builder, "proxy_ssl_port_spin"));

	priv->ftp_proxy_label = GTK_WIDGET (gtk_builder_get_object (builder, "proxy_ftp_label"));
	priv->ftp_proxy = GTK_ENTRY (gtk_builder_get_object (builder, "proxy_ftp_entry"));
	priv->ftp_port = GTK_SPIN_BUTTON (gtk_builder_get_object (builder, "proxy_ftp_port_spin"));

	priv->socks_proxy_label = GTK_WIDGET (gtk_builder_get_object (builder, "proxy_socks_label"));
	priv->socks_proxy = GTK_ENTRY (gtk_builder_get_object (builder, "proxy_socks_entry"));
	priv->socks_port = GTK_SPIN_BUTTON (gtk_builder_get_object (builder, "proxy_socks_port_spin"));
	priv->socks_version_5 = GTK_CHECK_BUTTON (gtk_builder_get_object (builder, "proxy_socks_version_checkbutton"));

	priv->pac_script_label = GTK_WIDGET (gtk_builder_get_object (builder, "proxy_pac_script_label"));
	priv->pac_script = GTK_FILE_CHOOSER (gtk_builder_get_object (builder, "proxy_pac_script_button"));

	priv->no_proxy_for_label = GTK_WIDGET (gtk_builder_get_object (builder, "proxy_no_proxy_for_label"));
	priv->no_proxy_for = GTK_ENTRY (gtk_builder_get_object (builder, "proxy_no_proxy_for_entry"));

	priv->pac_url_label = GTK_WIDGET (gtk_builder_get_object (builder, "proxy_pac_url_label"));
	priv->pac_url = GTK_ENTRY (gtk_builder_get_object (builder, "proxy_pac_url_entry"));
}

static void
method_changed (GtkComboBox *combo, gpointer user_data)
{
	CEPageProxy *self = user_data;
	CEPageProxyPrivate *priv = CE_PAGE_PROXY_GET_PRIVATE (self);
	int method;

	method = gtk_combo_box_get_active (combo);

	if (method == PROXY_METHOD_AUTO || method == PROXY_METHOD_NONE) {
		gtk_widget_set_sensitive (GTK_WIDGET (priv->http_proxy_label), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->http_proxy), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->http_port), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->http_default), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ssl_proxy_label), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ssl_proxy), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ssl_port), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ftp_proxy_label), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ftp_proxy), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ftp_port), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->socks_proxy_label), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->socks_proxy), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->socks_port), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->socks_version_5), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_script_label), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_script), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->no_proxy_for_label), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->no_proxy_for), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_url_label), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_url), TRUE);

		if (method == PROXY_METHOD_NONE) {
			gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_script_label), FALSE);
			gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_script), FALSE);
			gtk_widget_set_sensitive (GTK_WIDGET (priv->no_proxy_for_label), FALSE);
			gtk_widget_set_sensitive (GTK_WIDGET (priv->no_proxy_for), FALSE);
			gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_url_label), FALSE);
			gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_url), FALSE);
		}
	} else {
		gtk_widget_set_sensitive (GTK_WIDGET (priv->http_proxy_label), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->http_proxy), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->http_port), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->http_default), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ssl_proxy_label), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ssl_proxy), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ssl_port), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ftp_proxy_label), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ftp_proxy), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->ftp_port), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->socks_proxy_label), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->socks_proxy), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->socks_port), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->socks_version_5), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_script_label), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_script), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->no_proxy_for_label), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->no_proxy_for), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_url_label), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (priv->pac_url), FALSE);
	}
}

static void
populate_ui (CEPageProxy *self)
{
	CEPageProxyPrivate *priv = CE_PAGE_PROXY_GET_PRIVATE (self);
	NMSettingProxy *setting = priv->setting;
	NMSettingProxyMethod s_method;
	GString *string = NULL;
	char **excludes = NULL;
	const char *uri = NULL;

	/* Method */
	s_method = nm_setting_proxy_get_method (setting);
	switch (s_method) {
	case NM_SETTING_PROXY_METHOD_NONE:
		gtk_combo_box_set_active (priv->method, PROXY_METHOD_NONE);
		/* Nothing to Show */
		break;
	case NM_SETTING_PROXY_METHOD_AUTO:
		gtk_combo_box_set_active (priv->method, PROXY_METHOD_AUTO);

		/* No Proxy For */
		string = g_string_new ("");
		for (excludes = nm_setting_proxy_get_no_proxy_for (setting); *excludes; excludes++) {
			if (string->len)
				g_string_append (string, ", ");
			g_string_append (string, *excludes);
		}
		gtk_entry_set_text (priv->no_proxy_for, string->str);
		g_string_free (string, TRUE);

		/* Pac Url */
		gtk_entry_set_text (priv->pac_url, nm_setting_proxy_get_pac_url (setting));

		break;
	case NM_SETTING_PROXY_METHOD_MANUAL:
		gtk_combo_box_set_active (priv->method, PROXY_METHOD_MANUAL);

		/* HTTP Proxy */
		gtk_entry_set_text (priv->http_proxy, nm_setting_proxy_get_http_proxy (setting));
		gtk_spin_button_set_value (priv->http_port,
		                          (gdouble) nm_setting_proxy_get_http_port (setting));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->http_default),
		                              nm_setting_proxy_get_http_default (setting));

		/* SSL Proxy */
		gtk_entry_set_text (priv->ssl_proxy, nm_setting_proxy_get_ssl_proxy (setting));
		gtk_spin_button_set_value (priv->ssl_port,
		                          (gdouble) nm_setting_proxy_get_ssl_port (setting));

		/* FTP Proxy */
		gtk_entry_set_text (priv->ftp_proxy, nm_setting_proxy_get_ftp_proxy (setting));
		gtk_spin_button_set_value (priv->ftp_port,
		                          (gdouble) nm_setting_proxy_get_ftp_port (setting));

		/* SOCKS Proxy */
		gtk_entry_set_text (priv->socks_proxy, nm_setting_proxy_get_socks_proxy (setting));
		gtk_spin_button_set_value (priv->socks_port,
		                          (gdouble) nm_setting_proxy_get_socks_port (setting));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->socks_version_5),
		                              nm_setting_proxy_get_socks_version_5 (setting));

		/* Pac Script */
		uri = nm_setting_proxy_get_pac_script (setting);
		if (uri)
			gtk_file_chooser_set_uri (priv->pac_script, uri);

		/* No Proxy For */
		string = g_string_new ("");
		for (excludes = (char **) nm_setting_proxy_get_no_proxy_for (setting); *excludes; excludes++) {
			if (string->len)
				g_string_append (string, ", ");
			g_string_append (string, *excludes);
		}
		gtk_entry_set_text (priv->no_proxy_for, string->str);
		g_string_free (string, TRUE);
	}
}

static void
finish_setup (CEPageProxy *self, gpointer unused, GError *error, gpointer user_data)
{
	CEPageProxyPrivate *priv = CE_PAGE_PROXY_GET_PRIVATE (self);

	if (error)
		return;

	populate_ui (self);

	method_changed (priv->method, self);
	g_signal_connect (priv->method, "changed", G_CALLBACK (method_changed), self);
}

CEPage *
ce_page_proxy_new (NMConnectionEditor *editor,
                   NMConnection *connection,
                   GtkWindow *parent_window,
                   NMClient *client,
                   const char **out_secrets_setting_name,
                   GError **error)
{
	CEPageProxy *self;
	CEPageProxyPrivate *priv;
	NMSettingConnection *s_con;

	self = CE_PAGE_PROXY (ce_page_new (CE_TYPE_PAGE_PROXY,
	                                   editor,
	                                   connection,
	                                   parent_window,
	                                   client,
	                                   UIDIR "/ce-page-proxy.ui",
	                                   "ProxyPage",
	                                   _("Proxy")));
	if (!self) {
		g_set_error_literal (error, NMA_ERROR, NMA_ERROR_GENERIC, _("Could not load Proxy user interface."));
		return NULL;
	}

	proxy_private_init (self);
	priv = CE_PAGE_PROXY_GET_PRIVATE (self);

	s_con = nm_connection_get_setting_connection (connection);
	g_assert (s_con);

	priv->setting = nm_connection_get_setting_proxy (connection);
	g_assert (priv->setting);

	g_signal_connect (self, "initialized", G_CALLBACK (finish_setup), NULL);

	return CE_PAGE (self);
}

static void
ui_to_setting (CEPageProxy *self)
{
	CEPageProxyPrivate *priv = CE_PAGE_PROXY_GET_PRIVATE (self);
	NMSettingConnection *s_con;
	int method;
	const char *http_proxy = NULL;
	guint32 http_port;
	gboolean http_default = FALSE;
	const char *ssl_proxy = NULL;
	guint32 ssl_port;
	const char *ftp_proxy = NULL;
	guint32 ftp_port;
	const char *socks_proxy = NULL;
	guint32 socks_port;
	gboolean socks_version_5 = FALSE;
	const char *pac_script = NULL;
	const char *text;
	GPtrArray *tmp_array = NULL;
	char **no_proxy_for = NULL;
	char **items = NULL, **iter;
	const char *pac_url = NULL;

	s_con = nm_connection_get_setting_connection (CE_PAGE (self)->connection);
	g_return_if_fail (s_con != NULL);

	/* Method */
	method = gtk_combo_box_get_active (priv->method);
	switch (method) {
	case PROXY_METHOD_NONE:
		nm_connection_remove_setting (CE_PAGE (self)->connection, NM_TYPE_SETTING_PROXY);
		priv->setting = (NMSettingProxy *) nm_setting_proxy_new ();
		nm_connection_add_setting (CE_PAGE (self)->connection, NM_SETTING (priv->setting));

		/* Update NMSetting */
		g_object_set (priv->setting,
		              NM_SETTING_PROXY_METHOD, NM_SETTING_PROXY_METHOD_NONE,
		              NULL);
		break;
	case PROXY_METHOD_AUTO:
		nm_connection_remove_setting (CE_PAGE (self)->connection, NM_TYPE_SETTING_PROXY);
		priv->setting = (NMSettingProxy *) nm_setting_proxy_new ();
		nm_connection_add_setting (CE_PAGE (self)->connection, NM_SETTING (priv->setting));

		/* No Proxy For */
		tmp_array = g_ptr_array_new ();
		text = gtk_entry_get_text (GTK_ENTRY (priv->no_proxy_for));
		if (text && strlen (text)) {
			items = g_strsplit_set (text, ", ;:", 0);
			for (iter = items; *iter; iter++) {
				char *stripped = g_strstrip (*iter);

				if (strlen (stripped))
					g_ptr_array_add (tmp_array, g_strdup (stripped));
			}
			g_strfreev (items);
		}
		g_ptr_array_add (tmp_array, NULL);
		no_proxy_for = (char **) g_ptr_array_free (tmp_array, FALSE);

		pac_url = gtk_entry_get_text (priv->pac_url);

		/* Update NMSetting */
		g_object_set (priv->setting,
		              NM_SETTING_PROXY_METHOD, NM_SETTING_PROXY_METHOD_AUTO,
		              NM_SETTING_PROXY_NO_PROXY_FOR, no_proxy_for,
		              NM_SETTING_PROXY_PAC_URL, pac_url,
		              NULL);
		break;
	case PROXY_METHOD_MANUAL:
		nm_connection_remove_setting (CE_PAGE (self)->connection, NM_TYPE_SETTING_PROXY);
		priv->setting = (NMSettingProxy *) nm_setting_proxy_new ();
		nm_connection_add_setting (CE_PAGE (self)->connection, NM_SETTING (priv->setting));

		http_proxy = gtk_entry_get_text (priv->http_proxy);
		http_port = (guint32) gtk_spin_button_get_value_as_int (priv->http_port);

		/* HTTP Default */
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->http_default)))
			http_default = TRUE;

		ssl_proxy = gtk_entry_get_text (priv->ssl_proxy);
		ssl_port = (guint32) gtk_spin_button_get_value_as_int (priv->ssl_port);

		ftp_proxy = gtk_entry_get_text (priv->ftp_proxy);
		ftp_port = (guint32) gtk_spin_button_get_value_as_int (priv->ftp_port);

		socks_proxy = gtk_entry_get_text (priv->socks_proxy);
		socks_port = (guint32) gtk_spin_button_get_value_as_int (priv->socks_port);

		/* SOCKS Version */
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->socks_version_5)))
			socks_version_5 = TRUE;

		/* No Proxy For */
		tmp_array = g_ptr_array_new ();
		text = gtk_entry_get_text (GTK_ENTRY (priv->no_proxy_for));
		if (text && strlen (text)) {
			items = g_strsplit_set (text, ", ;:", 0);
			for (iter = items; *iter; iter++) {
				char *stripped = g_strstrip (*iter);

				if (strlen (stripped))
					g_ptr_array_add (tmp_array, g_strdup (stripped));
			}
			g_strfreev (items);
		}
		g_ptr_array_add (tmp_array, NULL);
		no_proxy_for = (char **) g_ptr_array_free (tmp_array, FALSE);

		pac_script = gtk_file_chooser_get_uri (priv->pac_script);

		/* Update NMSetting */
		g_object_set (priv->setting,
		              NM_SETTING_PROXY_METHOD, NM_SETTING_PROXY_METHOD_MANUAL,
		              NM_SETTING_PROXY_HTTP_PROXY, http_proxy,
		              NM_SETTING_PROXY_HTTP_PORT, http_port,
		              NM_SETTING_PROXY_HTTP_DEFAULT, http_default,
		              NM_SETTING_PROXY_SSL_PROXY, ssl_proxy,
		              NM_SETTING_PROXY_SSL_PORT, ssl_port,
		              NM_SETTING_PROXY_FTP_PROXY, ftp_proxy,
		              NM_SETTING_PROXY_FTP_PORT, ftp_port,
		              NM_SETTING_PROXY_SOCKS_PROXY, socks_proxy,
		              NM_SETTING_PROXY_SOCKS_PORT, socks_port,
		              NM_SETTING_PROXY_SOCKS_VERSION_5, socks_version_5,
		              NM_SETTING_PROXY_NO_PROXY_FOR, no_proxy_for,
		              NM_SETTING_PROXY_PAC_SCRIPT, pac_script,
		              NULL);
	}
}

static gboolean
ce_page_validate_v (CEPage *page, NMConnection *connection, GError **error)
{
	CEPageProxy *self = CE_PAGE_PROXY (page);
	CEPageProxyPrivate *priv = CE_PAGE_PROXY_GET_PRIVATE (self);

	if (!priv->setting) {
		priv->setting = (NMSettingProxy *) nm_setting_proxy_new ();
		nm_connection_add_setting (connection, NM_SETTING (priv->setting));
	}
	ui_to_setting (self);

	return nm_setting_verify (NM_SETTING (priv->setting), NULL, error);
}

static void
ce_page_proxy_init (CEPageProxy *self)
{
}

static void
ce_page_proxy_class_init (CEPageProxyClass *proxy_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (proxy_class);
	CEPageClass *parent_class = CE_PAGE_CLASS (proxy_class);

	g_type_class_add_private (object_class, sizeof (CEPageProxyPrivate));

	/* virtual methods */
	parent_class->ce_page_validate_v = ce_page_validate_v;
}
