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

#include "QuarkPlayerStyle.h"

#include <QtGui/QtGui>

QuarkPlayerStyle::QuarkPlayerStyle() {
	QString styleName = QApplication::style()->objectName();
	_systemStyle = QStyleFactory::create(styleName);
}

QuarkPlayerStyle::~QuarkPlayerStyle() {
	delete _systemStyle;
}

void QuarkPlayerStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex * option,
	QPainter * painter, const QWidget * widget) const {

	_systemStyle->drawComplexControl(control, option, painter, widget);
}

void QuarkPlayerStyle::drawControl(ControlElement element, const QStyleOption * option,
	QPainter * painter, const QWidget * widget) const {

	//QToolbar
	//Removes the ugly toolbar bottom line
	if (element == CE_ToolBar) {
		return;
	}

	_systemStyle->drawControl(element, option, painter, widget);
}

void QuarkPlayerStyle::drawPrimitive(PrimitiveElement element, const QStyleOption * option,
	QPainter * painter, const QWidget * widget) const {

	//QStatusBar
	//Removes the ugly frame/marging around the status bar icons under Windows
	if (element == PE_FrameStatusBar) {
		return;
	}

	_systemStyle->drawPrimitive(element, option, painter, widget);
}
