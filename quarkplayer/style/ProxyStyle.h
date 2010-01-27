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

#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QtGui/QStyle>

/**
 * A base class to easily write new styles.
 *
 * @see http://doc.trolltech.com/qq/qq09-q-and-a.html#style
 * @author Tanguy Krotoff
 */
class ProxyStyle : public QStyle {
public:

	ProxyStyle(const QString & baseStyle);

	virtual ~ProxyStyle();

	virtual void drawComplexControl(ComplexControl control, const QStyleOptionComplex * option,
		QPainter * painter, const QWidget * widget = 0) const;

	virtual void drawControl(ControlElement element, const QStyleOption * option,
		QPainter * painter, const QWidget * widget = 0) const;

	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
		QPainter * painter, const QWidget * widget = 0) const;

	virtual int styleHint(StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0,
		QStyleHintReturn * returnData = 0) const;

	virtual void drawItemPixmap(QPainter * painter, const QRect & rectangle,
		int alignment, const QPixmap & pixmap) const;

	virtual void drawItemText(QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette,
		bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole) const;

	virtual QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap & pixmap,const QStyleOption * option) const;

	virtual SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex * option,
		const QPoint & position, const QWidget * widget = 0) const;

	virtual QRect itemPixmapRect(const QRect & rectangle, int alignment, const QPixmap & pixmap) const;

	virtual QRect itemTextRect(const QFontMetrics & metrics, const QRect & rectangle, int alignment,
		bool enabled, const QString & text) const;

	virtual int pixelMetric(PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0) const;

	virtual void polish(QWidget * widget);

	virtual void polish(QApplication * application);

	virtual void polish(QPalette & palette);

	virtual QSize sizeFromContents(ContentsType type, const QStyleOption * option, const QSize & contentsSize,
		const QWidget * widget = 0) const;

	virtual QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption * option = 0,
		const QWidget * widget = 0) const;

	virtual QPalette standardPalette() const;

	virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption * option = 0,
		const QWidget * widget = 0) const;

	virtual QRect subControlRect(ComplexControl control, const QStyleOptionComplex * option, SubControl subControl,
		const QWidget * widget = 0) const;

	virtual QRect subElementRect(SubElement element, const QStyleOption * option, const QWidget * widget = 0) const;

	virtual void unpolish(QWidget * widget);

	virtual void unpolish(QApplication * application);

private:

	QStyle * _style;
};

#endif	//PROXYSTYLE_H
