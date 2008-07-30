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
#include "PluginManager.h"

#include "config/Config.h"
#include "version.h"

#include <tkutil/TkFile.h>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>
#include <phonon/videowidget.h>

QuarkPlayer::QuarkPlayer(QObject * parent)
	: QObject(parent) {

	_currentMediaObject = NULL;

	//mainWindow is an internal plugin in fact...
	_mainWindow = new MainWindow(*this, NULL);
	_mainWindow->show();

	connect(&PluginManager::instance(), SIGNAL(allPluginsLoaded()), SLOT(allPluginsLoaded()));
}

QuarkPlayer::~QuarkPlayer() {
}

MainWindow & QuarkPlayer::mainWindow() const {
	return *_mainWindow;
}

void QuarkPlayer::setCurrentMediaObject(Phonon::MediaObject * mediaObject) {
	_currentMediaObject = mediaObject;
	emit currentMediaObjectChanged(_currentMediaObject);
}

Phonon::MediaObject * QuarkPlayer::currentMediaObject() const {
	return _currentMediaObject;
}

QString QuarkPlayer::currentMediaObjectTitle() const {
	QMultiMap<QString, QString> metaData = _currentMediaObject->metaData();

	QString fullTitle;
	QString artist = metaData.value("ARTIST");
	QString title = metaData.value("TITLE");
	if (artist.isEmpty() && title.isEmpty()) {
		fullTitle = TkFile::removeFileExtension(TkFile::fileName(_currentMediaObject->currentSource().fileName()));
	} else {
		if (!title.isEmpty()) {
			fullTitle = title;
		}
		if (!artist.isEmpty()) {
			if (!fullTitle.isEmpty()) {
				fullTitle += " - ";
			}
			fullTitle += artist;
		}
	}

	return fullTitle;
}

QList<Phonon::MediaObject *> QuarkPlayer::mediaObjectList() const {
	return _mediaObjectList;
}

void QuarkPlayer::play(const Phonon::MediaSource & mediaSource) {
	if (_currentMediaObject) {
		_currentMediaObject->setCurrentSource(mediaSource);
		_currentMediaObject->play();
	}
}

Phonon::AudioOutput * QuarkPlayer::currentAudioOutput() const {
	Phonon::AudioOutput * audioOutput = NULL;

	QList<Phonon::Path> outputPaths = _currentMediaObject->outputPaths();
	foreach (Phonon::Path outputPath, outputPaths) {
		//Cannot use qobject_cast<> since MediaNode is not a QObject
		audioOutput = dynamic_cast<Phonon::AudioOutput *>(outputPath.sinkNode());
		if (audioOutput) {
			//We found the right audio output
			//FIXME There can be several audio output?
			break;
		}
	}

	return audioOutput;
}

Phonon::Path QuarkPlayer::currentAudioOutputPath() const {
	Phonon::AudioOutput * audioOutput = NULL;
	Phonon::Path audioOutputPath;

	QList<Phonon::Path> outputPaths = _currentMediaObject->outputPaths();
	foreach (Phonon::Path outputPath, outputPaths) {
		//Cannot use qobject_cast<> since MediaNode is not a QObject
		audioOutput = dynamic_cast<Phonon::AudioOutput *>(outputPath.sinkNode());
		if (audioOutput) {
			//We found the right audio output
			//FIXME There can be several audio output?
			audioOutputPath = outputPath;
			break;
		}
	}

	return audioOutputPath;
}

Phonon::VideoWidget * QuarkPlayer::currentVideoWidget() const {
	Phonon::VideoWidget * videoWidget = NULL;

	QList<Phonon::Path> outputPaths = _currentMediaObject->outputPaths();
	foreach (Phonon::Path outputPath, outputPaths) {
		//Cannot use qobject_cast<> since MediaNode is not a QObject
		videoWidget = dynamic_cast<Phonon::VideoWidget *>(outputPath.sinkNode());
		if (videoWidget) {
			//We found the right video widget
			//FIXME There can be several video widget?
			break;
		}
	}

	return videoWidget;
}

Config & QuarkPlayer::config() const {
	return Config::instance();
}

void QuarkPlayer::volumeChanged(qreal volume) {
	//TODO move to another class?
	Config & config = Config::instance();
	config.setValue(Config::LAST_VOLUME_USED_KEY, volume);
}

Phonon::MediaObject * QuarkPlayer::createNewMediaObject() {
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
	_currentMediaObject = new Phonon::MediaObject(this);
	_currentMediaObject->setTickInterval(1000);
	_mediaObjectList.append(_currentMediaObject);
	emit mediaObjectAdded(_currentMediaObject);
	//emit currentMediaObjectChanged(_currentMediaObject);

#ifndef KDE4_FOUND
	//QCoreApplication::setLibraryPaths(originalPaths);
#endif	//KDE4_FOUND

	//audioOutput
	Phonon::AudioOutput * audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	connect(audioOutput, SIGNAL(volumeChanged(qreal)), SLOT(volumeChanged(qreal)));
	audioOutput->setVolume(Config::instance().lastVolumeUsed());

	//Associates the mediaObject with an audioOutput
	Phonon::createPath(_currentMediaObject, audioOutput);

	return _currentMediaObject;
}

void QuarkPlayer::allPluginsLoaded() {
	createNewMediaObject();
}
