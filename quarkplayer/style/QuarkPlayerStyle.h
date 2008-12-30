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

#ifndef QUARKPLAYERSTYLE_H
#define QUARKPLAYERSTYLE_H

#include <QtGui/QStyle>

/**
 * QuarkPlayer Qt style for correcting some bugs or ugly style.
 *
 * - Removes the ugly toolbar bottom line under Windows
 * - Removes the ugly frame/marging around the status bar icons under Windows
 *
 * @author Tanguy Krotoff
 */
class QuarkPlayerStyle : public QStyle {
public:

	QuarkPlayerStyle();

	~QuarkPlayerStyle();


	void drawComplexControl(ComplexControl control, const QStyleOptionComplex * option, 
		QPainter * painter, const QWidget * widget = 0) const {
		_systemStyle->drawComplexControl(control, option, painter, widget);
	}

	void drawControl(ControlElement element, const QStyleOption * option,
		QPainter * painter, const QWidget * widget = 0) const;

	void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
		QPainter * painter, const QWidget * widget = 0) const;

	int styleHint(StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0,
		QStyleHintReturn * returnData = 0) const {
		return _systemStyle->styleHint(hint, option, widget, returnData);
	}

	void drawItemPixmap(QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap) const {
		_systemStyle->drawItemPixmap(painter, rectangle, alignment, pixmap);
	}

	void drawItemText(QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette,
		bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole) const {
		_systemStyle->drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole);
	}

	QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option) const {
		return _systemStyle->generatedIconPixmap(iconMode, pixmap, option);
	}

	SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex * option,
		const QPoint & position, const QWidget * widget = 0) const {
		return _systemStyle->hitTestComplexControl(control, option, position, widget);
	}

	QRect itemPixmapRect(const QRect & rectangle, int alignment, const QPixmap & pixmap) const {
		return _systemStyle->itemPixmapRect(rectangle, alignment, pixmap);
	}

	QRect itemTextRect(const QFontMetrics & metrics, const QRect & rectangle, int alignment,
		bool enabled, const QString & text) const {
		return _systemStyle->itemTextRect(metrics, rectangle, alignment, enabled, text);
	}

	int pixelMetric(PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0) const {
		return _systemStyle->pixelMetric(metric, option, widget);
	}

	void polish(QWidget * widget) {
		_systemStyle->polish(widget);
	}

	void polish(QApplication * application) {
		_systemStyle->polish(application);
	}

	void polish(QPalette & palette) {
		_systemStyle->polish(palette);
	}

	QSize sizeFromContents(ContentsType type, const QStyleOption * option, const QSize & contentsSize,
		const QWidget * widget = 0) const {
		return _systemStyle->sizeFromContents(type, option, contentsSize, widget);
	}

	QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption * option = 0,
		const QWidget * widget = 0) const {
		return _systemStyle->standardIcon(standardIcon, option, widget);
	}

	QPalette standardPalette() const {
		return _systemStyle->standardPalette();
	}

	QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption * option = 0,
		const QWidget * widget = 0) const {
		return _systemStyle->standardPixmap(standardPixmap, option, widget);
	}

	QRect subControlRect(ComplexControl control, const QStyleOptionComplex * option, SubControl subControl,
		const QWidget * widget = 0) const {
		return _systemStyle->subControlRect(control, option, subControl, widget);
	}

	QRect subElementRect(SubElement element, const QStyleOption * option, const QWidget * widget = 0) const {
		return _systemStyle->subElementRect(element, option, widget);
	}

	void unpolish(QWidget * widget) {
		_systemStyle->unpolish(widget);
	}

	void unpolish(QApplication * application) {
		_systemStyle->unpolish(application);
	}

private:

	QStyle * _systemStyle;
};

#include <QtGui/QStylePlugin>

/**
 * QStylePlugin.
 *
 * @author Tanguy Krotoff
 */
class QuarkPlayerStylePlugin : public QStylePlugin {
public:

	QStringList keys() const;

	QStyle * create(const QString & key);
};

#endif	//QUARKPLAYERSTYLE_H
