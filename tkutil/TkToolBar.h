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

#ifndef TKTOOLBAR_H
#define TKTOOLBAR_H

#include <tkutil/config.h>

#ifdef KDE4_FOUND
	#include <KToolBar>
	typedef KToolBar PrivateToolBar;
#else
	#include <QtGui/QToolBar>
	typedef QToolBar PrivateToolBar;
#endif	//KDE4_FOUND

/**
 * Thin class above QToolBar/KToolBar.
 *
 * @see QToolBar
 * @see KToolBar
 * @author Tanguy Krotoff
 */
class TkToolBar : public PrivateToolBar {
public:

	/** Makes a parent compulsary, KToolBar requires a parent, not QToolBar. */
	TkToolBar(QWidget * parent)
		: PrivateToolBar(parent) {
	}

private:
};

#endif	//TKTOOLBAR_H
