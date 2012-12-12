MAINTAINERCLEANFILES = \
	Makefile.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PRJ_API_NAME@.pc

library_includedir=$(includedir)/@PRJ_API_NAME@
library_include_HEADERS = @PRJ_NAME@.h

noinst_HEADERS = \
	@PRJ_NAME@-marshal.h

CLEANFILES = *~ @PRJ_API_NAME@.pc

AM_CFLAGS = -Wall \
	$(GLIB_CFLAGS) \
	$(CLUTTER_CFLAGS) \
	-DPKGDATADIR="\"$(pkgdatadir)\"" \
	-DENABLE_TESTS="\"$(enable_tests)\""

if ENABLE_DEBUG
AM_CFLAGS += -g3 -O0 -ggdb
else
AM_CFLAGS += -DG_DISABLE_ASSERT -DG_DISABLE_CHECKS
endif

# marshal sources
@PRJ_NAME@-marshal.h: @PRJ_NAME@-marshal.list
	glib-genmarshal --header \
		--prefix=@PRJ_SYMBOL_PREFIX@_marshal @PRJ_NAME@-marshal.list > @PRJ_NAME@-marshal.h

@PRJ_NAME@-marshal.c: @PRJ_NAME@-marshal.list
	glib-genmarshal --body \
		--prefix=@PRJ_SYMBOL_PREFIX@_marshal @PRJ_NAME@-marshal.list > @PRJ_NAME@-marshal.c

BUILT_SOURCES = \
	@PRJ_NAME@-marshal.c \
	@PRJ_NAME@-marshal.h

# libraries
lib_LTLIBRARIES = lib@PRJ_API_NAME@.la

source_c = \
	@PRJ_NAME@-marshal.c \
	@PRJ_NAME@-offscreen.c \
	@PRJ_NAME@-partsys.c

source_h = \
	@PRJ_NAME@.h \
	@PRJ_NAME@-offscreen.h \
	@PRJ_NAME@-partsys.h

source_h_priv =

lib@PRJ_API_NAME@_la_LIBADD = \
	$(GLIB_LIBS) \
	$(CLUTTER_LIBS) \
	-lm

lib@PRJ_API_NAME@_la_CFLAGS  = \
	$(AM_CFLAGS)

lib@PRJ_API_NAME@_la_LDFLAGS = \
	-version-info 0:1:0 \
	-no-undefined

lib@PRJ_API_NAME@_la_SOURCES = \
	$(source_c) \
	$(source_h) \
	$(source_h_priv)

nodist_lib@PRJ_API_NAME@_la_SOURCES = \
	$(BUILT_SOURCES)

@PRJ_SYMBOL_PREFIX@dir = $(includedir)/@PRJ_API_NAME@
@PRJ_SYMBOL_PREFIX@_HEADERS = \
	$(source_h) \
	$(source_h_priv)

# introspection support
if HAVE_INTROSPECTION

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = \
	--warn-all \
	--add-include-path=$(srcdir) \
	--symbol-prefix=@PRJ_SYMBOL_PREFIX@

INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

introspection_sources = \
	$(@PRJ_SYMBOL_PREFIX@_HEADERS) \
	$(source_c) \
	$(BUILT_SOURCES)

@PRJ_GIR_API_NAME@.gir: \
	lib@PRJ_API_NAME@.la \
	Makefile
@PRJ_GIR_TARGET_NAME@_INCLUDES = \
	GObject-2.0 \
	Gio-2.0 \
	Clutter-1.0
@PRJ_GIR_TARGET_NAME@_CFLAGS = $(INCLUDES)
@PRJ_GIR_TARGET_NAME@_LIBS = @PRJ_API_NAME@ m
@PRJ_GIR_TARGET_NAME@_NAMESPACE = @PRJ_GIR_NAMESPACE@
@PRJ_GIR_TARGET_NAME@_FILES = \
	$(addprefix $(srcdir)/,$(introspection_sources))

INTROSPECTION_GIRS += @PRJ_GIR_API_NAME@.gir

girdir = @INTROSPECTION_GIRDIR@
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = @INTROSPECTION_TYPELIBDIR@
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(dist_gir_DATA) $(typelib_DATA)
endif

maintainer-clean-local:
	rm -rf tmp-introspect*

EXTRA_DIST = \
	@PRJ_NAME@-marshal.list
