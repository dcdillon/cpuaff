#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(libcpuaff, 0.1 , dcdillon@gmail.com)
AM_INIT_AUTOMAKE(libcpuaff, 0.1)
AC_OUTPUT(Makefile include/Makefile)
AC_CONFIG_SRCDIR([examples/test.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_HAVE_LIBRARY(hwloc)

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.

AC_OUTPUT
