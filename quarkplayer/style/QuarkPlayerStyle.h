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

#ifndef QUARKPLAYERSTYLE_H
#define QUARKPLAYERSTYLE_H

#include <quarkplayer/QuarkPlayerExport.h>

#include <TkUtil/Singleton.h>

#include <QtGui/QProxyStyle>

/**
 * QuarkPlayer Qt style for correcting some bugs or ugly style.
 *
 * - Removes the ugly toolbar bottom line under Windows
 * - Removes the ugly frame/marging around the status bar icons under Windows
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API QuarkPlayerStyle : public QProxyStyle, public Singleton {
public:

	/** Singleton. */
	static QuarkPlayerStyle & instance();

	void drawControl(ControlElement element, const QStyleOption * option,
		QPainter * painter, const QWidget * widget = 0) const;

	void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
		QPainter * painter, const QWidget * widget = 0) const;

private:

	QuarkPlayerStyle();

	~QuarkPlayerStyle();
};

#endif	//QUARKPLAYERSTYLE_H
