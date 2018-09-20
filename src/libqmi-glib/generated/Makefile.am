
noinst_LTLIBRARIES = libqmi-glib-generated.la

GENERATED_H = \
	qmi-error-types.h \
	qmi-enum-types.h \
	qmi-enum-types-private.h \
	qmi-flags64-types.h \
	qmi-ctl.h \
	qmi-dms.h \
	qmi-nas.h \
	qmi-wds.h \
	qmi-wms.h \
	qmi-pds.h \
	qmi-pdc.h \
	qmi-pbm.h \
	qmi-uim.h \
	qmi-oma.h \
	qmi-wda.h \
	qmi-voice.h \
	qmi-loc.h \
	qmi-qos.h \
	$(NULL)

GENERATED_C = \
	qmi-error-types.c \
	qmi-error-quarks.c \
	qmi-enum-types.c \
	qmi-enum-types-private.c \
	qmi-flags64-types.c \
	qmi-ctl.c \
	qmi-dms.c \
	qmi-nas.c \
	qmi-wds.c \
	qmi-wms.c \
	qmi-pds.c \
	qmi-pdc.c \
	qmi-pbm.c \
	qmi-uim.c \
	qmi-oma.c \
	qmi-wda.c \
	qmi-voice.c \
	qmi-loc.c \
	qmi-qos.c \
	$(NULL)

GENERATED_SECTIONS = \
	qmi-ctl.sections \
	qmi-dms.sections \
	qmi-nas.sections \
	qmi-wds.sections \
	qmi-wms.sections \
	qmi-pds.sections \
	qmi-pdc.sections \
	qmi-pbm.sections \
	qmi-uim.sections \
	qmi-oma.sections \
	qmi-wda.sections \
	qmi-voice.sections \
	qmi-loc.sections \
	qmi-qos.sections \
	$(NULL)

# Error types
qmi-error-types.h: $(top_srcdir)/src/libqmi-glib/qmi-errors.h $(top_srcdir)/build-aux/templates/qmi-error-types-template.h
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#ifndef __LIBQMI_GLIB_ERROR_TYPES_H__\n#define __LIBQMI_GLIB_ERROR_TYPES_H__\n#include \"qmi-errors.h\"\n" \
		--template $(top_srcdir)/build-aux/templates/qmi-error-types-template.h \
		--ftail "#endif /* __LIBQMI_GLIB_ERROR_TYPES_H__ */\n" \
		$(top_srcdir)/src/libqmi-glib/qmi-errors.h > $@

qmi-error-types.c: $(top_srcdir)/src/libqmi-glib/qmi-errors.h qmi-error-types.h $(top_srcdir)/build-aux/templates/qmi-error-types-template.c
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#include \"qmi-errors.h\"\n#include \"qmi-error-types.h\"\n" \
		--template $(top_srcdir)/build-aux/templates/qmi-error-types-template.c \
		$(top_srcdir)/src/libqmi-glib/qmi-errors.h > $@

qmi-error-quarks.c: $(top_srcdir)/src/libqmi-glib/qmi-errors.h qmi-error-types.h $(top_srcdir)/build-aux/templates/qmi-error-quarks-template.c
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#include \"qmi-errors.h\"\n#include \"qmi-error-types.h\"\n" \
		--template $(top_srcdir)/build-aux/templates/qmi-error-quarks-template.c \
		$(top_srcdir)/src/libqmi-glib/qmi-errors.h > $@

# Enum/Flag types
ENUMS = \
	$(top_srcdir)/src/libqmi-glib/qmi-enums.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-wds.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-dms.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-nas.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-wms.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-pdc.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-pds.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-pbm.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-uim.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-oma.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-wda.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-voice.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-loc.h \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-qos.h \
	$(top_srcdir)/src/libqmi-glib/qmi-device.h
qmi-enum-types.h:  $(ENUMS) $(top_srcdir)/build-aux/templates/qmi-enum-types-template.h
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#ifndef __LIBQMI_GLIB_ENUM_TYPES_H__\n#define __LIBQMI_GLIB_ENUM_TYPES_H__\n#include \"qmi-enums.h\"\n#include \"qmi-enums-wds.h\"\n#include \"qmi-enums-dms.h\"\n#include \"qmi-enums-nas.h\"\n#include \"qmi-enums-wms.h\"\n#include \"qmi-enums-pds.h\"\n#include \"qmi-enums-pdc.h\"\n#include \"qmi-enums-pbm.h\"\n#include \"qmi-enums-uim.h\"\n#include \"qmi-enums-oma.h\"\n#include \"qmi-enums-wda.h\"\n#include \"qmi-enums-voice.h\"\n#include \"qmi-enums-loc.h\"\n#include \"qmi-enums-qos.h\"\n#include \"qmi-device.h\"\n" \
		--template $(top_srcdir)/build-aux/templates/qmi-enum-types-template.h \
		--ftail "#endif /* __LIBQMI_GLIB_ENUM_TYPES_H__ */\n" \
		$(ENUMS) > $@

qmi-enum-types.c: $(ENUMS) qmi-enum-types.h $(top_srcdir)/build-aux/templates/qmi-enum-types-template.c
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#include \"qmi-enum-types.h\"\n" \
		--template $(top_srcdir)/build-aux/templates/qmi-enum-types-template.c \
		$(ENUMS) > $@

# Private Enum/Flag types
PRIVATE_ENUMS = \
	$(top_srcdir)/src/libqmi-glib/qmi-enums-private.h
qmi-enum-types-private.h:  $(PRIVATE_ENUMS) $(top_srcdir)/build-aux/templates/qmi-enum-types-template.h
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#ifndef __LIBQMI_GLIB_ENUM_TYPES_PRIVATE_H__\n#define __LIBQMI_GLIB_ENUM_TYPES_PRIVATE_H__\n#include \"qmi-enums-private.h\"\n" \
		--template $(top_srcdir)/build-aux/templates/qmi-enum-types-template.h \
		--ftail "#endif /* __LIBQMI_GLIB_ENUM_TYPES_PRIVATE_H__ */\n" \
		$(PRIVATE_ENUMS) > $@

qmi-enum-types-private.c: $(PRIVATE_ENUMS) qmi-enum-types-private.h $(top_srcdir)/build-aux/templates/qmi-enum-types-template.c
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#include \"qmi-enum-types-private.h\"\n" \
		--template $(top_srcdir)/build-aux/templates/qmi-enum-types-template.c \
		$(PRIVATE_ENUMS) > $@

# 64bit flag types
FLAGS64 = $(top_srcdir)/src/libqmi-glib/qmi-flags64-dms.h $(top_srcdir)/src/libqmi-glib/qmi-flags64-nas.h $(top_srcdir)/src/libqmi-glib/qmi-flags64-loc.h
qmi-flags64-types.h:  $(FLAGS64) $(top_srcdir)/build-aux/templates/qmi-flags64-types-template.h
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#ifndef __LIBQMI_GLIB_FLAGS64_TYPES_H__\n#define __LIBQMI_GLIB_FLAGS64_TYPES_H__\n#include \"qmi-flags64-dms.h\"\n#include \"qmi-flags64-nas.h\"\n#include \"qmi-flags64-loc.h\"\n" \
		--template $(top_srcdir)/build-aux/templates/qmi-flags64-types-template.h \
		--ftail "#endif /* __LIBQMI_GLIB_FLAGS64_TYPES_H__ */\n" \
		$(FLAGS64) > $@

qmi-flags64-types.c: $(FLAGS64) qmi-flags64-types.h $(top_srcdir)/build-aux/templates/qmi-flags64-types-template.c
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#include \"qmi-flags64-types.h\"\n" \
		--template $(top_srcdir)/build-aux/templates/qmi-flags64-types-template.c \
		$(FLAGS64) > $@

# CTL service
qmi-ctl.h qmi-ctl.c qmi-ctl.sections: $(top_srcdir)/data/qmi-service-ctl.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN)  \
		rm -f qmi-ctl.h && \
		rm -f qmi-ctl.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-ctl.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-ctl

# DMS service
qmi-dms.h qmi-dms.c qmi-dms.sections: $(top_srcdir)/data/qmi-service-dms.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-dms.h && \
		rm -f qmi-dms.c && \
		 $(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-dms.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-dms

# WDS service
qmi-wds.h qmi-wds.c qmi-wds.sections: $(top_srcdir)/data/qmi-service-wds.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-wds.h && \
		rm -f qmi-wds.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-wds.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-wds

# NAS service
qmi-nas.h qmi-nas.c qmi-nas.sections: $(top_srcdir)/data/qmi-service-nas.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-nas.h && \
		rm -f qmi-nas.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-nas.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-nas

# WMS service
qmi-wms.h qmi-wms.c qmi-wms.sections: $(top_srcdir)/data/qmi-service-wms.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-wms.h && \
		rm -f qmi-wms.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-wms.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-wms

# PDS service
qmi-pds.h qmi-pds.c qmi-pds.sections: $(top_srcdir)/data/qmi-service-pds.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-pds.h && \
		rm -f qmi-pds.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-pds.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-pds

# PDC service
qmi-pdc.h qmi-pdc.c qmi-pdc.sections: $(top_srcdir)/data/qmi-service-pdc.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-pdc.h && \
		rm -f qmi-pdc.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-pdc.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-pdc

# PBM service
qmi-pbm.h qmi-pbm.c qmi-pbm.sections: $(top_srcdir)/data/qmi-service-pbm.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-pbm.h && \
		rm -f qmi-pbm.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-pbm.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-pbm

# UIM service
qmi-uim.h qmi-uim.c qmi-uim.sections: $(top_srcdir)/data/qmi-service-uim.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN)  \
		rm -f qmi-uim.h && \
		rm -f qmi-uim.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-uim.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-uim

# OMA service
qmi-oma.h qmi-oma.c qmi-oma.sections: $(top_srcdir)/data/qmi-service-oma.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-oma.h && \
		rm -f qmi-oma.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-oma.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-oma

# WDA service
qmi-wda.h qmi-wda.c qmi-wda.sections: $(top_srcdir)/data/qmi-service-wda.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-wda.h && \
		rm -f qmi-wda.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-wda.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-wda

# VOICE service
qmi-voice.h qmi-voice.c qmi-voice.sections: $(top_srcdir)/data/qmi-service-voice.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-voice.h && \
		rm -f qmi-voice.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-voice.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-voice

# LOC service
qmi-loc.h qmi-loc.c qmi-loc.sections: $(top_srcdir)/data/qmi-service-loc.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-loc.h && \
		rm -f qmi-loc.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-loc.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-loc

# QoS service
qmi-qos.h qmi-qos.c qmi-qos.sections: $(top_srcdir)/data/qmi-service-qos.json $(top_srcdir)/build-aux/qmi-codegen/*.py $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen
	$(AM_V_GEN) \
		rm -f qmi-qos.h && \
		rm -f qmi-qos.c && \
		$(PYTHON) $(top_srcdir)/build-aux/qmi-codegen/qmi-codegen \
			--input $(top_srcdir)/data/qmi-service-qos.json \
			--include $(top_srcdir)/data/qmi-common.json \
			--output qmi-qos

BUILT_SOURCES = $(GENERATED_H) $(GENERATED_C)

nodist_libqmi_glib_generated_la_SOURCES = \
	$(GENERATED_H) \
	$(GENERATED_C)

libqmi_glib_generated_la_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src/libqmi-glib \
	-I$(top_builddir)/src/libqmi-glib \
	-DLIBQMI_GLIB_COMPILATION \
	-DG_LOG_DOMAIN=\"Qmi\" \
	-Wno-unused-function

libqmi_glib_generated_la_LIBADD = \
	$(GLIB_LIBS)

includedir = @includedir@/libqmi-glib
nodist_include_HEADERS = \
	qmi-error-types.h \
	qmi-enum-types.h \
	qmi-flags64-types.h \
	qmi-dms.h \
	qmi-nas.h \
	qmi-wds.h \
	qmi-wms.h \
	qmi-pds.h \
	qmi-pdc.h \
	qmi-pbm.h \
	qmi-uim.h \
	qmi-oma.h \
	qmi-wda.h \
	qmi-voice.h \
	qmi-loc.h \
	qmi-qos.h \
	$(NULL)

CLEANFILES = $(GENERATED_H) $(GENERATED_C) $(GENERATED_SECTIONS)
