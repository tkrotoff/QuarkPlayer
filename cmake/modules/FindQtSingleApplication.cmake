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


if (QTSINGLEAPPLICATION_LIBRARIES AND QTSINGLEAPPLICATION_INCLUDE_DIRS)
  # In cache already
  set(QTSINGLEAPPLICATION_FOUND TRUE)
else (QTSINGLEAPPLICATION_LIBRARIES AND QTSINGLEAPPLICATION_INCLUDE_DIRS)

  set(QTSINGLEAPPLICATION_DEFINITIONS -DQTSINGLEAPPLICATION)

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

  set(QTSINGLEAPPLICATION_DEFINITIONS -DQT_QTSINGLEAPPLICATION_IMPORT)

  if (QTSINGLEAPPLICATION_INCLUDE_DIRS AND QTSINGLEAPPLICATION_LIBRARIES)
    set(QTSINGLEAPPLICATION_FOUND TRUE)
  endif (QTSINGLEAPPLICATION_INCLUDE_DIRS AND QTSINGLEAPPLICATION_LIBRARIES)

  if (QTSINGLEAPPLICATION_FOUND)
    if (NOT QtSingleApplication_FIND_QUIETLY)
      message(STATUS "Found QtSingleApplication: ${QTSINGLEAPPLICATION_LIBRARIES}")
    endif (NOT QtSingleApplication_FIND_QUIETLY)
  else (QTSINGLEAPPLICATION_FOUND)
    if (QtSingleApplication_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find QtSingleApplication")
    endif (QtSingleApplication_FIND_REQUIRED)
  endif (QTSINGLEAPPLICATION_FOUND)

  # Show the QTSINGLEAPPLICATION_INCLUDE_DIRS and QTSINGLEAPPLICATION_LIBRARIES variables only in the advanced view
  mark_as_advanced(QTSINGLEAPPLICATION_INCLUDE_DIRS QTSINGLEAPPLICATION_LIBRARIES)

endif (QTSINGLEAPPLICATION_LIBRARIES AND QTSINGLEAPPLICATION_INCLUDE_DIRS)
