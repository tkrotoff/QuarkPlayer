/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PluginManager_win32.h"

#include <QtCore/QtGlobal>

#ifdef Q_WS_WIN
	#define _WIN32_WINNT 0x0502
	#include <QtCore/qt_windows.h>
#endif	//Q_WS_WIN

void PluginManagerWin32::setErrorMode() {
#ifdef Q_WS_WIN
	SetErrorMode(0);
#endif	//Q_WS_WIN
}

void PluginManagerWin32::setDllDirectory(const char * path) {
	Q_ASSERT(path);

#ifdef Q_WS_WIN
	//Use SetEnvironmentVariable() ?
	//See http://msdn.microsoft.com/en-us/library/ms686206.aspx
	//See http://www.google.com/codesearch/p?hl=en#k_74qd9ASEQ/trunk/psycle/src/psycle/host/Plugin.cpp&q=SetDllDirectory
	//SetDllDirectoryA(".\\plugins");
	SetDllDirectoryA(path);
#endif	//Q_WS_WIN
}
