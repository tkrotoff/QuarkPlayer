/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Trolltech ASA. All rights reserved.
 * Copyright (C) 2008  Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef SQUEEZELABEL_H
#define SQUEEZELABEL_H

#include <TkUtil/TkUtilExport.h>

#include <QtGui/QLabel>

/**
 * A replacement for QLabel that squeezes its text.
 *
 * If the text is too long to fit into the label it is divided into
 * remaining left and right parts which are separated by three dots.
 *
 * Example:
 * @code
 * http://www.kde.org/documentation/index.html could be squeezed to
 * http://www.kde...ion/index.html
 * @endcode
 *
 * Taken from Trolltech Browser demo and Arora source code
 * @see http://github.com/Arora/arora/tree/master
 * @see http://websvn.kde.org/trunk/KDE/kdelibs/kdeui/widgets/ksqueezedtextlabel.cpp
 */
class TKUTIL_API SqueezeLabel : public QLabel {
public:

	SqueezeLabel(QWidget * parent = 0);

	SqueezeLabel(const QString & text, QWidget * parent = 0);

	QSize minimumSizeHint() const;

protected:

	void paintEvent(QPaintEvent * event);

private:

	/**
	 * Removes all HTML tags to keep only the real plain text.
	 *
	 * This is done because HTML tags inside QLabel are invisible
	 * to the end user, thus we have to calculate the size of the text
	 * using the real plain text not the text containing the HTML tags.
	 */
	QString plainText() const;
};

#endif	//SQUEEZELABEL_H
