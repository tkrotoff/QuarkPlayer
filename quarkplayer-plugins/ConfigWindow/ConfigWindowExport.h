/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef CONFIGWINDOWEXPORT_H
#define CONFIGWINDOWEXPORT_H

#include <QtCore/QtGlobal>

#ifdef QT_STATICPLUGIN
	//This library is a static plugin
	#define CONFIGWINDOW_API
#else
	#ifdef BUILD_CONFIGWINDOW
		//We are building this library
		#define CONFIGWINDOW_API Q_DECL_EXPORT
	#else
		//We are using this library
		#define CONFIGWINDOW_API Q_DECL_IMPORT
	#endif
#endif

#endif	//CONFIGWINDOWEXPORT_H
