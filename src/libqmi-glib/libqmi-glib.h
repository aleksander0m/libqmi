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
 * Copyright (C) 2012 Google, Inc.
 * Copyright (C) 2012-2017 Aleksander Morgado <aleksander@aleksander.es>
 */

#ifndef _LIBQMI_GLIB_H_
#define _LIBQMI_GLIB_H_

#define __LIBQMI_GLIB_H_INSIDE__

/* libqmi-glib headers */

#include "qmi-version.h"
#include "qmi-device.h"
#include "qmi-client.h"
#include "qmi-proxy.h"
#include "qmi-message.h"
#include "qmi-message-context.h"
#include "qmi-enums.h"
#include "qmi-utils.h"

#include "qmi-compat.h"

#include "qmi-enums-dms.h"
#include "qmi-flags64-dms.h"
#include "qmi-dms.h"

#include "qmi-flags64-nas.h"
#include "qmi-enums-nas.h"
#include "qmi-nas.h"

#include "qmi-enums-wds.h"
#include "qmi-wds.h"

#include "qmi-enums-wms.h"
#include "qmi-wms.h"

#include "qmi-enums-pds.h"
#include "qmi-pds.h"

#include "qmi-enums-pdc.h"
#include "qmi-pdc.h"

#include "qmi-enums-pbm.h"
#include "qmi-pbm.h"

#include "qmi-enums-uim.h"
#include "qmi-uim.h"

#include "qmi-enums-oma.h"
#include "qmi-oma.h"

#include "qmi-enums-wda.h"
#include "qmi-wda.h"

#include "qmi-enums-voice.h"
#include "qmi-voice.h"

#include "qmi-flags64-loc.h"
#include "qmi-enums-loc.h"
#include "qmi-loc.h"

#include "qmi-enums-qos.h"
#include "qmi-qos.h"

/* generated */
#include "qmi-error-types.h"
#include "qmi-enum-types.h"
#include "qmi-flags64-types.h"

#endif /* _LIBQMI_GLIB_H_ */
