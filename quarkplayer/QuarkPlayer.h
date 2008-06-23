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

#ifndef QUARKPLAYER_H
#define QUARKPLAYER_H

#include <quarkplayer/quarkplayer_export.h>

#include <QtCore/QObject>

#include <phonon/phononnamespace.h>
#include <phonon/path.h>

class MainWindow;
class Config;

namespace Phonon {
	class MediaObject;
	class AudioOutput;
	class MediaSource;
	class VideoWidget;
}

/**
 * Gives access to all important objects of the application.
 *
 * Pattern proxy.
 *
 * Main class for writing plugins.
 *
 * How to make dynamic plugins? i.e possibility to load/reload them at runtime?
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API QuarkPlayer : public QObject {
	Q_OBJECT
public:

	QuarkPlayer(QObject * parent);

	~QuarkPlayer();

	MainWindow & mainWindow() const;

	Phonon::AudioOutput & currentAudioOutput() const;

	Phonon::Path currentAudioOutputPath() const;

	Phonon::MediaObject & currentMediaObject() const;

	void setCurrentVideoWidget(Phonon::VideoWidget * videoWidget);
	Phonon::VideoWidget * currentVideoWidget() const;

	Config & config() const;

private slots:

	void volumeChanged(qreal volume);

private:

	MainWindow * _mainWindow;

	Phonon::MediaObject * _mediaObject;
	Phonon::AudioOutput * _audioOutput;
	Phonon::Path _audioOutputPath;
	Phonon::MediaSource * _mediaSource;
	Phonon::VideoWidget * _videoWidget;
};

#endif	//QUARKPLAYER_H
