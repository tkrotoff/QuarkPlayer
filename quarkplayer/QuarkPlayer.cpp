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

#include "QuarkPlayer.h"

#include "MainWindow.h"

#include "config/Config.h"
#include "version.h"

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>
#include <phonon/videowidget.h>

QuarkPlayer::QuarkPlayer(QObject * parent)
	: QObject(parent) {

#ifndef KDE4_FOUND
	//FIXME Backend selection, this is a hack
	//Not available under KDE, systemsettings will do it
	/*QStringList originalPaths = QCoreApplication::libraryPaths();
	QString backendName = Config::instance().backend();
	QStringList paths;
	paths << QCoreApplication::applicationDirPath() + "/" + backendName;
	QCoreApplication::setLibraryPaths(paths);
	*/
#endif	//KDE4_FOUND

	//mediaObject
	_mediaObject = new Phonon::MediaObject(this);
	_mediaObject->setTickInterval(1000);

#ifndef KDE4_FOUND
	//QCoreApplication::setLibraryPaths(originalPaths);
#endif	//KDE4_FOUND

	//audioOutput
	_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	connect(_audioOutput, SIGNAL(volumeChanged(qreal)), SLOT(volumeChanged(qreal)));
	_audioOutput->setVolume(Config::instance().lastVolumeUsed());

	//Associates the mediaObject with an audioOutput
	_audioOutputPath = Phonon::createPath(_mediaObject, _audioOutput);

	//videoWidget
	_videoWidget = NULL;

	//mainWindow is an internal plugin in fact...
	_mainWindow = new MainWindow(*this, NULL);
	_mainWindow->show();
}

QuarkPlayer::~QuarkPlayer() {
}

MainWindow & QuarkPlayer::mainWindow() const {
	return *_mainWindow;
}

Phonon::AudioOutput & QuarkPlayer::currentAudioOutput() const {
	return *_audioOutput;
}

Phonon::Path QuarkPlayer::currentAudioOutputPath() const {
	return _audioOutputPath;
}

Phonon::MediaObject & QuarkPlayer::currentMediaObject() const {
	return *_mediaObject;
}

void QuarkPlayer::setCurrentVideoWidget(Phonon::VideoWidget * videoWidget) {
	_videoWidget = videoWidget;
}

Phonon::VideoWidget * QuarkPlayer::currentVideoWidget() const {
	return _videoWidget;
}

Config & QuarkPlayer::config() const {
	return Config::instance();
}

void QuarkPlayer::volumeChanged(qreal volume) {
	//TODO move to another class?
	Config & config = Config::instance();
	config.setValue(Config::LAST_VOLUME_USED_KEY, volume);
}
