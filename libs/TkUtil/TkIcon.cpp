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

#include "TkIcon.h"

#include <QtGui/QtGui>

#include <QtCore/QDebug>

QString TkIcon::_iconTheme;
QString TkIcon::_iconSize;

//Example: ":/icons/oxygen/16x16/actions/media-playback-start.png"
//Example: ":/oxygen/16x16/media-playback-start"
TkIcon::TkIcon(const QString & standardIconName)
#ifdef KDE4_FOUND
	: PrivateIcon(standardIconName)
#else
	: PrivateIcon(":/" + _iconTheme + '/' + _iconSize + '/' + standardIconName)
#endif	//KDE4_FOUND
	{

	/*QString filename = Helper::appHomePath() + _fileName;
	if (!QFile::exists(filename)) {
	}*/
}

TkIcon::~TkIcon() {
}

void TkIcon::setIconTheme(const QString & iconTheme) {
	_iconTheme = iconTheme;
}

void TkIcon::setIconSize(int iconSize) {
	QString size = QString::number(iconSize);
	_iconSize = size + 'x' + size;
}
