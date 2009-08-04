# Defines general default CMake configuration options
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


# This option is OFF, not needed
# Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
# in about every subdir
# Since CMake 2.4.0
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
