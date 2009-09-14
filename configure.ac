#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(memmgr, 1.00, molnar@ti.com)
AC_CONFIG_SRCDIR([src/memmgr.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([memmgr], [1.00])
LT_INIT

# Checks for programs.
AC_PROG_CC
AM_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([munmap strerror])

AC_ARG_ENABLE(tilermgr,
[  --enable-tilermgr    Include TilerMgr headers],
[case "${enableval}" in
  yes) tilermgr=true ;;
  no)  tilermgr=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-tilermgr) ;;
esac],[tilermgr=false])

AC_ARG_ENABLE(unit_tests,
[  --enable-unit-tests    Build unit tests],
[case "${enableval}" in
  yes) unit_tests=true ;;
  no)  unit_tests=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-unittests) ;;
esac],[tilermgr=false])

AM_CONDITIONAL(UNIT_TESTS, test x$unit_tests = xtrue)

AM_CONDITIONAL(TILERMGR, test x$tilermgr = xtrue)

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT

