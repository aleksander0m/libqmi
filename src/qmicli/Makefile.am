
SUBDIRS = . test

noinst_LTLIBRARIES = libhelpers.la

libhelpers_la_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(MBIM_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src/libqmi-glib \
	-I$(top_srcdir)/src/libqmi-glib/generated \
	-I$(top_builddir)/src/libqmi-glib \
	-I$(top_builddir)/src/libqmi-glib/generated \
	$(NULL)

libhelpers_la_SOURCES = \
	qmicli-helpers.c \
	qmicli-helpers.h \
	$(NULL)

libhelpers_la_LIBADD = \
        $(GLIB_LIBS)

bin_PROGRAMS = qmicli

qmicli_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(MBIM_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src/libqmi-glib \
	-I$(top_srcdir)/src/libqmi-glib/generated \
	-I$(top_builddir)/src/libqmi-glib \
	-I$(top_builddir)/src/libqmi-glib/generated

qmicli_SOURCES = \
	qmicli.c \
	qmicli.h \
	qmicli-dms.c \
	qmicli-wds.c \
	qmicli-nas.c \
	qmicli-pbm.c \
	qmicli-pdc.c \
	qmicli-uim.c \
	qmicli-wms.c \
	qmicli-wda.c \
	qmicli-voice.c \
	qmicli-loc.c \
	qmicli-qos.c \
	qmicli-charsets.c \
	qmicli-charsets.h

qmicli_LDADD = \
	$(MBIM_LIBS) \
	$(GLIB_LIBS) \
	libhelpers.la \
	$(top_builddir)/src/libqmi-glib/libqmi-glib.la

completiondir = $(datadir)/bash-completion/completions

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(completiondir)
	$(INSTALL_DATA) $(srcdir)/qmicli-completion $(DESTDIR)$(completiondir)/qmicli

uninstall-hook:
	rm -f $(DESTDIR)$(completiondir)/qmicli

EXTRA_DIST = qmicli-completion
