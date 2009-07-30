# Defines global public variables
#
# BUILD_NAME = build name in lower case (ex: win32-x86-msvc90-debug)
# SVN_REVISION = subversion revision number, see ow_get_svn_revision()
# BUILD_DATE = current date time: 20070323121316 (e.g 2007-03-23 12:13:16), see ow_get_current_date_time()
#
# Useful CMake variables:
# http://www.cmake.org/Wiki/CMake_Useful_Variables
# All variables defined by CMake:
# http://www.cmake.org/Wiki/CMake_Useful_Variables/Get_Variables_From_CMake_Dashboards
#
# Copyright (C) 2006-2007  Wengo
# Copyright (C) 2007-2009  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


# Sets a global variable to test if OWBuild is already included or not in the build system
set(OWBUILD_INCLUDED TRUE)

# Build name in lower case, ex: win32-x86-msvc90-debug
# Should be used for the directory name where binary files will be copied
# Use the system name (Linux, Windows, MacOS...), processor architecture (i686, x86, x86_64)
# and compiler name (MSVC, GCC...)
set(BUILD_NAME ${SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${COMPILER_NAME}-${CMAKE_BUILD_TYPE})
string(TOLOWER ${BUILD_NAME} BUILD_NAME)

# Gets svn revision
set(SVN_REVISION "")
ow_get_svn_revision(SVN_REVISION)
# Bugfix with SVN revision number that can integrate a : and
# this does not work under Windows for the installer name, replace it by -
if (SVN_REVISION)
	string(REPLACE ":" "-" SVN_REVISION ${SVN_REVISION})
endif (SVN_REVISION)

# Gets current date time
set(BUILD_DATE "0")
ow_get_current_date_time(BUILD_DATE)
