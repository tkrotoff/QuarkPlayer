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

#include "DesktopEnvironment.h"

#include "TkUtilLogger.h"

#include <QtCore/QtGlobal>
#include <QtCore/QString>

DesktopEnvironment desktopEnvironment() {
	DesktopEnvironment platform = Unknown;

#ifdef Q_WS_WIN
	platform = Windows;
#elif defined Q_WS_MAC
	platform = Mac;
#elif defined Q_WS_X11
	QByteArray desktopEnvironment = qgetenv("DESKTOP_SESSION");
	if (desktopEnvironment == "kde") {
		platform = KDE;
	} else if (desktopEnvironment == "gnome") {
		platform = GNOME;
	}
#endif

	return platform;
}
