# - add_qt_translations(dir1 ... dirN)
# Adds directories which contain source files to be translated.
# Creates a global variable named DIRS_TRANSLATIONS
# - clear_qt_translations()
# Clears global variable named DIRS_TRANSLATIONS
#
# Example:
# clear_qt_translations()
# set(myapp_SRCS
#     toto.cpp
#     tata.cpp
# )
# add_qt_translations(${CMAKE_CURRENT_SOURCE_DIR})
# qt4_create_translation(myapp_QMS ${DIRS_TRANSLATIONS})
# add_executable(myapp ${myapp_SRCS} ${myapp_QMS})
#
# Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (clear_qt_translations)

	set(DIRS_TRANSLATIONS
		""
		CACHE INTERNAL "Directories to be translated" FORCE
	)

endmacro (clear_qt_translations)

macro (add_qt_translations)

	set(DIRS_TRANSLATIONS
		${DIRS_TRANSLATIONS}
		${ARGN}
		CACHE INTERNAL "Directories to be translated" FORCE
	)

endmacro (add_qt_translations)
