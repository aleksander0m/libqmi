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
 * Copyright (C) 2018 Aleksander Morgado <aleksander@aleksander.es>
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#include <glib.h>
#include <gio/gio.h>

#include <libqmi-glib.h>

#include "qmicli.h"
#include "qmicli-helpers.h"

#undef VALIDATE_UNKNOWN
#define VALIDATE_UNKNOWN(str) (str ? str : "unknown")

/* Context */
typedef struct {
    QmiDevice *device;
    QmiClientQos *client;
    GCancellable *cancellable;
} Context;
static Context *ctx;

/* Options */
static gint     get_flow_status_int = -1;
static gint     get_granted_int = -1;
static gboolean get_network_status_flag;
static gboolean reset_flag;
static gboolean noop_flag;

static GOptionEntry entries[] = {
    { "qos-get-flow-status", 0, 0, G_OPTION_ARG_INT, &get_flow_status_int,
      "Get QoS flow status",
      "[QoS ID]"
    },
    { "qos-get-granted", 0, 0, G_OPTION_ARG_INT, &get_granted_int,
      "Get granted QoS parameters",
      "[QoS ID]"
    },
    { "qos-get-network-status", 0, 0, G_OPTION_ARG_NONE, &get_network_status_flag,
      "Gets the network status",
      NULL
    },
    { "qos-reset", 0, 0, G_OPTION_ARG_NONE, &reset_flag,
      "Reset the service state",
      NULL
    },
    { "qos-noop", 0, 0, G_OPTION_ARG_NONE, &noop_flag,
      "Just allocate or release a QOS client. Use with `--client-no-release-cid' and/or `--client-cid'",
      NULL
    },
    { NULL }
};

GOptionGroup *
qmicli_qos_get_option_group (void)
{
    GOptionGroup *group;

    group = g_option_group_new ("qos",
                                "QoS options",
                                "Show Quality of Service options",
                                NULL,
                                NULL);
    g_option_group_add_entries (group, entries);

    return group;
}

gboolean
qmicli_qos_options_enabled (void)
{
    static guint n_actions = 0;
    static gboolean checked = FALSE;

    if (checked)
        return !!n_actions;

    n_actions = ((get_flow_status_int >= 0) +
                 (get_granted_int >= 0) +
                 get_network_status_flag +
                 reset_flag +
                 noop_flag);

    if (n_actions > 1) {
        g_printerr ("error: too many QoS actions requested\n");
        exit (EXIT_FAILURE);
    }

    checked = TRUE;
    return !!n_actions;
}

static void
context_free (Context *context)
{
    if (!context)
        return;

    if (context->cancellable)
        g_object_unref (context->cancellable);
    if (context->device)
        g_object_unref (context->device);
    if (context->client)
        g_object_unref (context->client);
    g_slice_free (Context, context);
}

static void
operation_shutdown (gboolean operation_status)
{
    /* Cleanup context and finish async operation */
    context_free (ctx);
    qmicli_async_operation_done (operation_status, FALSE);
}

static void
get_flow_status_ready (QmiClientQos *client,
                       GAsyncResult *res)
{
    QmiMessageQosGetFlowStatusOutput *output;
    GError *error = NULL;
    QmiQosStatus flow_status;

    output = qmi_client_qos_get_flow_status_finish (client, res, &error);
    if (!output) {
        g_printerr ("error: operation failed: %s\n", error->message);
        g_error_free (error);
        operation_shutdown (FALSE);
        return;
    }

    if (!qmi_message_qos_get_flow_status_output_get_result (output, &error)) {
        g_printerr ("error: couldn't get QoS flow status: %s\n", error->message);
        g_error_free (error);
        qmi_message_qos_get_flow_status_output_unref (output);
        operation_shutdown (FALSE);
        return;
    }

    qmi_message_qos_get_flow_status_output_get_value (output, &flow_status, NULL);

    g_print ("[%s] QoS flow status: %s\n",
             qmi_device_get_path_display (ctx->device),
             qmi_qos_status_get_string (flow_status));

    qmi_message_qos_get_flow_status_output_unref (output);
    operation_shutdown (TRUE);
}

static void
get_granted_ready (QmiClientQos *client,
                   GAsyncResult *res)
{
    QmiMessageQosGetGrantedOutput *output;
    GError *error = NULL;

    output = qmi_client_qos_get_granted_finish (client, res, &error);
    if (!output) {
        g_printerr ("error: operation failed: %s\n", error->message);
        g_error_free (error);
        operation_shutdown (FALSE);
        return;
    }

    if (!qmi_message_qos_get_granted_output_get_result (output, &error)) {
        g_printerr ("error: couldn't get granted QoS parameters: %s\n", error->message);
        g_error_free (error);
        qmi_message_qos_get_granted_output_unref (output);
        operation_shutdown (FALSE);
        return;
    }

    g_print ("[%s] Successfully retrieved granted QoS parameters\n",
             qmi_device_get_path_display (ctx->device));

    /* TODO */

    qmi_message_qos_get_granted_output_unref (output);
    operation_shutdown (TRUE);
}

static void
get_network_status_ready (QmiClientQos *client,
                          GAsyncResult *res)
{
    QmiMessageQosGetNetworkStatusOutput *output;
    GError *error = NULL;
    gboolean qos_supported;

    output = qmi_client_qos_get_network_status_finish (client, res, &error);
    if (!output) {
        g_printerr ("error: operation failed: %s\n", error->message);
        g_error_free (error);
        operation_shutdown (FALSE);
        return;
    }

    if (!qmi_message_qos_get_network_status_output_get_result (output, &error)) {
        g_printerr ("error: couldn't get network status: %s\n", error->message);
        g_error_free (error);
        qmi_message_qos_get_network_status_output_unref (output);
        operation_shutdown (FALSE);
        return;
    }

    qmi_message_qos_get_network_status_output_get_qos_supported (output, &qos_supported, NULL);

    g_print ("[%s] QoS %ssupported in network\n",
             qmi_device_get_path_display (ctx->device),
             qos_supported ? "" : "not ");

    qmi_message_qos_get_network_status_output_unref (output);
    operation_shutdown (TRUE);
}

static void
reset_ready (QmiClientQos *client,
             GAsyncResult *res)
{
    QmiMessageQosResetOutput *output;
    GError *error = NULL;

    output = qmi_client_qos_reset_finish (client, res, &error);
    if (!output) {
        g_printerr ("error: operation failed: %s\n", error->message);
        g_error_free (error);
        operation_shutdown (FALSE);
        return;
    }

    if (!qmi_message_qos_reset_output_get_result (output, &error)) {
        g_printerr ("error: couldn't reset the QoS service: %s\n", error->message);
        g_error_free (error);
        qmi_message_qos_reset_output_unref (output);
        operation_shutdown (FALSE);
        return;
    }

    g_print ("[%s] Successfully performed QoS service reset\n",
             qmi_device_get_path_display (ctx->device));

    qmi_message_qos_reset_output_unref (output);
    operation_shutdown (TRUE);
}

static gboolean
noop_cb (gpointer unused)
{
    operation_shutdown (TRUE);
    return FALSE;
}

void
qmicli_qos_run (QmiDevice *device,
                QmiClientQos *client,
                GCancellable *cancellable)
{
    /* Initialize context */
    ctx = g_slice_new (Context);
    ctx->device = g_object_ref (device);
    ctx->client = g_object_ref (client);
    ctx->cancellable = g_object_ref (cancellable);

    /* Request to get QoS flow status? */
    if (get_flow_status_int >= 0) {
        QmiMessageQosGetFlowStatusInput *input;

        input = qmi_message_qos_get_flow_status_input_new ();
        qmi_message_qos_get_flow_status_input_set_qos_id (input, get_flow_status_int, NULL);
        g_debug ("Asynchronously getting QoS flow status...");
        qmi_client_qos_get_flow_status (ctx->client,
                                        input,
                                        10,
                                        ctx->cancellable,
                                        (GAsyncReadyCallback)get_flow_status_ready,
                                        NULL);
        qmi_message_qos_get_flow_status_input_unref (input);
        return;
    }

    /* Request to get granted QoS parameters? */
    if (get_granted_int >= 0) {
        QmiMessageQosGetGrantedInput *input;

        input = qmi_message_qos_get_granted_input_new ();
        qmi_message_qos_get_granted_input_set_qos_id (input, get_granted_int, NULL);
        g_debug ("Asynchronously getting granted QoS parameters...");
        qmi_client_qos_get_granted (ctx->client,
                                    input,
                                    10,
                                    ctx->cancellable,
                                    (GAsyncReadyCallback)get_granted_ready,
                                    NULL);
        qmi_message_qos_get_granted_input_unref (input);
        return;
    }

    /* Request to get network status? */
    if (get_network_status_flag) {
        g_debug ("Asynchronously getting network status...");
        qmi_client_qos_get_network_status (ctx->client,
                                           NULL,
                                           10,
                                           ctx->cancellable,
                                           (GAsyncReadyCallback)get_network_status_ready,
                                           NULL);
        return;
    }

    /* Request to reset QoS service? */
    if (reset_flag) {
        g_debug ("Asynchronously resetting QoS service...");
        qmi_client_qos_reset (ctx->client,
                              NULL,
                              10,
                              ctx->cancellable,
                              (GAsyncReadyCallback)reset_ready,
                              NULL);
        return;
    }

    /* Just client allocate/release? */
    if (noop_flag) {
        g_idle_add (noop_cb, NULL);
        return;
    }

    g_warn_if_reached ();
}
