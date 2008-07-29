# - add_qt_translations(file1 ... fileN)
# Adds source files (*.cpp, *.h, *.cxx...) to a list of files to be translated.
# Creates a global variable named SRCS_TRANSLATIONS
# - clear_qt_translations()
# Clears global variable named SRCS_TRANSLATIONS
#
# Example:
# clear_qt_translations()
# set(myapp_SRCS
#     toto.cpp
#     tata.cpp
# )
# add_qt_translations(${myapp_SRCS})
# qt4_create_translation(myapp_QMS ${SRCS_TRANSLATIONS})
# add_executable(myapp ${myapp_SRCS} ${myapp_QMS})
#
# Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (clear_qt_translations)

	set(SRCS_TRANSLATIONS
		""
		CACHE INTERNAL "Source files (*.cpp, *.h, *.cxx...) to be translated" FORCE
	)

endmacro (clear_qt_translations)

macro (add_qt_translations)

	set(_fileList "")
	foreach(file ${ARGN})
		get_filename_component(_path ${file} ABSOLUTE)
		list(APPEND _fileList
			${_path}
		)
	endforeach(file ${ARGN})

	set(SRCS_TRANSLATIONS
		${SRCS_TRANSLATIONS}
		${_fileList}
		CACHE INTERNAL "Source files (*.cpp, *.h, *.cxx...) to be translated" FORCE
	)

endmacro (add_qt_translations)
