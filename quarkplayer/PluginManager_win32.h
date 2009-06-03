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

#ifndef PLUGINMANAGERWIN32_H
#define PLUGINMANAGERWIN32_H

/**
 * Specific Windows functions for PluginManager.
 *
 * In a separate file since it fails to compile otherwise :/
 *
 * @author Tanguy Krotoff
 */
class PluginManagerWin32 {
public:

	/**
	 * Controls whether the system will handle the specified types of serious errors or
	 * whether the process will handle them.
	 *
	 * 0 => Use the system default, which is to display all error dialog boxes
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms680621.aspx
	 */
	static void setErrorMode();

	/**
	 * Equivalent to rpath under Windows.
	 *
	 * Adds a directory to the search path used to locate DLLs for the application.
	 * SetDllDirectory is supported under Windows XP SP1 and later.
	 * Service Pack 1 (SP1) for Windows XP was released on 9 September 2002.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms686203.aspx
	 */
	static void setDllDirectory(const char * path);
};

#endif	//PLUGINMANAGERWIN32_H
