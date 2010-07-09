project(MainWindow)

if (TAGLIB)
	include_directories(${TAGLIB_INCLUDE_DIRS})
	add_definitions(${TAGLIB_DEFINITIONS})
endif (TAGLIB)

if (MEDIAINFOLIB)
	include_directories(${MEDIAINFOLIB_INCLUDE_DIRS})
endif (MEDIAINFOLIB)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

include_directories(${QTSINGLEAPPLICATION_INCLUDE_DIRS})
add_definitions(${QTSINGLEAPPLICATION_DEFINITIONS})

add_definitions(-DBUILD_MAINWINDOW)

# Otherwise QString::fromStdWString() is not defined
# This happens when compiling using the KDE4 libraries
remove_definitions(-DQT_NO_STL)

set(MainWindow_SRCS
	MainWindow.cpp
	AboutWindow.cpp
	LogWindow.cpp
)

if (MSVC)
	if (MSVC_VERSION GREATER 1399)
		# If using Visual C++ 2005 (MSVC80, MSVC_VERSION=1400) and greater
		# Do not treat wchar_t as built-in type otherwise QString::fromStdWString()
		# and QString::toStdWString() get unresolved
		# See http://www.qtforum.org/post/69786/problem-using-qstring-fromstdwstring.html
		# See http://www.qtcentre.org/forum/f-qt-programming-2/t-wchar-to-qstring-giving-error-in-vs2005-4625.html
		# See http://lists.trolltech.com/pipermail/qt-interest/2009-February/002259.html
		# See http://www.qtcentre.org/forum/f-qt-programming-2/t-finding-window-linker-error-12478.html
		# See http://thread.gmane.org/gmane.comp.lib.qt.general/8486
		# /Zc:wchar_t- is not recognized under Visual C++ .NET 2003 (MSVC71)
		set_source_files_properties(AboutWindow.cpp
			PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-"
		)
	endif (MSVC_VERSION GREATER 1399)
endif (MSVC)

qt4_wrap_cpp(MainWindow_SRCS
	MainWindow.h
	LogWindow.h
)

qt4_wrap_ui(MainWindow_SRCS
	AboutWindow.ui
	LogWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(MainWindow STATIC ${MainWindow_SRCS})
else (STATICPLUGINS)
	add_library(MainWindow SHARED ${MainWindow_SRCS})
endif (STATICPLUGINS)

target_link_libraries(MainWindow
	QuarkPlayerCore

	${QTSINGLEAPPLICATION_LIBRARIES}

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
)

if (MEDIAINFOLIB)
	target_link_libraries(MainWindow ${MEDIAINFOLIB_LIBRARIES})
endif (MEDIAINFOLIB)

# Translations
add_qt_translations(${MainWindow_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS MainWindow ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
