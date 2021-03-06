/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <TkUtil/TkUtilExport.h>

/**
 * Enables/disables the screensaver.
 *
 * Under Windows, this class catches Windows events SC_SCREENSAVE and SC_MONITORPOWER.
 * This class does not use Windows function SystemParametersInfo() with parameter SPI_SETSCREENSAVEACTIVE
 * because in case the application crashes it won't restore the screensaver and power parameters from Windows.
 * Instead by catching events SC_SCREENSAVE and SC_MONITORPOWER we don't modify Windows parameters
 * and thus no problem if the application crashes.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API ScreenSaver {
public:

	static void disable();

	static void restore();

private:

	ScreenSaver();
};

#endif	//SCREENSAVER_H
