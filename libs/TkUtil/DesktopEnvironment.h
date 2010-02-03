/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef DESKTOPENVIRONMENT_H
#define DESKTOPENVIRONMENT_H

#include <TkUtil/tkutil_export.h>

/**
 * Detects at runtime which desktop environment is currently used.
 *
 * Detects if GNOME or KDE is running, this helps to use the best matching icons.
 *
 * @see http://en.wikipedia.org/wiki/Desktop_environment
 * @see QIcon::fromTheme()
 * @author Tanguy Krotoff
 */
class TKUTIL_API DesktopEnvironment {
public:

	enum Environment {
		/** Couldn't detect the desktop environment. */
		Unknown,

		/** Microsoft Windows is being used. */
		Windows,

		/** Mac OS X is being used. */
		Mac,

		/** KDE is being used. */
		KDE,

		/** GNOME is being used. */
		GNOME
	};

	/**
	 * Gets the currently used environment.
	 *
	 * @return the environment (KDE or GNOME or Unknow)
	 */
	static Environment env();

private:

	DesktopEnvironment();

	~DesktopEnvironment();
};

#endif	//DESKTOPENVIRONMENT_H
