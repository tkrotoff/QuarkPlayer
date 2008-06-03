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

#ifndef MEDIACONTROLLER_H
#define MEDIACONTROLLER_H

#include <QtCore/QObject>

class MainWindow;

namespace Phonon {
	class MediaController;
	class MediaObject;
}

class QWidget;

/**
 * Handles Phonon::MediaController.
 *
 * @author Tanguy Krotoff
 */
class MediaController : public QObject {
	Q_OBJECT
public:

	MediaController(MainWindow * mainWindow, Phonon::MediaObject * mediaObject, QWidget * parent);

	~MediaController();

private slots:

	void openSubtitleFile();

	void availableAudioChannelsChanged();
	void actionAudioChannelTriggered(int id);

	void availableSubtitlesChanged();
	void actionSubtitleTriggered(int id);

	void availableTitlesChanged();
	void actionTitleTriggered(int id);

	void availableChaptersChanged();
	void actionChapterTriggered(int id);

	void availableAnglesChanged();
	void actionAngleTriggered(int id);

private:

	void removeAllAction(QWidget * widget);

	MainWindow * _mainWindow;

	QWidget * _parent;

	Phonon::MediaController * _mediaController;
};

#endif	//MEDIACONTROLLER_H
