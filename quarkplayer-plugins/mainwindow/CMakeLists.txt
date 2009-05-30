project(mainwindow)

add_definitions(-DQT_QTSINGLEAPPLICATION_IMPORT)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

# Otherwise QString::fromStdWString() is not defined
# This happens when compiling using the KDE4 libraries
remove_definitions(-DQT_NO_STL)

# Takes a lot of time :/
set(QUARKPLAYER_CORE_LOC 0)
set(QUARKPLAYER_PLUGINS_LOC 0)
set(QUARKPLAYER_LIBS_LOC 0)
set(QUARKPLAYER_3RDPARTY_LOC 0)
loc_counter(${CMAKE_SOURCE_DIR}/quarkplayer QUARKPLAYER_CORE_LOC 1)
loc_counter(${CMAKE_SOURCE_DIR}/quarkplayer-plugins QUARKPLAYER_PLUGINS_LOC 1)
loc_counter(${CMAKE_SOURCE_DIR}/libs QUARKPLAYER_LIBS_LOC 1)
loc_counter(${CMAKE_SOURCE_DIR}/3rdparty QUARKPLAYER_3RDPARTY_LOC 1)
configure_file(metrics.h.in ${CMAKE_CURRENT_BINARY_DIR}/metrics.h)

set(mainwindow_SRCS
	MainWindow.cpp
	AboutWindow.cpp
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

qt4_wrap_cpp(mainwindow_SRCS
	MainWindow.h
)

qt4_wrap_ui(mainwindow_SRCS
	AboutWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(mainwindow STATIC ${mainwindow_SRCS})
else (STATICPLUGINS)
	add_library(mainwindow SHARED ${mainwindow_SRCS})
endif (STATICPLUGINS)

target_link_libraries(mainwindow
	quarkplayercore
	qtsingleapplication
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
)

if (MEDIAINFOLIB)
	target_link_libraries(mainwindow mediainfo)
endif (MEDIAINFOLIB)

# Translations
add_qt_translations(${mainwindow_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS mainwindow ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
