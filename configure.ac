#
# configure.ac
#
# i11y-clutter - Set of Cogl/Clutter modules for Interactivity related apps
# Copyright (C) 2012 Igalia S.L.
#
# Authors:
#   Eduardo Lima <elima@igalia.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# version 3, or (at your option) any later version as published by
# the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License at http://www.gnu.org/licenses/lgpl-3.0.txt
# for more details.
#

AC_PREREQ(2.59)

m4_define([prj_name],       [i11y-clutter])
m4_define([prj_short_name], [i11y-clutter])
m4_define([prj_home],       [https://github.com/elima/i11y-clutter])

# package version number (not shared library version)
# odd micro numbers indicate in-progress development
# even micro numbers indicate released versions
m4_define([prj_version_major], [0])
m4_define([prj_version_minor], [1])
m4_define([prj_version_micro], [1])

m4_define([prj_version],
          [prj_version_major.prj_version_minor.prj_version_micro])
m4_define([prj_api_version],
          [prj_version_major.prj_version_minor])

AC_INIT([prj_name],
	[prj_version],
	[prj_home])

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE

# Project specific definitions
m4_define([prj_gir_namespace], [I11yClutter])

PRJ_NAME="prj_short_name"
AC_SUBST(PRJ_NAME)

PRJ_SYMBOL_PREFIX="i11y_clutter"
AC_SUBST(PRJ_SYMBOL_PREFIX)

PRJ_VERSION="prj_version"
AC_SUBST(PRJ_VERSION)

PRJ_API_VERSION="prj_api_version"
AC_SUBST(PRJ_API_VERSION)

PRJ_API_NAME="prj_short_name-prj_api_version"
AC_SUBST(PRJ_API_NAME)

# Check for programs
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

# enable pkg-config
PKG_PROG_PKG_CONFIG

# Required libraries
GLIB_REQUIRED=2.28.0
CLUTTER_REQUIRED=1.10

PKG_CHECK_MODULES(GLIB, gio-2.0 >= $GLIB_REQUIRED
		        glib-2.0 >= $GLIB_REQUIRED
		        gthread-2.0 >= $GLIB_REQUIRED
		        gobject-2.0 >= $GLIB_REQUIRED)

PKG_CHECK_MODULES(CLUTTER, clutter-1.0 >= $CLUTTER_REQUIRED)

# GObject-Introspection check
GOBJECT_INTROSPECTION_CHECK([0.6.7])
if test "x$found_introspection" = "xyes"; then
   PRJ_GIR_NAMESPACE="prj_gir_namespace"
   AC_SUBST(PRJ_GIR_NAMESPACE)

   PRJ_GIR_API_NAME="prj_gir_namespace"-"prj_api_version"
   AC_SUBST(PRJ_GIR_API_NAME)

   PRJ_GIR_TARGET_NAME="prj_gir_namespace"_"prj_version_major"_"prj_version_minor"_gir
   AC_SUBST(PRJ_GIR_TARGET_NAME)
fi

# GTK Doc
GTK_DOC_CHECK([1.11])
AM_CONDITIONAL([BUILD_GTK_DOC], [test "x$enable_gtk_doc" = "xyes" || test ! -f "autogen.sh"])

# Silent build
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

# Tests
AC_ARG_ENABLE(tests,
        AS_HELP_STRING([--enable-tests[=@<:@no/yes@:>@]],
                [Enable automated unit and functional tests [default=yes]]),,
                [enable_tests=yes])

AM_CONDITIONAL(ENABLE_TESTS, test x"${enable_tests}" = x"yes")

# Debug
AC_ARG_ENABLE(debug,
        AS_HELP_STRING([--enable-debug[=@<:@no/yes@:>@]],
                [Enable debug mode by adding -ggdb, -g3, -O0 and -Werror to CFLAGS [default=no]]),,
                [enable_debug=no])

AM_CONDITIONAL(ENABLE_DEBUG, test x"${enable_debug}" = x"yes")

# Output files
AC_OUTPUT([
        Makefile
        i11y-clutter/Makefile
        i11y-clutter/i11y-clutter-0.1.pc
])

echo ""
echo "   prj_name $VERSION"
echo "   ====================="
echo ""
echo "              Install prefix:   ${prefix}"
echo "    Build introspection data:   ${enable_introspection}"
echo "     Build API documentation:   ${enable_gtk_doc}"
echo "           Enable debug mode:   ${enable_debug}"
echo "      Enable automated tests:   ${enable_tests}"
echo ""
