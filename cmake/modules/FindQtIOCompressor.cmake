# - Try to find QtIOCompressor
# Once done this will define
#
#  QTIOCOMPRESSOR_FOUND - System has QtIOCompressor
#  QTIOCOMPRESSOR_INCLUDE_DIRS - QtIOCompressor include directories
#  QTIOCOMPRESSOR_LIBRARIES - Link these to use QtIOCompressor
#  QTIOCOMPRESSOR_DEFINITIONS - Compiler switches required for using QtIOCompressor

# Copyright (C) 2010  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


set(QTIOCOMPRESSOR_DEFINITIONS -DQT_QTIOCOMPRESSOR_IMPORT)

find_path(QTIOCOMPRESSOR_INCLUDE_DIRS
  NAMES
    QtIOCompressor
  PATHS
    /usr/include/QtSolutions
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
)

find_library(QTIOCOMPRESSOR_LIBRARIES
  NAMES
    QtSolutions_IOCompressor-2.3
  PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QtIOCompressor DEFAULT_MSG QTIOCOMPRESSOR_INCLUDE_DIRS QTIOCOMPRESSOR_LIBRARIES)
