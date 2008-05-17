# - reduce_qt_include_dirs()
# Optimization: reduce Qt include directories and thus compilation time
#
# Inside your source code you must replace all #include <Q...> by #include <QtCore/Q...>
#
# Remove all Qt modules from include path
# The modules were automatically added to the include path by file UseQt4.cmake
# Inspired by UseQt4.cmake
#
# Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (reduce_qt_include_dirs)

	get_directory_property(include_dirs INCLUDE_DIRECTORIES)
	foreach(module QT3SUPPORT QTOPENGL QTASSISTANT QTDESIGNER QTMOTIF QTNSPLUGIN
		QTSCRIPT QTSVG QTUITOOLS QTHELP QTWEBKIT PHONON QTGUI QTTEST
		QTDBUS QTXML QTSQL QTXMLPATTERNS QTNETWORK QTCORE)

		if (QT_${module}_FOUND)
			string(REPLACE ${QT_${module}_INCLUDE_DIR} "" include_dirs "${include_dirs}")
		endif (QT_${module}_FOUND)
	endforeach(module)
	set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "${include_dirs}")

endmacro (reduce_qt_include_dirs)
