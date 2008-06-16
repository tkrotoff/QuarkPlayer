/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef TKICON_H
#define TKICON_H

#include "config.h"

#ifdef KDE4_FOUND
	#include <KIcon>
	typedef KIcon PrivateIcon;
#else
	#include <QtGui/QIcon>
	typedef QIcon PrivateIcon;
#endif	//KDE4_FOUND

/**
 * Loads a standard icon.
 *
 * Thin wrapper above QIcon and KIcon.
 * A standard pixmap/icon is a one that follows KDE naming conventions.
 * see Oxygen and freedesktop.org naming conventions.
 *
 * @see http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @see http://tango.freedesktop.org/
 * @see QIcon
 * @see KIcon
 * @author Tanguy Krotoff
 */
class TkIcon : public PrivateIcon {
public:

	TkIcon(const QString & standardIconName);

	static void setIconTheme(const QString & iconTheme);

	static void setIconSize(int iconSize);

private:

	static QString _iconTheme;

	static QString _iconSize;
};

#endif	//TKICON_H
