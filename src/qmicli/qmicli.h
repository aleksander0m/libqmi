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

#include <glib.h>

#ifndef __QMICLI_H__
#define __QMICLI_H__

/* Common */
void          qmicli_async_operation_done (gboolean reported_operation_status,
                                           gboolean skip_cid_release);
void          qmicli_expect_indications   (void);

/* DMS group */
GOptionGroup *qmicli_dms_get_option_group (void);
gboolean      qmicli_dms_options_enabled  (void);
void          qmicli_dms_run              (QmiDevice *device,
                                           QmiClientDms *client,
                                           GCancellable *cancellable);

/* WDS group */
GOptionGroup *qmicli_wds_get_option_group (void);
gboolean      qmicli_wds_options_enabled  (void);
void          qmicli_wds_run              (QmiDevice *device,
                                           QmiClientWds *client,
                                           GCancellable *cancellable);

/* NAS group */
GOptionGroup *qmicli_nas_get_option_group (void);
gboolean      qmicli_nas_options_enabled  (void);
void          qmicli_nas_run              (QmiDevice *device,
                                           QmiClientNas *client,
                                           GCancellable *cancellable);

/* PBM group */
GOptionGroup *qmicli_pbm_get_option_group (void);
gboolean      qmicli_pbm_options_enabled  (void);
void          qmicli_pbm_run              (QmiDevice *device,
                                           QmiClientPbm *client,
                                           GCancellable *cancellable);

/* PDC group */
GOptionGroup *qmicli_pdc_get_option_group (void);
gboolean      qmicli_pdc_options_enabled  (void);
void          qmicli_pdc_run              (QmiDevice *device,
                                           QmiClientPdc *client,
                                           GCancellable *cancellable);

/* UIM group */
GOptionGroup *qmicli_uim_get_option_group (void);
gboolean      qmicli_uim_options_enabled  (void);
void          qmicli_uim_run              (QmiDevice *device,
                                           QmiClientUim *client,
                                           GCancellable *cancellable);

/* WMS group */
GOptionGroup *qmicli_wms_get_option_group (void);
gboolean      qmicli_wms_options_enabled  (void);
void          qmicli_wms_run              (QmiDevice *device,
                                           QmiClientWms *client,
                                           GCancellable *cancellable);

/* WDA group */
GOptionGroup *qmicli_wda_get_option_group (void);
gboolean      qmicli_wda_options_enabled  (void);
void          qmicli_wda_run              (QmiDevice *device,
                                           QmiClientWda *client,
                                           GCancellable *cancellable);

/* Voice group */
GOptionGroup *qmicli_voice_get_option_group (void);
gboolean      qmicli_voice_options_enabled  (void);
void          qmicli_voice_run              (QmiDevice *device,
                                             QmiClientVoice *client,
                                             GCancellable *cancellable);

/* Location group */
GOptionGroup *qmicli_loc_get_option_group (void);
gboolean      qmicli_loc_options_enabled  (void);
void          qmicli_loc_run              (QmiDevice *device,
                                           QmiClientLoc *client,
                                           GCancellable *cancellable);

/* QoS group */
GOptionGroup *qmicli_qos_get_option_group (void);
gboolean      qmicli_qos_options_enabled  (void);
void          qmicli_qos_run              (QmiDevice *device,
                                           QmiClientQos *client,
                                           GCancellable *cancellable);

#endif /* __QMICLI_H__ */
