# - Try to find QtSingleApplication
# Once done this will define
#
#  QTSINGLEAPPLICATION_FOUND - System has QtSingleApplication
#  QTSINGLEAPPLICATION_INCLUDE_DIRS - QtSingleApplication include directories
#  QTSINGLEAPPLICATION_LIBRARIES - Link these to use QtSingleApplication
#  QTSINGLEAPPLICATION_DEFINITIONS - Compiler switches required for using QtSingleApplication

# Copyright (C) 2010  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


set(QTSINGLEAPPLICATION_DEFINITIONS -DQT_QTSINGLEAPPLICATION_IMPORT)

find_path(QTSINGLEAPPLICATION_INCLUDE_DIRS
  NAMES
    QtSingleApplication
  PATHS
    /usr/include/QtSolutions
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
)

find_library(QTSINGLEAPPLICATION_LIBRARIES
  NAMES
    QtSolutions_SingleApplication-2.6
  PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QtSingleApplication DEFAULT_MSG QTSINGLEAPPLICATION_INCLUDE_DIRS QTSINGLEAPPLICATION_LIBRARIES)
