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

#include "ProxyStyle.h"

#include <QtGui/QStyleFactory>

ProxyStyle::ProxyStyle(const QString & baseStyle) {
	_style = QStyleFactory::create(baseStyle);
	if (!_style) {
		_style = QStyleFactory::create("windows");
	}
}

ProxyStyle::~ProxyStyle() {
	//delete _style;
}

void ProxyStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex * option,
	QPainter * painter, const QWidget * widget) const {
	_style->drawComplexControl(control, option, painter, widget);
}

void ProxyStyle::drawControl(ControlElement element, const QStyleOption * option,
	QPainter * painter, const QWidget * widget) const {
	_style->drawControl(element, option, painter, widget);
}

void ProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption * option,
	QPainter * painter, const QWidget * widget) const {
	_style->drawPrimitive(element, option, painter, widget);
}

int ProxyStyle::styleHint(StyleHint hint, const QStyleOption * option, const QWidget * widget,
	QStyleHintReturn * returnData) const {
	return _style->styleHint(hint, option, widget, returnData);
}

void ProxyStyle::drawItemPixmap(QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap) const {
	_style->drawItemPixmap(painter, rectangle, alignment, pixmap);
}

void ProxyStyle::drawItemText(QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette,
	bool enabled, const QString & text, QPalette::ColorRole textRole) const {
	_style->drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole);
}

QPixmap ProxyStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option) const {
	return _style->generatedIconPixmap(iconMode, pixmap, option);
}

QStyle::SubControl ProxyStyle::hitTestComplexControl(ComplexControl control, const QStyleOptionComplex * option,
	const QPoint & position, const QWidget * widget) const {
	return _style->hitTestComplexControl(control, option, position, widget);
}

QRect ProxyStyle::itemPixmapRect(const QRect & rectangle, int alignment, const QPixmap & pixmap) const {
	return _style->itemPixmapRect(rectangle, alignment, pixmap);
}

QRect ProxyStyle::itemTextRect(const QFontMetrics & metrics, const QRect & rectangle, int alignment,
	bool enabled, const QString & text) const {
	return _style->itemTextRect(metrics, rectangle, alignment, enabled, text);
}

int ProxyStyle::pixelMetric(PixelMetric metric, const QStyleOption * option, const QWidget * widget) const {
	return _style->pixelMetric(metric, option, widget);
}

void ProxyStyle::polish(QWidget * widget) {
	_style->polish(widget);
}

void ProxyStyle::polish(QApplication * application) {
	_style->polish(application);
}

void ProxyStyle::polish(QPalette & palette) {
	_style->polish(palette);
}

QSize ProxyStyle::sizeFromContents(ContentsType type, const QStyleOption * option, const QSize & contentsSize,
	const QWidget * widget) const {
	return _style->sizeFromContents(type, option, contentsSize, widget);
}

QIcon ProxyStyle::standardIcon(StandardPixmap standardIcon, const QStyleOption * option,
	const QWidget * widget) const {
	return _style->standardIcon(standardIcon, option, widget);
}

QPalette ProxyStyle::standardPalette() const {
	return _style->standardPalette();
}

QPixmap ProxyStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption * option,
	const QWidget * widget) const {
	return _style->standardPixmap(standardPixmap, option, widget);
}

QRect ProxyStyle::subControlRect(ComplexControl control, const QStyleOptionComplex * option, SubControl subControl,
	const QWidget * widget) const {
	return _style->subControlRect(control, option, subControl, widget);
}

QRect ProxyStyle::subElementRect(SubElement element, const QStyleOption * option, const QWidget * widget) const {
	return _style->subElementRect(element, option, widget);
}

void ProxyStyle::unpolish(QWidget * widget) {
	_style->unpolish(widget);
}

void ProxyStyle::unpolish(QApplication * application) {
	_style->unpolish(application);
}
