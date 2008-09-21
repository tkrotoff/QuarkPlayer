# - add_qt_translations(file1 ... fileN)
# Adds source code files (*.cpp, *.c...) to be translated.
# Creates global variables named SRCS_TRANSLATIONS and DIRS_TRANSLATIONS
# - clear_qt_translations()
# Clears global variables named SRCS_TRANSLATIONS and DIRS_TRANSLATIONS
#
# Example:
# clear_qt_translations()
# set(myapp_SRCS
#     toto.cpp
#     tata.cpp
# )
# add_qt_translations(${myapp_SRCS})
#
# set(TS_FILES
# 	"${CMAKE_CURRENT_SOURCE_DIR}/translations/myapp_fr.ts"
# 	"${CMAKE_CURRENT_SOURCE_DIR}/translations/myapp_de.ts"
# 	"${CMAKE_CURRENT_SOURCE_DIR}/translations/myapp_es.ts"
# )
# set(QM_FILES "")
# qt4_create_translation(QM_FILES ${DIRS_TRANSLATIONS} ${TS_FILES})
# add_custom_command(OUTPUT ${TS_FILES} DEPENDS ${SRCS_TRANSLATIONS} APPEND)
#
# add_executable(myapp ${myapp_SRCS} ${QM_FILES})
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

	set(SRCS_TRANSLATIONS
		""
		CACHE INTERNAL "Sources (*.cpp, *.c...) to be translated" FORCE
	)

endmacro (clear_qt_translations)

macro (add_qt_translations)

	set(_pathList)
	set(_absoluteFilenameList)
	foreach(_file ${ARGN})
		get_filename_component(_absoluteFilename ${_file} ABSOLUTE)
		list(APPEND _absoluteFilenameList ${_absoluteFilename})

		get_filename_component(_path ${_absoluteFilename} PATH)
		list(APPEND _pathList ${_path})
	endforeach(_file ${ARGN})

	list(REMOVE_DUPLICATES _pathList)
	list(REMOVE_DUPLICATES _absoluteFilenameList)

	set(DIRS_TRANSLATIONS
		${DIRS_TRANSLATIONS}
		${_pathList}
		CACHE INTERNAL "Directories to be translated" FORCE
	)

	set(SRCS_TRANSLATIONS
		${SRCS_TRANSLATIONS}
		${_absoluteFilenameList}
		CACHE INTERNAL "Sources (*.cpp, *.c...) to be translated" FORCE
	)

endmacro (add_qt_translations)
