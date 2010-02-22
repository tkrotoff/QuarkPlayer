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

#include "TkToolBar.h"

TkToolBar::TkToolBar(QWidget * parent)
	: PrivateToolBar(parent) {
}

void TkToolBar::setToolButtonStyle(QToolBar * toolBar) {
#ifdef Q_WS_WIN
	toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#else
	//Defines the style of all tool buttons that are added as QActions
	//By default under GNOME Qt::ToolButtonFollowStyle will show
	//the QActions text beside the icons
	toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
#endif	//Q_WS_WIN
}
