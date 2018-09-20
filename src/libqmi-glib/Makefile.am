
SUBDIRS = generated . test

lib_LTLIBRARIES = libqmi-glib.la

libqmi_glib_la_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(MBIM_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_srcdir)/src/libqmi-glib \
	-I$(top_srcdir)/src/libqmi-glib/generated \
	-I$(top_builddir)/src/libqmi-glib \
	-I$(top_builddir)/src/libqmi-glib/generated \
	-DLIBQMI_GLIB_COMPILATION \
	-DLIBEXEC_PATH=\""$(libexecdir)"\" \
	-DG_LOG_DOMAIN=\"Qmi\"

libqmi_glib_la_SOURCES = \
	libqmi-glib.h \
	qmi-version.h \
	qmi-errors.h \
	qmi-enums-wds.h qmi-enums-wds.c \
	qmi-enums-dms.h qmi-flags64-dms.h qmi-enums-dms.c \
	qmi-enums-nas.h qmi-flags64-nas.h\
	qmi-enums-wms.h \
	qmi-enums-pds.h \
	qmi-enums-pdc.h \
	qmi-enums-pbm.h \
	qmi-enums-uim.h \
	qmi-enums-oma.h \
	qmi-enums-wda.h \
	qmi-enums-voice.h \
	qmi-enums-loc.h qmi-flags64-loc.h \
	qmi-enums-qos.h \
	qmi-enums.h qmi-enums-private.h \
	qmi-utils.h qmi-utils.c \
	qmi-compat.h qmi-compat.c \
	qmi-message.h qmi-message.c \
	qmi-message-context.h qmi-message-context.c \
	qmi-device.h qmi-device.c \
	qmi-client.h qmi-client.c \
	qmi-proxy.h qmi-proxy.c

libqmi_glib_la_LIBADD = \
	${top_builddir}/src/libqmi-glib/generated/libqmi-glib-generated.la \
	$(GLIB_LIBS) \
	$(MBIM_LIBS)

libqmi_glib_la_LDFLAGS = \
	-version-info $(QMI_GLIB_LT_CURRENT):$(QMI_GLIB_LT_REVISION):$(QMI_GLIB_LT_AGE)

includedir = @includedir@/libqmi-glib
include_HEADERS = \
	libqmi-glib.h \
	qmi-version.h \
	qmi-errors.h \
	qmi-enums.h \
	qmi-enums-private.h \
	qmi-compat.h \
	qmi-enums-dms.h qmi-flags64-dms.h \
	qmi-enums-wds.h \
	qmi-enums-nas.h qmi-flags64-nas.h \
	qmi-enums-wms.h \
	qmi-enums-pds.h \
	qmi-enums-pdc.h \
	qmi-enums-pbm.h \
	qmi-enums-uim.h \
	qmi-enums-oma.h \
	qmi-enums-wda.h \
	qmi-enums-voice.h \
	qmi-enums-loc.h qmi-flags64-loc.h \
	qmi-enums-qos.h \
	qmi-utils.h \
	qmi-message.h \
	qmi-message-context.h \
	qmi-device.h \
	qmi-client.h \
	qmi-proxy.h

EXTRA_DIST = \
	qmi-version.h.in
