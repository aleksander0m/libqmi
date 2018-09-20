/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * libqmi-glib -- GLib/GIO based library to control QMI devices
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2018 Aleksander Morgado <aleksander@aleksander.es>
 */

#ifndef _LIBQMI_GLIB_QMI_ENUMS_QOS_H_
#define _LIBQMI_GLIB_QMI_ENUMS_QOS_H_

#if !defined (__LIBQMI_GLIB_H_INSIDE__) && !defined (LIBQMI_GLIB_COMPILATION)
#error "Only <libqmi-glib.h> can be included directly."
#endif

/**
 * SECTION: qmi-enums-qos
 * @title: QOS enumerations and flags
 *
 * This section defines enumerations and flags used in the QOS service
 * interface.
 */

/**
 * QmiQosStatus:
 * @QMI_QOS_STATUS_UNKNOWN: Unknown.
 * @QMI_QOS_STATUS_ACTIVATED: Activated.
 * @QMI_QOS_STATUS_SUSPENDED: Suspended.
 * @QMI_QOS_STATUS_GONE: Gone.
 *
 * QoS flow status.
 *
 * Since: 1.22
 */
typedef enum {
    QMI_QOS_STATUS_UNKNOWN   = 0,
    QMI_QOS_STATUS_ACTIVATED = 1,
    QMI_QOS_STATUS_SUSPENDED = 2,
    QMI_QOS_STATUS_GONE      = 3,
} QmiQosStatus;

/**
 * qmi_qos_status_get_string:
 *
 * Since: 1.22
 */

#endif /* _LIBQMI_GLIB_QMI_ENUMS_QOS_H_ */
