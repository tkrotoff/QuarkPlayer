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

#ifndef MEDIACONTROLLERTOOLBAR_H
#define MEDIACONTROLLERTOOLBAR_H

#include <tkutil/TkToolBar.h>

class QPushButton;
class QMenu;

/**
 * Media controller toolbar.
 *
 * @author Tanguy Krotoff
 */
class MediaControllerToolBar : public TkToolBar {
	Q_OBJECT
public:

	MediaControllerToolBar();

	~MediaControllerToolBar();

	QMenu * menuAudioChannels() const;

	QMenu * menuSubtitles() const;

private slots:

	void retranslate();

private:

	QPushButton * _audioChannelsButton;
	QMenu * _menuAudioChannels;

	QPushButton * _subtitlesButton;
	QMenu * _menuSubtitles;
};

#endif	//MEDIACONTROLLERTOOLBAR_H
