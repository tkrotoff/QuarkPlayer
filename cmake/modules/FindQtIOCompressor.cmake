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


if (QTIOCOMPRESSOR_LIBRARIES AND QTIOCOMPRESSOR_INCLUDE_DIRS)
  # In cache already
  set(QTIOCOMPRESSOR_FOUND TRUE)
else (QTIOCOMPRESSOR_LIBRARIES AND QTIOCOMPRESSOR_INCLUDE_DIRS)

  set(QTIOCOMPRESSOR_DEFINITIONS -DQTIOCOMPRESSOR)

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

  set(QTIOCOMPRESSOR_DEFINITIONS -DQT_QTIOCOMPRESSOR_IMPORT)

  if (QTIOCOMPRESSOR_INCLUDE_DIRS AND QTIOCOMPRESSOR_LIBRARIES)
    set(QTIOCOMPRESSOR_FOUND TRUE)
  endif (QTIOCOMPRESSOR_INCLUDE_DIRS AND QTIOCOMPRESSOR_LIBRARIES)

  if (QTIOCOMPRESSOR_FOUND)
    if (NOT QtIOCompressor_FIND_QUIETLY)
      message(STATUS "Found QtIOCompressor: ${QTIOCOMPRESSOR_LIBRARIES}")
    endif (NOT QtIOCompressor_FIND_QUIETLY)
  else (QTIOCOMPRESSOR_FOUND)
    if (QtIOCompressor_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find QtIOCompressor")
    endif (QtIOCompressor_FIND_REQUIRED)
  endif (QTIOCOMPRESSOR_FOUND)

  # Show the QTIOCOMPRESSOR_INCLUDE_DIRS and QTIOCOMPRESSOR_LIBRARIES variables only in the advanced view
  mark_as_advanced(QTIOCOMPRESSOR_INCLUDE_DIRS QTIOCOMPRESSOR_LIBRARIES)

endif (QTIOCOMPRESSOR_LIBRARIES AND QTIOCOMPRESSOR_INCLUDE_DIRS)
