#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(cpuaff,1.0.4,dcdillon@gmail.com,cpuaff)
AM_INIT_AUTOMAKE
AC_OUTPUT(Makefile include/Makefile examples/Makefile tests/Makefile)
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CANONICAL_HOST
case $host in
	*linux*)
		;;
	*)
		AC_CHECK_LIB(hwloc, hwloc_topology_init)
		;;
esac

AC_OUTPUT
