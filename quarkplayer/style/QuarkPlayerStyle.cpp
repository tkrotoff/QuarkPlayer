/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "QuarkPlayerStyle.h"

#include <quarkplayer/QuarkPlayerCoreLogger.h>

#include <QtGui/QtGui>

QuarkPlayerStyle::QuarkPlayerStyle() {
}

QuarkPlayerStyle::~QuarkPlayerStyle() {
}

QuarkPlayerStyle & QuarkPlayerStyle::instance() {
	static QuarkPlayerStyle instance;
	return instance;
}

void QuarkPlayerStyle::drawControl(ControlElement element, const QStyleOption * option,
	QPainter * painter, const QWidget * widget) const {

	//QToolbar
	//Removes the ugly toolbar bottom line under Windows
	if (element == CE_ToolBar) {
		return;
	}

	QProxyStyle::drawControl(element, option, painter, widget);
}

void QuarkPlayerStyle::drawPrimitive(PrimitiveElement element, const QStyleOption * option,
	QPainter * painter, const QWidget * widget) const {

	//QStatusBar
	//Removes the ugly frame/marging around the status bar icons under Windows
	if (element == PE_FrameStatusBar) {
		return;
	}

	QProxyStyle::drawPrimitive(element, option, painter, widget);
}
