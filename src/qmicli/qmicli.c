/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * qmicli -- Command line interface to control QMI devices
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
 * Copyright (C) 2012-2017 Aleksander Morgado <aleksander@aleksander.es>
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gio/gio.h>
#include <glib-unix.h>

#include <libqmi-glib.h>

#if defined MBIM_QMUX_ENABLED
#include <libmbim-glib.h>
#endif

#include "qmicli.h"
#include "qmicli-helpers.h"

#define PROGRAM_NAME    "qmicli"
#define PROGRAM_VERSION PACKAGE_VERSION

/* Globals */
static GMainLoop *loop;
static GCancellable *cancellable;
static QmiDevice *device;
static QmiClient *client;
static QmiService service;
static gboolean operation_status;
static gboolean expect_indications;

/* Main options */
static gchar *device_str;
static gboolean get_service_version_info_flag;
static gboolean get_wwan_iface_flag;
static gboolean get_expected_data_format_flag;
static gchar *set_expected_data_format_str;
static gchar *device_set_instance_id_str;
static gboolean device_open_version_info_flag;
static gboolean device_open_sync_flag;
static gchar *device_open_net_str;
static gboolean device_open_proxy_flag;
static gboolean device_open_qmi_flag;
static gboolean device_open_mbim_flag;
static gboolean device_open_auto_flag;
static gchar *client_cid_str;
static gboolean client_no_release_cid_flag;
static gboolean verbose_flag;
static gboolean silent_flag;
static gboolean version_flag;

static GOptionEntry main_entries[] = {
    { "device", 'd', 0, G_OPTION_ARG_STRING, &device_str,
      "Specify device path",
      "[PATH]"
    },
    { "get-wwan-iface", 'w', 0, G_OPTION_ARG_NONE, &get_wwan_iface_flag,
      "Get the WWAN iface name associated with this control port",
      NULL
    },
    { "get-expected-data-format", 'e', 0, G_OPTION_ARG_NONE, &get_expected_data_format_flag,
      "Get the expected data format in the WWAN iface",
      NULL
    },
    { "set-expected-data-format", 'E', 0, G_OPTION_ARG_STRING, &set_expected_data_format_str,
      "Set the expected data format in the WWAN iface",
      "[802-3|raw-ip]"
    },
    { "get-service-version-info", 0, 0, G_OPTION_ARG_NONE, &get_service_version_info_flag,
      "Get service version info",
      NULL
    },
    { "device-set-instance-id", 0, 0, G_OPTION_ARG_STRING, &device_set_instance_id_str,
      "Set instance ID",
      "[Instance ID]"
    },
    { "device-open-version-info", 0, 0, G_OPTION_ARG_NONE, &device_open_version_info_flag,
      "Run version info check when opening device",
      NULL
    },
    { "device-open-sync", 0, 0, G_OPTION_ARG_NONE, &device_open_sync_flag,
      "Run sync operation when opening device",
      NULL
    },
    { "device-open-proxy", 'p', 0, G_OPTION_ARG_NONE, &device_open_proxy_flag,
      "Request to use the 'qmi-proxy' proxy",
      NULL
    },
    { "device-open-qmi", 0, 0, G_OPTION_ARG_NONE, &device_open_qmi_flag,
      "Open a cdc-wdm device explicitly in QMI mode",
      NULL
    },
    { "device-open-mbim", 0, 0, G_OPTION_ARG_NONE, &device_open_mbim_flag,
      "Open a cdc-wdm device explicitly in MBIM mode",
      NULL
    },
    { "device-open-auto", 0, 0, G_OPTION_ARG_NONE, &device_open_auto_flag,
      "Open a cdc-wdm device in either QMI or MBIM mode (default)",
      NULL
    },
    { "device-open-net", 0, 0, G_OPTION_ARG_STRING, &device_open_net_str,
      "Open device with specific link protocol and QoS flags",
      "[net-802-3|net-raw-ip|net-qos-header|net-no-qos-header]"
    },
    { "client-cid", 0, 0, G_OPTION_ARG_STRING, &client_cid_str,
      "Use the given CID, don't allocate a new one",
      "[CID]"
    },
    { "client-no-release-cid", 0, 0, G_OPTION_ARG_NONE, &client_no_release_cid_flag,
      "Do not release the CID when exiting",
      NULL
    },
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose_flag,
      "Run action with verbose logs, including the debug ones",
      NULL
    },
    { "silent", 0, 0, G_OPTION_ARG_NONE, &silent_flag,
      "Run action with no logs; not even the error/warning ones",
      NULL
    },
    { "version", 'V', 0, G_OPTION_ARG_NONE, &version_flag,
      "Print version",
      NULL
    },
    { NULL }
};

static gboolean
signals_handler (void)
{
    if (cancellable) {
        /* Ignore consecutive requests of cancellation */
        if (!g_cancellable_is_cancelled (cancellable)) {
            g_printerr ("cancelling the operation...\n");
            g_cancellable_cancel (cancellable);
            /* Re-set the signal handler to allow main loop cancellation on
             * second signal */
            return G_SOURCE_CONTINUE;
        }
    }

    if (loop && g_main_loop_is_running (loop)) {
        g_printerr ("cancelling the main loop...\n");
        g_idle_add ((GSourceFunc) g_main_loop_quit, loop);
    }
    return G_SOURCE_REMOVE;
}

static void
log_handler (const gchar *log_domain,
             GLogLevelFlags log_level,
             const gchar *message,
             gpointer user_data)
{
    const gchar *log_level_str;
    time_t now;
    gchar time_str[64];
    struct tm *local_time;
    gboolean err;

    /* Nothing to do if we're silent */
    if (silent_flag)
        return;

    now = time ((time_t *) NULL);
    local_time = localtime (&now);
    strftime (time_str, 64, "%d %b %Y, %H:%M:%S", local_time);
    err = FALSE;

    switch (log_level) {
    case G_LOG_LEVEL_WARNING:
        log_level_str = "-Warning **";
        err = TRUE;
        break;

    case G_LOG_LEVEL_CRITICAL:
    case G_LOG_FLAG_FATAL:
    case G_LOG_LEVEL_ERROR:
        log_level_str = "-Error **";
        err = TRUE;
        break;

    case G_LOG_LEVEL_DEBUG:
        log_level_str = "[Debug]";
        break;

    default:
        log_level_str = "";
        break;
    }

    if (!verbose_flag && !err)
        return;

    g_fprintf (err ? stderr : stdout,
               "[%s] %s %s\n",
               time_str,
               log_level_str,
               message);
}

static void
print_version_and_exit (void)
{
    g_print ("\n"
             PROGRAM_NAME " " PROGRAM_VERSION "\n"
             "Copyright (C) 2012-2018 Aleksander Morgado\n"
             "License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl-2.0.html>\n"
             "This is free software: you are free to change and redistribute it.\n"
             "There is NO WARRANTY, to the extent permitted by law.\n"
             "\n");
    exit (EXIT_SUCCESS);
}

static gboolean
generic_options_enabled (void)
{
    static guint n_actions = 0;
    static gboolean checked = FALSE;

    if (checked)
        return !!n_actions;

    n_actions = (!!device_set_instance_id_str +
                 get_service_version_info_flag +
                 get_wwan_iface_flag +
                 get_expected_data_format_flag +
                 !!set_expected_data_format_str);

    if (n_actions > 1) {
        g_printerr ("error: too many generic actions requested\n");
        exit (EXIT_FAILURE);
    }

    checked = TRUE;
    return !!n_actions;
}

/*****************************************************************************/
/* Report that indications are expected */

void
qmicli_expect_indications (void)
{
    expect_indications = TRUE;
}

/*****************************************************************************/
/* Running asynchronously */

static void
close_ready (QmiDevice    *dev,
             GAsyncResult *res)
{
    GError *error = NULL;

    if (!qmi_device_close_finish (dev, res, &error)) {
        g_printerr ("error: couldn't close: %s\n", error->message);
        g_error_free (error);
    } else
        g_debug ("Closed");

    g_main_loop_quit (loop);
}

static void
release_client_ready (QmiDevice *dev,
                      GAsyncResult *res)
{
    GError *error = NULL;

    if (!qmi_device_release_client_finish (dev, res, &error)) {
        g_printerr ("error: couldn't release client: %s\n", error->message);
        g_error_free (error);
    } else
        g_debug ("Client released");

    qmi_device_close_async (dev, 10, NULL, (GAsyncReadyCallback) close_ready, NULL);
}

void
qmicli_async_operation_done (gboolean reported_operation_status,
                             gboolean skip_cid_release)
{
    QmiDeviceReleaseClientFlags flags = QMI_DEVICE_RELEASE_CLIENT_FLAGS_NONE;

    /* Keep the result of the operation */
    operation_status = reported_operation_status;

    /* Cleanup cancellation */
    g_clear_object (&cancellable);

    /* If no client was allocated (e.g. generic action), just quit */
    if (!client) {
        g_main_loop_quit (loop);
        return;
    }

    if (skip_cid_release)
        g_debug ("Skipped CID release");
    else if (!client_no_release_cid_flag)
        flags |= QMI_DEVICE_RELEASE_CLIENT_FLAGS_RELEASE_CID;
    else
        g_print ("[%s] Client ID not released:\n"
                 "\tService: '%s'\n"
                 "\t    CID: '%u'\n",
                 qmi_device_get_path_display (device),
                 qmi_service_get_string (service),
                 qmi_client_get_cid (client));

    qmi_device_release_client (device,
                               client,
                               flags,
                               10,
                               NULL,
                               (GAsyncReadyCallback)release_client_ready,
                               NULL);
}

static void
allocate_client_ready (QmiDevice *dev,
                       GAsyncResult *res)
{
    GError *error = NULL;

    client = qmi_device_allocate_client_finish (dev, res, &error);
    if (!client) {
        g_printerr ("error: couldn't create client for the '%s' service: %s\n",
                    qmi_service_get_string (service),
                    error->message);
        exit (EXIT_FAILURE);
    }

    /* Run the service-specific action */
    switch (service) {
    case QMI_SERVICE_DMS:
        qmicli_dms_run (dev, QMI_CLIENT_DMS (client), cancellable);
        return;
    case QMI_SERVICE_NAS:
        qmicli_nas_run (dev, QMI_CLIENT_NAS (client), cancellable);
        return;
    case QMI_SERVICE_WDS:
        qmicli_wds_run (dev, QMI_CLIENT_WDS (client), cancellable);
        return;
    case QMI_SERVICE_PBM:
        qmicli_pbm_run (dev, QMI_CLIENT_PBM (client), cancellable);
        return;
    case QMI_SERVICE_PDC:
        qmicli_pdc_run (dev, QMI_CLIENT_PDC (client), cancellable);
        return;
    case QMI_SERVICE_UIM:
        qmicli_uim_run (dev, QMI_CLIENT_UIM (client), cancellable);
        return;
    case QMI_SERVICE_WMS:
        qmicli_wms_run (dev, QMI_CLIENT_WMS (client), cancellable);
        return;
    case QMI_SERVICE_WDA:
        qmicli_wda_run (dev, QMI_CLIENT_WDA (client), cancellable);
        return;
    case QMI_SERVICE_VOICE:
        qmicli_voice_run (dev, QMI_CLIENT_VOICE (client), cancellable);
        return;
    case QMI_SERVICE_LOC:
        qmicli_loc_run (dev, QMI_CLIENT_LOC (client), cancellable);
        return;
    case QMI_SERVICE_QOS:
        qmicli_qos_run (dev, QMI_CLIENT_QOS (client), cancellable);
        return;
    default:
        g_assert_not_reached ();
    }
}

static void
device_allocate_client (QmiDevice *dev)
{
    guint8 cid = QMI_CID_NONE;

    if (client_cid_str) {
        guint32 cid32;

        cid32 = atoi (client_cid_str);
        if (!cid32 || cid32 > G_MAXUINT8) {
            g_printerr ("error: invalid CID given '%s'\n",
                        client_cid_str);
            exit (EXIT_FAILURE);
        }

        cid = (guint8)cid32;
        g_debug ("Reusing CID '%u'", cid);
    }

    /* As soon as we get the QmiDevice, create a client for the requested
     * service */
    qmi_device_allocate_client (dev,
                                service,
                                cid,
                                10,
                                cancellable,
                                (GAsyncReadyCallback)allocate_client_ready,
                                NULL);
}

static void
set_instance_id_ready (QmiDevice *dev,
                       GAsyncResult *res)
{
    GError *error = NULL;
    guint16 link_id;

    if (!qmi_device_set_instance_id_finish (dev, res, &link_id, &error)) {
        g_printerr ("error: couldn't set instance ID: %s\n",
                    error->message);
        exit (EXIT_FAILURE);
    }

    g_print ("[%s] Instance ID set:\n"
             "\tLink ID: '%" G_GUINT16_FORMAT "'\n",
             qmi_device_get_path_display (dev),
             link_id);

    /* We're done now */
    qmicli_async_operation_done (TRUE, FALSE);
}

static void
device_set_instance_id (QmiDevice *dev)
{
    gint instance_id;

    if (g_str_equal (device_set_instance_id_str, "0"))
        instance_id = 0;
    else {
        instance_id = atoi (device_set_instance_id_str);
        if (instance_id == 0) {
            g_printerr ("error: invalid instance ID given: '%s'\n", device_set_instance_id_str);
            exit (EXIT_FAILURE);
        } else if (instance_id < 0 || instance_id > G_MAXUINT8) {
            g_printerr ("error: given instance ID is out of range [0,%u]: '%s'\n",
                        G_MAXUINT8,
                        device_set_instance_id_str);
            exit (EXIT_FAILURE);
        }
    }

    g_debug ("Setting instance ID '%d'...", instance_id);
    qmi_device_set_instance_id (dev,
                                (guint8)instance_id,
                                10,
                                cancellable,
                                (GAsyncReadyCallback)set_instance_id_ready,
                                NULL);
}

static void
get_service_version_info_ready (QmiDevice *dev,
                                GAsyncResult *res)
{
    GError *error = NULL;
    GArray *services;
    guint i;

    services = qmi_device_get_service_version_info_finish (dev, res, &error);
    if (!services) {
        g_printerr ("error: couldn't get service version info: %s\n",
                    error->message);
        exit (EXIT_FAILURE);
    }

    g_print ("[%s] Supported versions:\n",
             qmi_device_get_path_display (dev));
    for (i = 0; i < services->len; i++) {
        QmiDeviceServiceVersionInfo *info;
        const gchar *service_str;

        info = &g_array_index (services, QmiDeviceServiceVersionInfo, i);
        service_str = qmi_service_get_string (info->service);
        if (service_str)
            g_print ("\t%s (%u.%u)\n",
                     service_str,
                     info->major_version,
                     info->minor_version);
        else
            g_print ("\tunknown [0x%02x] (%u.%u)\n",
                     info->service,
                     info->major_version,
                     info->minor_version);
    }
    g_array_unref (services);

    /* We're done now */
    qmicli_async_operation_done (TRUE, FALSE);
}

static void
device_get_service_version_info (QmiDevice *dev)
{
    g_debug ("Getting service version info...");
    qmi_device_get_service_version_info (dev,
                                         10,
                                         cancellable,
                                         (GAsyncReadyCallback)get_service_version_info_ready,
                                         NULL);
}

static gboolean
device_set_expected_data_format_cb (QmiDevice *dev)
{
    QmiDeviceExpectedDataFormat expected;
    GError *error = NULL;

    if (!qmicli_read_expected_data_format_from_string (set_expected_data_format_str, &expected) ||
        expected == QMI_DEVICE_EXPECTED_DATA_FORMAT_UNKNOWN)
        g_printerr ("error: invalid requested data format: %s", set_expected_data_format_str);
    else if (!qmi_device_set_expected_data_format (dev, expected, &error)) {
        g_printerr ("error: cannot set expected data format: %s\n", error->message);
        g_error_free (error);
    } else
        g_print ("[%s] expected data format set to: %s\n",
                 qmi_device_get_path_display (dev),
                 qmi_device_expected_data_format_get_string (expected));

    /* We're done now */
    qmicli_async_operation_done (!error, FALSE);

    g_object_unref (dev);
    return FALSE;
}

static void
device_set_expected_data_format (QmiDevice *dev)
{
    g_debug ("Setting expected WWAN data format this control port...");
    g_idle_add ((GSourceFunc) device_set_expected_data_format_cb, g_object_ref (dev));
}

static gboolean
device_get_expected_data_format_cb (QmiDevice *dev)
{
    QmiDeviceExpectedDataFormat expected;
    GError *error = NULL;

    expected = qmi_device_get_expected_data_format (dev, &error);
    if (expected == QMI_DEVICE_EXPECTED_DATA_FORMAT_UNKNOWN) {
        g_printerr ("error: cannot get expected data format: %s\n", error->message);
        g_error_free (error);
    } else
        g_print ("%s\n", qmi_device_expected_data_format_get_string (expected));

    /* We're done now */
    qmicli_async_operation_done (!error, FALSE);

    g_object_unref (dev);
    return FALSE;
}

static void
device_get_expected_data_format (QmiDevice *dev)
{
    g_debug ("Getting expected WWAN data format this control port...");
    g_idle_add ((GSourceFunc) device_get_expected_data_format_cb, g_object_ref (dev));
}

static gboolean
device_get_wwan_iface_cb (QmiDevice *dev)
{
    const gchar *wwan_iface;

    wwan_iface = qmi_device_get_wwan_iface (dev);
    if (!wwan_iface)
        g_printerr ("error: cannot get WWAN interface name\n");
    else
        g_print ("%s\n", wwan_iface);

    /* We're done now */
    qmicli_async_operation_done (!!wwan_iface, FALSE);

    g_object_unref (dev);
    return FALSE;
}

static void
device_get_wwan_iface (QmiDevice *dev)
{
    g_debug ("Getting WWAN iface for this control port...");
    g_idle_add ((GSourceFunc) device_get_wwan_iface_cb, g_object_ref (dev));
}

static void
device_open_ready (QmiDevice *dev,
                   GAsyncResult *res)
{
    GError *error = NULL;

    if (!qmi_device_open_finish (dev, res, &error)) {
        g_printerr ("error: couldn't open the QmiDevice: %s\n",
                    error->message);
        exit (EXIT_FAILURE);
    }

    g_debug ("QMI Device at '%s' ready",
             qmi_device_get_path_display (dev));

    if (device_set_instance_id_str)
        device_set_instance_id (dev);
    else if (get_service_version_info_flag)
        device_get_service_version_info (dev);
    else if (get_wwan_iface_flag)
        device_get_wwan_iface (dev);
    else if (get_expected_data_format_flag)
        device_get_expected_data_format (dev);
    else if (set_expected_data_format_str)
        device_set_expected_data_format (dev);
    else
        device_allocate_client (dev);
}

static void
device_new_ready (GObject *unused,
                  GAsyncResult *res)
{
    QmiDeviceOpenFlags open_flags = QMI_DEVICE_OPEN_FLAGS_NONE;
    GError *error = NULL;

    device = qmi_device_new_finish (res, &error);
    if (!device) {
        g_printerr ("error: couldn't create QmiDevice: %s\n",
                    error->message);
        exit (EXIT_FAILURE);
    }

    if (device_open_mbim_flag + device_open_qmi_flag + device_open_auto_flag > 1) {
        g_printerr ("error: cannot specify multiple mode flags to open device\n");
        exit (EXIT_FAILURE);
    }

    /* Setup device open flags */
    if (device_open_version_info_flag)
        open_flags |= QMI_DEVICE_OPEN_FLAGS_VERSION_INFO;
    if (device_open_sync_flag)
        open_flags |= QMI_DEVICE_OPEN_FLAGS_SYNC;
    if (device_open_proxy_flag)
        open_flags |= QMI_DEVICE_OPEN_FLAGS_PROXY;
    if (device_open_mbim_flag)
        open_flags |= QMI_DEVICE_OPEN_FLAGS_MBIM;
    if (device_open_auto_flag || (!device_open_qmi_flag && !device_open_mbim_flag))
        open_flags |= QMI_DEVICE_OPEN_FLAGS_AUTO;
    if (expect_indications)
        open_flags |= QMI_DEVICE_OPEN_FLAGS_EXPECT_INDICATIONS;
    if (device_open_net_str)
        if (!qmicli_read_net_open_flags_from_string (device_open_net_str, &open_flags))
            exit (EXIT_FAILURE);

    /* Open the device */
    qmi_device_open (device,
                     open_flags,
                     15,
                     cancellable,
                     (GAsyncReadyCallback)device_open_ready,
                     NULL);
}

/*****************************************************************************/

static void
parse_actions (void)
{
    guint actions_enabled = 0;

    /* Generic options? */
    if (generic_options_enabled ()) {
        service = QMI_SERVICE_CTL;
        actions_enabled++;
    }

    /* DMS options? */
    if (qmicli_dms_options_enabled ()) {
        service = QMI_SERVICE_DMS;
        actions_enabled++;
    }

    /* NAS options? */
    if (qmicli_nas_options_enabled ()) {
        service = QMI_SERVICE_NAS;
        actions_enabled++;
    }

    /* WDS options? */
    if (qmicli_wds_options_enabled ()) {
        service = QMI_SERVICE_WDS;
        actions_enabled++;
    }

    /* PBM options? */
    if (qmicli_pbm_options_enabled ()) {
        service = QMI_SERVICE_PBM;
        actions_enabled++;
    }

    /* PDC options? */
    if (qmicli_pdc_options_enabled ()) {
        service = QMI_SERVICE_PDC;
        actions_enabled++;
    }

    /* UIM options? */
    if (qmicli_uim_options_enabled ()) {
        service = QMI_SERVICE_UIM;
        actions_enabled++;
    }

    /* WMS options? */
    if (qmicli_wms_options_enabled ()) {
        service = QMI_SERVICE_WMS;
        actions_enabled++;
    }

    /* WDA options? */
    if (qmicli_wda_options_enabled ()) {
        service = QMI_SERVICE_WDA;
        actions_enabled++;
    }

    /* VOICE options? */
    if (qmicli_voice_options_enabled ()) {
        service = QMI_SERVICE_VOICE;
        actions_enabled++;
    }

    /* LOC options? */
    if (qmicli_loc_options_enabled ()) {
        service = QMI_SERVICE_LOC;
        actions_enabled++;
    }

    /* QOS options? */
    if (qmicli_qos_options_enabled ()) {
        service = QMI_SERVICE_QOS;
        actions_enabled++;
    }

    /* Cannot mix actions from different services */
    if (actions_enabled > 1) {
        g_printerr ("error: cannot execute multiple actions of different services\n");
        exit (EXIT_FAILURE);
    }

    /* No options? */
    if (actions_enabled == 0) {
        g_printerr ("error: no actions specified\n");
        exit (EXIT_FAILURE);
    }

    /* Go on! */
}

int main (int argc, char **argv)
{
    GError *error = NULL;
    GFile *file;
    GOptionContext *context;

    setlocale (LC_ALL, "");

    /* Setup option context, process it and destroy it */
    context = g_option_context_new ("- Control QMI devices");
    g_option_context_add_group (context,
                                qmicli_dms_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_nas_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_wds_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_pbm_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_pdc_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_uim_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_wms_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_wda_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_voice_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_loc_get_option_group ());
    g_option_context_add_group (context,
                                qmicli_qos_get_option_group ());
    g_option_context_add_main_entries (context, main_entries, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_printerr ("error: %s\n",
                    error->message);
        exit (EXIT_FAILURE);
    }
    g_option_context_free (context);

    if (version_flag)
        print_version_and_exit ();

    g_log_set_handler (NULL, G_LOG_LEVEL_MASK, log_handler, NULL);
    g_log_set_handler ("Qmi", G_LOG_LEVEL_MASK, log_handler, NULL);
    if (verbose_flag)
        qmi_utils_set_traces_enabled (TRUE);

#if defined MBIM_QMUX_ENABLED
    /* libmbim logging */
    g_log_set_handler ("Mbim", G_LOG_LEVEL_MASK, log_handler, NULL);
    if (verbose_flag)
        mbim_utils_set_traces_enabled (TRUE);
#endif

    /* No device path given? */
    if (!device_str) {
        g_printerr ("error: no device path specified\n");
        exit (EXIT_FAILURE);
    }

    /* Build new GFile from the commandline arg */
    file = g_file_new_for_commandline_arg (device_str);

    parse_actions ();

    /* Create requirements for async options */
    cancellable = g_cancellable_new ();
    loop = g_main_loop_new (NULL, FALSE);

    /* Setup signals */
    g_unix_signal_add (SIGINT,  (GSourceFunc) signals_handler, NULL);
    g_unix_signal_add (SIGHUP,  (GSourceFunc) signals_handler, NULL);
    g_unix_signal_add (SIGTERM, (GSourceFunc) signals_handler, NULL);

    /* Launch QmiDevice creation */
    qmi_device_new (file,
                    cancellable,
                    (GAsyncReadyCallback)device_new_ready,
                    NULL);
    g_main_loop_run (loop);

    if (cancellable)
        g_object_unref (cancellable);
    if (client)
        g_object_unref (client);
    if (device)
        g_object_unref (device);
    g_main_loop_unref (loop);
    g_object_unref (file);

    return (operation_status ? EXIT_SUCCESS : EXIT_FAILURE);
}
