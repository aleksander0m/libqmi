/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * qmi-firmware-update -- Command line tool to update firmware in QMI devices
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
 * Copyright (C) 2016 Zodiac Inflight Innovation
 * Copyright (C) 2016 Aleksander Morgado <aleksander@aleksander.es>
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gio/gio.h>

#include <libqmi-glib.h>

#include "qfu-operation.h"
#include "qfu-udev-helpers.h"

#define PROGRAM_NAME    "qmi-firmware-update"
#define PROGRAM_VERSION PACKAGE_VERSION

/*****************************************************************************/
/* Options */

/* Generic device selections */
static guint      busnum;
static guint      devnum;
static guint16    vid;
static guint16    pid;

/* Update */
static gboolean   action_update_flag;
static gchar     *device_str;
static gchar     *firmware_version_str;
static gchar     *config_version_str;
static gchar     *carrier_str;
static gboolean   device_open_proxy_flag;
static gboolean   device_open_mbim_flag;

/* Update (QDL mode) */
static gboolean   action_update_qdl_flag;
static gchar     *serial_str;

/* Verify */
static gboolean   action_verify_flag;

/* Main */
static gchar    **image_strv;
static gboolean   verbose_flag;
static gboolean   silent_flag;
static gboolean   version_flag;
static gboolean   help_flag;

static gboolean
parse_busnum_devnum (const gchar  *option_name,
                     const gchar  *value,
                     gpointer      data,
                     GError      **error)
{
    gchar    **strv;
    gint       busnum_idx = -1;
    gint       devnum_idx = 0;
    gulong     aux;
    gboolean   result = FALSE;

    strv = g_strsplit (value, ":", -1);
    g_assert (strv[0]);
    if (strv[1]) {
        busnum_idx = 0;
        devnum_idx = 1;
        if (strv[2]) {
            g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                         "invalid busnum-devnum string: too many fields");
            goto out;
        }
    }

    if (busnum_idx != -1) {
        aux = strtoul (strv[busnum_idx], NULL, 10);
        if (aux == 0 || aux > G_MAXUINT) {
            g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                         "invalid bus number: %s", strv[busnum_idx]);
            goto out;
        }
        busnum = (guint) aux;
    }

    aux = strtoul (strv[devnum_idx], NULL, 10);
    if (aux == 0 || aux > G_MAXUINT) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "invalid dev number: %s", strv[devnum_idx]);
        goto out;
    }
    devnum = (guint) aux;
    result = TRUE;

out:
    g_strfreev (strv);
    return result;
}

static gboolean
parse_vid_pid (const gchar  *option_name,
               const gchar  *value,
               gpointer      data,
               GError      **error)
{
    gchar    **strv;
    gint       vid_idx = 0;
    gint       pid_idx = -1;
    gulong     aux;
    gboolean   result = FALSE;

    strv = g_strsplit (value, ":", -1);
    g_assert (strv[0]);
    if (strv[1]) {
        pid_idx = 1;
        if (strv[2]) {
            g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                         "invalid vid-pid string: too many fields");
            goto out;
        }
    }

    if (pid_idx != -1) {
        aux = strtoul (strv[pid_idx], NULL, 16);
        if (aux == 0 || aux > G_MAXUINT16) {
            g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                         "invalid product id: %s", strv[pid_idx]);
            goto out;
        }
        pid = (guint) aux;
    }

    aux = strtoul (strv[vid_idx], NULL, 16);
    if (aux == 0 || aux > G_MAXUINT16) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "invalid vendor id: %s", strv[vid_idx]);
        goto out;
    }
    vid = (guint16) aux;
    result = TRUE;

out:
    g_strfreev (strv);
    return result;
}

static GOptionEntry context_selection_entries[] = {
    { "busnum-devnum", 'N', 0, G_OPTION_ARG_CALLBACK, &parse_busnum_devnum,
      "Select device by bus and device number (in decimal).",
      "[BUS:]DEV"
    },
    { "vid-pid", 'D', 0, G_OPTION_ARG_CALLBACK, &parse_vid_pid,
      "Select device by device vendor and product id (in hexadecimal).",
      "VID:[PID]"
    },
    { NULL }
};

static GOptionEntry context_update_entries[] = {
    { "update", 'u', 0, G_OPTION_ARG_NONE, &action_update_flag,
      "Launch firmware update process.",
      NULL
    },
    { "device", 'd', 0, G_OPTION_ARG_FILENAME, &device_str,
      "Specify cdc-wdm device path (e.g. /dev/cdc-wdm0).",
      "[PATH]"
    },
    { "firmware-version", 'f', 0, G_OPTION_ARG_STRING, &firmware_version_str,
      "Firmware version (e.g. '05.05.58.00').",
      "[VERSION]",
    },
    { "config-version", 'c', 0, G_OPTION_ARG_STRING, &config_version_str,
      "Config version (e.g. '005.025_002').",
      "[VERSION]",
    },
    { "carrier", 'C', 0, G_OPTION_ARG_STRING, &carrier_str,
      "Carrier name (e.g. 'Generic')",
      "[CARRIER]",
    },
    { "device-open-proxy", 'p', 0, G_OPTION_ARG_NONE, &device_open_proxy_flag,
      "Request to use the 'qmi-proxy' proxy.",
      NULL
    },
    { "device-open-mbim", 0, 0, G_OPTION_ARG_NONE, &device_open_mbim_flag,
      "Open an MBIM device with EXT_QMUX support.",
      NULL
    },
    { NULL }
};

static GOptionEntry context_update_qdl_entries[] = {
    { "update-qdl", 'U', 0, G_OPTION_ARG_NONE, &action_update_qdl_flag,
      "Launch firmware update process in QDL mode.",
      NULL
    },
    { "serial", 's', 0, G_OPTION_ARG_FILENAME, &serial_str,
      "Specify QDL serial device path (e.g. /dev/ttyUSB0).",
      "[PATH]"
    },
    { NULL }
};

static GOptionEntry context_verify_entries[] = {
    { "verify", 'z', 0, G_OPTION_ARG_NONE, &action_verify_flag,
      "Analyze and Verify firmware images.",
      NULL
    },
    { NULL }
};

static GOptionEntry context_main_entries[] = {
    { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &image_strv, "",
      "FILE1 FILE2..."
    },
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose_flag,
      "Run action with verbose logs, including the debug ones.",
      NULL
    },
    { "silent", 0, 0, G_OPTION_ARG_NONE, &silent_flag,
      "Run action with no logs; not even the error/warning ones.",
      NULL
    },
    { "version", 'V', 0, G_OPTION_ARG_NONE, &version_flag,
      "Print version.",
      NULL
    },
    { "help", 'h', 0, G_OPTION_ARG_NONE, &help_flag,
      "Show help",
      NULL
    },
    { NULL }
};

static const gchar *context_description =
    " E.g. an update operation:\n"
    " $ sudo " PROGRAM_NAME " \\\n"
    "       --update \\\n"
    "       --device /dev/cdc-wdm4 \\\n"
    "       --firmware-version 05.05.58.00 \\\n"
    "       --config-version 005.025_002 \\\n"
    "       --carrier Generic \\\n"
    "       SWI9X15C_05.05.58.00.cwe \\\n"
    "       SWI9X15C_05.05.58.00_Generic_005.025_002.nvu\n"
    "\n"
    " E.g. a verify operation:\n"
    " $ sudo " PROGRAM_NAME " \\\n"
    "       --verify \\\n"
    "       SWI9X15C_05.05.58.00.cwe \\\n"
    "       SWI9X15C_05.05.58.00_Generic_005.025_002.nvu\n"
    "\n";

/*****************************************************************************/
/* Logging output */

static void
log_handler (const gchar    *log_domain,
             GLogLevelFlags  log_level,
             const gchar    *message,
             gpointer        user_data)
{
    const gchar *log_level_str;
    time_t       now;
    gchar        time_str[64];
    struct tm   *local_time;
    gboolean     err;

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

/*****************************************************************************/

static void
print_version (void)
{
    g_print ("\n"
             PROGRAM_NAME " " PROGRAM_VERSION "\n"
             "Copyright (C) 2016 Bjørn Mork\n"
             "Copyright (C) 2016 Zodiac Inflight Innovations\n"
             "Copyright (C) 2016 Aleksander Morgado\n"
             "License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl-2.0.html>\n"
             "This is free software: you are free to change and redistribute it.\n"
             "There is NO WARRANTY, to the extent permitted by law.\n"
             "\n");
}

static void
print_help (GOptionContext *context)
{
    gchar *str;

    /* Always print --help-all */
    str = g_option_context_get_help (context, FALSE, NULL);
    g_print ("%s", str);
    g_free (str);
}

static gchar *
select_path (const char              *manual,
             QfuUdevHelperDeviceType  type)
{
    gchar  *path = NULL;
    GError *error = NULL;
    gchar  *sysfs_path = NULL;
    GList  *list = NULL;

    if (manual && (vid != 0 || pid != 0)) {
        g_printerr ("error: cannot specify device path and vid:pid lookup\n");
        return NULL;
    }

    if (manual && (busnum != 0 || devnum != 0)) {
        g_printerr ("error: cannot specify device path and busnum:devnum lookup\n");
        return NULL;
    }

    if ((vid != 0 || pid != 0) && (busnum != 0 || devnum != 0)) {
        g_printerr ("error: cannot specify busnum:devnum and vid:pid lookups\n");
        return NULL;
    }

    if (manual) {
        path = g_strdup (manual);
        goto out;
    }

    /* lookup sysfs path */
    sysfs_path = qfu_udev_helper_find_by_device_info (vid, pid, busnum, devnum, &error);
    if (!sysfs_path) {
        g_printerr ("error: %s\n", error->message);
        g_error_free (error);
        goto out;
    }

    list = qfu_udev_helper_list_devices (type, sysfs_path);
    if (!list) {
        g_printerr ("error: no devices found in sysfs path: %s\n", sysfs_path);
        goto out;
    }

    path = g_file_get_path (G_FILE (list->data));

out:
    if (list)
        g_list_free_full (list, (GDestroyNotify) g_object_unref);

    return path;
}

int main (int argc, char **argv)
{
    GError         *error = NULL;
    GOptionContext *context;
    GOptionGroup   *group;
    guint           n_actions;
    gboolean        result = FALSE;

    setlocale (LC_ALL, "");

    g_type_init ();

    /* Setup option context, process it and destroy it */
    context = g_option_context_new ("- Update firmware in QMI devices");

    group = g_option_group_new ("selection", "Generic device selection options", "", NULL, NULL);
    g_option_group_add_entries (group, context_selection_entries);
    g_option_context_add_group (context, group);

    group = g_option_group_new ("update", "Update options", "", NULL, NULL);
    g_option_group_add_entries (group, context_update_entries);
    g_option_context_add_group (context, group);

    group = g_option_group_new ("update-qdl", "Update options (QDL mode)", "", NULL, NULL);
    g_option_group_add_entries (group, context_update_qdl_entries);
    g_option_context_add_group (context, group);

    group = g_option_group_new ("verify", "Verify options", "", NULL, NULL);
    g_option_group_add_entries (group, context_verify_entries);
    g_option_context_add_group (context, group);

    g_option_context_add_main_entries (context, context_main_entries, NULL);
    g_option_context_set_description  (context, context_description);
    g_option_context_set_help_enabled (context, FALSE);

    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_printerr ("error: couldn't parse option context: %s\n", error->message);
        g_error_free (error);
        goto out;
    }

    if (version_flag) {
        print_version ();
        result = TRUE;
        goto out;
    }

    if (help_flag) {
        print_help (context);
        result = TRUE;
        goto out;
    }

    g_log_set_handler (NULL, G_LOG_LEVEL_MASK, log_handler, NULL);
    g_log_set_handler ("Qmi", G_LOG_LEVEL_MASK, log_handler, NULL);
    if (verbose_flag)
        qmi_utils_set_traces_enabled (TRUE);

    /* We don't allow multiple actions at the same time */
    n_actions = (action_verify_flag + action_update_flag + action_update_qdl_flag);
    if (n_actions == 0) {
        g_printerr ("error: no actions specified\n");
        goto out;
    }
    if (n_actions > 1) {
        g_printerr ("error: too many actions specified\n");
        goto out;
    }

    /* A list of images must always be provided */
    if (!image_strv) {
        g_printerr ("error: no firmware images specified\n");
        goto out;
    }

    /* Run */
    if (action_update_flag) {
        gchar *path;

        path = select_path (device_str, QFU_UDEV_HELPER_DEVICE_TYPE_CDC_WDM);
        if (!path)
            goto out;
        g_debug ("using cdc-wdm device: %s", path);
        result = qfu_operation_update_run ((const gchar **) image_strv,
                                           path,
                                           firmware_version_str,
                                           config_version_str,
                                           carrier_str,
                                           device_open_proxy_flag,
                                           device_open_mbim_flag);
        g_free (path);
    } else if (action_update_qdl_flag) {
        gchar *path;

        path = select_path (serial_str, QFU_UDEV_HELPER_DEVICE_TYPE_TTY);
        if (!path)
            goto out;
        g_debug ("using tty device: %s", path);
        result = qfu_operation_update_qdl_run ((const gchar **) image_strv,
                                               path);
        g_free (path);
    } else if (action_verify_flag)
        result = qfu_operation_verify_run ((const gchar **) image_strv);
    else
        g_assert_not_reached ();

out:
    /* Clean exit for a clean memleak report */
    if (context)
        g_option_context_free (context);

    return (result ? EXIT_SUCCESS : EXIT_FAILURE);
}
