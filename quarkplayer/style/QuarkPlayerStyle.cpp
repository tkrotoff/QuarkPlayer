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

#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(quarkplayerstyle, QuarkPlayerStylePlugin)

static const QString QUARKPLAYERSTYLE_NAME = "QuarkPlayerStyle";

//Saves the last valid system style name (i.e valid means different than QuarkPlayerStyle)
//This permit to avoid a crash when applying QuarkPlayerStyle on QuarkPlayerStyle
static QString _lastValidSystemStyleName;

QuarkPlayerStyle::QuarkPlayerStyle() {
	QString styleName = QApplication::style()->objectName();

	if (styleName != QUARKPLAYERSTYLE_NAME.toLower()) {
		_lastValidSystemStyleName = styleName;
		_systemStyle = QStyleFactory::create(_lastValidSystemStyleName);
	} else {
		//If styleName == QUARKPLAYERSTYLE_NAME, then we end up in recursion
		_systemStyle = QStyleFactory::create(_lastValidSystemStyleName);
	}

	if (!_systemStyle) {
		_systemStyle = QStyleFactory::create("windows");
	}
}

QuarkPlayerStyle::~QuarkPlayerStyle() {
	//delete _systemStyle;
}

void QuarkPlayerStyle::drawControl(ControlElement element, const QStyleOption * option,
	QPainter * painter, const QWidget * widget) const {

	//QToolbar
	//Removes the ugly toolbar bottom line under Windows
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

//QStylePlugin
QStringList QuarkPlayerStylePlugin::keys() const {
	return QStringList() << QUARKPLAYERSTYLE_NAME;
}

QStyle * QuarkPlayerStylePlugin::create(const QString & key) {
	if (key.toLower() == QUARKPLAYERSTYLE_NAME.toLower()) {
		return new QuarkPlayerStyle();
	}
	return NULL;
}
