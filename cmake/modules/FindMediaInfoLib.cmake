# - Try to find MediaInfoLib
# Once done this will define
#
#  MEDIAINFOLIB_FOUND - System has MediaInfoLib
#  MEDIAINFOLIB_INCLUDE_DIRS - MediaInfoLib include directories
#  MEDIAINFOLIB_LIBRARIES - Link these to use MediaInfoLib
#  MEDIAINFOLIB_DEFINITIONS - Compiler switches required for using MediaInfoLib

# Copyright (C) 2010  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (MEDIAINFOLIB_LIBRARIES AND MEDIAINFOLIB_INCLUDE_DIRS)
  # In cache already
  set(MEDIAINFOLIB_FOUND TRUE)
else (MEDIAINFOLIB_LIBRARIES AND MEDIAINFOLIB_INCLUDE_DIRS)

  set(MEDIAINFOLIB_DEFINITIONS -DMEDIAINFOLIB)

  find_path(MEDIAINFOLIB_INCLUDE_DIRS
    NAMES
      MediaInfo/MediaInfo.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  find_library(MEDIAINFOLIB_LIBRARIES
    NAMES
      mediainfo
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (MEDIAINFOLIB_INCLUDE_DIRS AND MEDIAINFOLIB_LIBRARIES)
    set(MEDIAINFOLIB_FOUND TRUE)
  endif (MEDIAINFOLIB_INCLUDE_DIRS AND MEDIAINFOLIB_LIBRARIES)

  if (MEDIAINFOLIB_FOUND)
    if (NOT MediaInfoLib_FIND_QUIETLY)
      message(STATUS "Found MediaInfoLib: ${MEDIAINFOLIB_LIBRARIES}")
    endif (NOT MediaInfoLib_FIND_QUIETLY)
  else (MEDIAINFOLIB_FOUND)
    if (MediaInfoLib_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find MediaInfoLib")
    endif (MediaInfoLib_FIND_REQUIRED)
  endif (MEDIAINFOLIB_FOUND)

  # Show the MEDIAINFOLIB_INCLUDE_DIRS and MEDIAINFOLIB_LIBRARIES variables only in the advanced view
  mark_as_advanced(MEDIAINFOLIB_INCLUDE_DIRS MEDIAINFOLIB_LIBRARIES)

endif (MEDIAINFOLIB_LIBRARIES AND MEDIAINFOLIB_INCLUDE_DIRS)
