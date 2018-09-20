
# The name of the module.
DOC_MODULE = libqmi-glib

# The top-level SGML file.
DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.xml

# Sections file building
ALL_SECTIONS = \
	$(srcdir)/libqmi-glib-common.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-dms.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-nas.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-wds.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-wms.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-pdc.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-pds.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-pbm.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-uim.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-oma.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-wda.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-voice.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-loc.sections \
	$(top_builddir)/src/libqmi-glib/generated/qmi-qos.sections \
	$(NULL)

$(DOC_MODULE)-sections.mstamp: $(ALL_SECTIONS)
	$(AM_V_GEN) \
		rm -f $(DOC_MODULE)-sections.txt && \
		cat $(ALL_SECTIONS) > $(DOC_MODULE)-sections.txt && \
		touch $(DOC_MODULE)-sections.mstamp

BUILT_SOURCES = $(DOC_MODULE)-sections.mstamp

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS = --rebuild-types --deprecated-guards="QMI_DISABLE_DEPRECATED"

# The directory containing the source code.
DOC_SOURCE_DIR = \
	$(top_srcdir)/src/libqmi-glib \
	$(top_builddir)/src/libqmi-glib/generated

# Used for dependencies
HFILE_GLOB = \
	$(top_srcdir)/src/libqmi-glib/*.h \
	$(top_builddir)/src/libqmi-glib/*.h \
	$(top_builddir)/src/libqmi-glib/generated/*.h
CFILE_GLOB = \
	$(top_srcdir)/src/libqmi-glib/*.c \
	$(top_builddir)/src/libqmi-glib/generated/*.c

# Headers to ignore
IGNORE_HFILES = \
	qmi-enums-private.h \
	qmi-enum-types-private.h \
	qmi-ctl.h \
	test-port-context.h \
	test-fixture.h

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
AM_CPPFLAGS = \
	-I$(srcdir) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(GLIB_CFLAGS)

GTKDOC_LIBS = \
	$(GLIB_LIBS) \
	$(top_builddir)/src/libqmi-glib/libqmi-glib.la

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS = --output-format=xml --sgml-mode --name-space=qmi

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
content_files = version.xml

expand_content_files =

extra_files =

include $(top_srcdir)/gtk-doc.make

EXTRA_DIST += \
	libqmi-glib-common.sections \
	$(DOC_MODULE)-sections.txt \
	$(DOC_MODULE)-sections.mstamp \
	version.xml.in \
	$(DIAGRAMS) \
	$(NULL)

CLEANFILES += \
	$(DOC_MODULE)-decl-list.txt \
	$(DOC_MODULE)-decl.txt \
	$(DOC_MODULE)-overrides.txt \
	$(DOC_MODULE)-undeclared.txt \
	$(DOC_MODULE)-undocumented.txt \
	$(DOC_MODULE)-overrides.txt \
	$(DOC_MODULE)-unused.txt \
	$(DOC_MODULE).args \
	$(DOC_MODULE).hierarchy \
	$(DOC_MODULE).interfaces \
	$(DOC_MODULE).prerequisites \
	$(DOC_MODULE).signals \
	$(DOC_MODULE).types \
	$(DOC_MODULE)-sections.txt \
	$(DOC_MODULE)-sections.mstamp \
	*.stamp \
	-rf xml html tmpl \
	$(NULL)
