/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef SIMPLEMPLAYER_H
#define SIMPLEMPLAYER_H

#include <QtGui/QMainWindow>

namespace Ui { class SimpleMPlayer; }

class MPlayerVideoWidget;
class MPlayerProcess;

/**
 * A simple player that uses libmplayer.
 *
 * @author Tanguy Krotoff
 */
class SimpleMPlayer : public QMainWindow {
	Q_OBJECT
public:

	SimpleMPlayer(QWidget * parent);

	~SimpleMPlayer();

private slots:

	void playFile();

	/**
	 * @see MPlayerProcess::videoWidgetSizeChanged()
	 */
	void videoWidgetSizeChanged(int width, int height);

private:

	Ui::SimpleMPlayer * _ui;

	MPlayerVideoWidget * _videoWidget;
	MPlayerProcess * _process;
};

#endif	//SIMPLEMPLAYER_H
