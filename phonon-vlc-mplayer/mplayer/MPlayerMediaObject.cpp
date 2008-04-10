/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MPlayerMediaObject.h"

#include "../VideoWidget.h"

#include <mplayer/MPlayerProcess.h>
#include <mplayer/MPlayerLoader.h>

#include <QtCore/QTimer>

namespace Phonon
{
namespace VLC_MPlayer
{

static MPlayerLoader mplayerLoader(NULL);

MPlayerMediaObject::MPlayerMediaObject(QObject * parent)
	: QObject(parent) {

	//Default MediaObject state is Phonon::LoadingState
	_currentState = Phonon::LoadingState;

	_process = NULL;
	_mediaDataLoader = NULL;

	_currentTime = 0;
	_totalTime = 0;

	_videoWidgetId = 0;
}

MPlayerMediaObject::~MPlayerMediaObject() {
}

void MPlayerMediaObject::setVideoWidgetId(int videoWidgetId) {
	_videoWidgetId = videoWidgetId;
}

void MPlayerMediaObject::loadMedia(const QString & filename) {
	_playRequestReached = false;

	_filename = filename;

	//Optimization:
	//wait to see if play() is run just after loadMedia()
	//100 milliseconds should be OK
	QTimer::singleShot(50, this, SLOT(loadMediaInternal()));
}

void MPlayerMediaObject::loadMediaInternal() {
	if (_playRequestReached) {
		//We are already playing the media,
		//so there no need to load it
		return;
	}

	_mediaDataLoader = mplayerLoader.loadMedia(_filename);
	connect(_mediaDataLoader, SIGNAL(finished()),
		SLOT(mediaLoaded()));
}

void MPlayerMediaObject::mediaLoaded() {
	MediaData mediaData;
	if (_mediaDataLoader) {
		mediaData = _mediaDataLoader->mediaData();
	} else {
		mediaData = _process->mediaData();
	}

	QMultiMap<QString, QString> metaDataMap;
	metaDataMap.insert(QLatin1String("ARTIST"), mediaData.clip_artist);
	metaDataMap.insert(QLatin1String("ALBUM"), mediaData.clip_album);
	metaDataMap.insert(QLatin1String("TITLE"), mediaData.clip_name);
	metaDataMap.insert(QLatin1String("DATE"), mediaData.clip_date);
	metaDataMap.insert(QLatin1String("GENRE"), mediaData.clip_genre);
	metaDataMap.insert(QLatin1String("TRACKNUMBER"), mediaData.clip_track);
	metaDataMap.insert(QLatin1String("DESCRIPTION"), mediaData.clip_comment);
	metaDataMap.insert(QLatin1String("COPYRIGHT"), mediaData.clip_copyright);
	metaDataMap.insert(QLatin1String("URL"), mediaData.stream_url);
	metaDataMap.insert(QLatin1String("ENCODEDBY"), mediaData.clip_software);

	//duration should be in milliseconds
	_totalTime = mediaData.duration * 1000;
	emit totalTimeChanged(_totalTime);

	emit hasVideoChanged(!mediaData.novideo);

	emit metaDataChanged(metaDataMap);

	if (_mediaDataLoader) {
		emit stateChanged(Phonon::StoppedState);
	}
}

void MPlayerMediaObject::play() {
	_playRequestReached = true;
	_process = mplayerLoader.startMPlayerProcess(_filename, (int) _videoWidgetId);

	connect(_process, SIGNAL(pause()),
		SLOT(pausedState()));
	connect(_process, SIGNAL(tick(double)),
		SLOT(tickInternal(double)));
	connect(_process, SIGNAL(endOfFile()),
		SLOT(endOfFile()));
	connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)),
		SLOT(finished(int, QProcess::ExitStatus)));
}

MPlayerProcess * MPlayerMediaObject::getMPlayerProcess() const {
	return _process;
}

void MPlayerMediaObject::setState(Phonon::State newState) {
	_currentState = newState;
	emit stateChanged(_currentState);
}

void MPlayerMediaObject::tickInternal(double seconds) {
	//We are now playing the file
	if (_currentState != Phonon::PlayingState) {
		setState(Phonon::PlayingState);
		mediaLoaded();
	}

	//time should be in milliseconds
	_currentTime = seconds * 1000;
	emit tick(_currentTime);
}

void MPlayerMediaObject::pause() {
	_process->writeToStdin("pause");
}

void MPlayerMediaObject::pausedState() {
	setState(Phonon::PausedState);
}

void MPlayerMediaObject::stop() {
	_process->writeToStdin("quit");
}

void MPlayerMediaObject::finished(int exitCode, QProcess::ExitStatus exitStatus) {
	switch (exitStatus) {
	case QProcess::NormalExit:
		qDebug() << "MPlayer process exited normally";
		break;
	case QProcess::CrashExit:
		qCritical() << __FUNCTION__ << "MPlayer process crashed";
		break;
	}

	setState(Phonon::StoppedState);
}

void MPlayerMediaObject::endOfFile() {
	setState(Phonon::StoppedState);
	emit finished();
}

void MPlayerMediaObject::seek(qint64 milliseconds) {
	static qint64 previousSeek = 0;

	//Only keep seek that are separated by 1000 milliseconds = 1 second
	//Otherwise MPlayer gets ugly since it is not fast enough
	if (previousSeek > (milliseconds + 1000) || previousSeek < (milliseconds - 1000)) {
		_process->writeToStdin("seek " + QString::number(milliseconds / 1000.0) + " 2");
		previousSeek = milliseconds;
	}
}

bool MPlayerMediaObject::hasVideo() const {
	return true;
}

bool MPlayerMediaObject::isSeekable() const {
	return true;
}

qint64 MPlayerMediaObject::currentTime() const {
	return _currentTime;
}

QString MPlayerMediaObject::errorString() const {
	return "";
}

qint64 MPlayerMediaObject::totalTime() const {
	return _totalTime;
}

}}	//Namespace Phonon::VLC_MPlayer
