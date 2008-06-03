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

#ifndef ICONENGINE_H
#define ICONENGINE_H

#include <QtGui/QIconEngineV2>

/**
 *
 *
 * @author Tanguy Krotoff
 */
class IconEngine : public QIconEngineV2 {
public:

	void paint(QPainter * painter, const QRect & rect, QIcon::Mode mode, QIcon::State state);

private:
};

#endif	//ICONENGINE_H
