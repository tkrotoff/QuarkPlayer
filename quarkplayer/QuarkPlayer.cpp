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
#include "PluginsManager.h"

#include "config/Config.h"

#include <tkutil/TkFile.h>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>
#include <phonon/videowidget.h>
#include <phonon/mediacontroller.h>

#include <QtCore/QRegExp>

QuarkPlayer::QuarkPlayer(QObject * parent)
	: QObject(parent) {

	_currentMediaObject = NULL;
	_mediaController = NULL;

	//mainWindow is an internal plugin in fact...
	//FIXME I cannot manage to make it a real external plugin
	_mainWindow = new MainWindow(*this, QUuid::createUuid(), NULL);
	_mainWindow->show();

	connect(&PluginsManager::instance(), SIGNAL(allPluginsLoaded()), SLOT(allPluginsLoaded()));
}

QuarkPlayer::~QuarkPlayer() {
	PluginsManager::instance().deleteInstance();
	Config::instance().deleteInstance();
}

void QuarkPlayer::registerMetaTypes() {
}

MainWindow * QuarkPlayer::mainWindow() const {
	return _mainWindow;
}

void QuarkPlayer::setCurrentMediaObject(Phonon::MediaObject * mediaObject) {
	if (_currentMediaObject != mediaObject) {
		_currentMediaObject = mediaObject;
		emit currentMediaObjectChanged(_currentMediaObject);
	}
}

Phonon::MediaObject * QuarkPlayer::currentMediaObject() const {
	return _currentMediaObject;
}

QString QuarkPlayer::currentMediaObjectTitle() const {
	QString fullTitle;

	if (_currentMediaObject) {
		QString fileName = _currentMediaObject->currentSource().fileName();
		QMultiMap<QString, QString> metaData = _currentMediaObject->metaData();

		QString artist = metaData.value("ARTIST");
		QString title = metaData.value("TITLE");
		if (artist.isEmpty() && title.isEmpty()) {
			fullTitle = TkFile::removeFileExtension(TkFile::fileName(fileName));
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
	}

	return fullTitle;
}

QList<Phonon::MediaObject *> QuarkPlayer::mediaObjectList() const {
	return _mediaObjectList;
}

void QuarkPlayer::play(const Phonon::MediaSource & mediaSource) {
	Phonon::MediaSource tmp(mediaSource);

	if (_currentMediaObject) {

#ifdef Q_OS_WIN
		//Detects audio CD tracks under Windows
		//They have a special name: "D:/Track01.cda" means track 1 of the audio CD
		//from device D:/
		//Let's use a regexp to detect this case
		static QRegExp rx_windows_cdda("^(\\D+)Track(\\d+).cda$");
		QString fileName = tmp.fileName();
		if (!fileName.isEmpty() && rx_windows_cdda.indexIn(fileName) > -1) {
			QString deviceName = rx_windows_cdda.cap(1);
			int track = rx_windows_cdda.cap(2).toInt();	//FIXME Unused for the moment
			tmp = Phonon::MediaSource(Phonon::Cd, deviceName/*, track*/);
		}
		///
#endif	//Q_OS_WIN

		_currentMediaObject->setCurrentSource(tmp);
		_currentMediaObject->play();
	}
}

void QuarkPlayer::addFilesToPlaylist(const QStringList & files) {
	emit addFilesToCurrentPlaylist(files);
}

Phonon::AudioOutput * QuarkPlayer::currentAudioOutput() const {
	Phonon::AudioOutput * audioOutput = NULL;

	QList<Phonon::Path> outputPaths = _currentMediaObject->outputPaths();
	foreach (Phonon::Path outputPath, outputPaths) {
		//Cannot use qobject_cast<> since MediaNode is not a QObject
		audioOutput = dynamic_cast<Phonon::AudioOutput *>(outputPath.sink());
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
		audioOutput = dynamic_cast<Phonon::AudioOutput *>(outputPath.sink());
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
		videoWidget = dynamic_cast<Phonon::VideoWidget *>(outputPath.sink());
		if (videoWidget) {
			//We found the right video widget
			//FIXME There can be several video widget?
			break;
		}
	}

	return videoWidget;
}

void QuarkPlayer::volumeChanged(qreal volume) {
	Config::instance().setValue(Config::LAST_VOLUME_USED_KEY, volume);
}

void QuarkPlayer::mutedChanged(bool muted) {
	Config::instance().setValue(Config::VOLUME_MUTED_KEY, muted);
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

#ifndef KDE4_FOUND
	//QCoreApplication::setLibraryPaths(originalPaths);
#endif	//KDE4_FOUND

	//audioOutput
	Phonon::AudioOutput * audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	connect(audioOutput, SIGNAL(volumeChanged(qreal)), SLOT(volumeChanged(qreal)));
	connect(audioOutput, SIGNAL(mutedChanged(bool)), SLOT(mutedChanged(bool)));
	audioOutput->setVolume(Config::instance().lastVolumeUsed());
	audioOutput->setMuted(Config::instance().volumeMuted());

	//Associates the mediaObject with an audioOutput
	Phonon::createPath(_currentMediaObject, audioOutput);

	//Do it only now since we need the audioOutput to be created too
	_mediaObjectList.append(_currentMediaObject);
	emit mediaObjectAdded(_currentMediaObject);
	emit currentMediaObjectChanged(_currentMediaObject);

	return _currentMediaObject;
}

void QuarkPlayer::allPluginsLoaded() {
	//Let's create the Phonon MediaObject
	//since all plugins have been loaded
	//Create the Phonon MediaObject can be pretty slow:
	//it initializes the backend
	//So let's do it only when all the backends are loaded
	//Other advantage: plugins just have to connect to the signal
	//since they are already created
	createNewMediaObject();
}

void QuarkPlayer::setMediaController(Phonon::MediaController * mediaController) {
	if (_mediaController) {
		qCritical() << __FUNCTION__ << "Error: _mediaController is not NULL";
	}

	_mediaController = mediaController;
}

Phonon::MediaController * QuarkPlayer::mediaController() const {
	return _mediaController;
}
