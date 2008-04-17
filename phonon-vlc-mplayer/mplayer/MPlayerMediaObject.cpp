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

MPlayerMediaObject::MPlayerMediaObject(QObject * parent)
	: QObject(parent) {

	//We could have done some 'lazy initizalition' here: only create a MPlayerProcess
	//when starting to read the file.
	//But we create a MPlayerProcess now so other classes like VideoWidget
	//can connect to the MPlayerProcess signals
	//1 MediaObject = 1 MPlayerMediaObject = 1 MPlayerProcess
	_process = MPlayerLoader::createNewMPlayerProcess(this);

	connect(_process, SIGNAL(stateChanged(MPlayerProcess::State)),
		SLOT(stateChanged(MPlayerProcess::State)));

	connect(_process, SIGNAL(tick(qint64)),
		SIGNAL(tick(qint64)));

	connect(_process, SIGNAL(totalTimeChanged(qint64)),
		SIGNAL(totalTimeChanged(qint64)));

	connect(_process, SIGNAL(hasVideoChanged(bool)),
		SIGNAL(hasVideoChanged(bool)));

	connect(_process, SIGNAL(seekableChanged(bool)),
		SIGNAL(seekableChanged(bool)));

	connect(_process, SIGNAL(mediaDataChanged()),
		SLOT(mediaDataChanged()));

	connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)),
		SLOT(finished(int, QProcess::ExitStatus)));

	_videoWidgetId = 0;
}

MPlayerMediaObject::~MPlayerMediaObject() {
	stop();
}

void MPlayerMediaObject::setVideoWidgetId(int videoWidgetId) {
	_videoWidgetId = videoWidgetId;
}

void MPlayerMediaObject::loadMedia(const QString & filename) {
	/*if (filename == _filename) {
		//File already loaded
		return;
	}*/

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

	MPlayerLoader::loadMedia(_process, _filename);
}

void MPlayerMediaObject::mediaDataChanged() {
	MediaData mediaData = _process->getMediaData();

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

	//Other infos
	metaDataMap.insert(QLatin1String("DEMUXER"), mediaData.demuxer);
	if (mediaData.hasVideo) {
		metaDataMap.insert(QLatin1String("VIDEO_FORMAT"), mediaData.videoFormat);
		metaDataMap.insert(QLatin1String("VIDEO_BITRATE"), QString::number(mediaData.videoBitrate));
		metaDataMap.insert(QLatin1String("VIDEO_WIDTH"), QString::number(mediaData.videoWidth));
		metaDataMap.insert(QLatin1String("VIDEO_HEIGHT"), QString::number(mediaData.videoHeight));
		metaDataMap.insert(QLatin1String("VIDEO_FPS"), QString::number(mediaData.videoFPS));
		metaDataMap.insert(QLatin1String("VIDEO_ASPECT_RATIO"), QString::number(mediaData.videoAspectRatio));
		metaDataMap.insert(QLatin1String("AUDIO_FORMAT"), mediaData.audioFormat);
		metaDataMap.insert(QLatin1String("AUDIO_BITRATE"), QString::number(mediaData.audioBitrate));
		metaDataMap.insert(QLatin1String("AUDIO_RATE"), QString::number(mediaData.audioRate));
		metaDataMap.insert(QLatin1String("AUDIO_NCH"), QString::number(mediaData.audioNbChannels));
		metaDataMap.insert(QLatin1String("LENGTH"), QString::number(mediaData.totalTime));
		metaDataMap.insert(QLatin1String("VIDEO_CODEC"), mediaData.videoCodec);
		metaDataMap.insert(QLatin1String("AUDIO_CODEC"), mediaData.audioCodec);
	} else {
		//Because of the mediaplayer example, see MediaPlayer::updateInfo()
		metaDataMap.insert(QLatin1String("BITRATE"), QString::number(mediaData.audioBitrate));
	}

	emit metaDataChanged(metaDataMap);
}

void MPlayerMediaObject::play() {
	_playRequestReached = true;

	MPlayerLoader::start(_process, _filename, (int) _videoWidgetId);
}

MPlayerProcess * MPlayerMediaObject::getMPlayerProcess() const {
	return _process;
}

void MPlayerMediaObject::setState(Phonon::State newState) {
	emit stateChanged(newState);
}

void MPlayerMediaObject::pause() {
	if (_process) {
		_process->writeToStdin("pause");
	} else {
		qWarning() << __FUNCTION__ << "Error: _process is NULL";
	}
}

void MPlayerMediaObject::stateChanged(MPlayerProcess::State state) {
	switch (state) {
	case MPlayerProcess::LoadingState:
		qDebug() << __FUNCTION__ << "LoadingState";
		setState(Phonon::LoadingState);
		break;
	case MPlayerProcess::PlayingState:
		qDebug() << __FUNCTION__ << "PlayingState";
		setState(Phonon::PlayingState);
		break;
	case MPlayerProcess::BufferingState:
		qDebug() << __FUNCTION__ << "BufferingState";
		setState(Phonon::BufferingState);
		break;
	case MPlayerProcess::PausedState:
		qDebug() << __FUNCTION__ << "PausedState";
		setState(Phonon::PausedState);
		break;
	case MPlayerProcess::EndOfFileState:
		qDebug() << __FUNCTION__ << "EndOfFileState";
		setState(Phonon::StoppedState);
		emit finished();
		break;
	case MPlayerProcess::ErrorState:
		qDebug() << __FUNCTION__ << "ErrorState";
		break;
	default:
		qDebug() << __FUNCTION__ << "Error: unknown state:" << state;
	}
}

void MPlayerMediaObject::stop() {
	if (_process) {
		_process->stop();
	} else {
		qWarning() << __FUNCTION__ << "Error: _process is NULL";
	}
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

	qDebug() << __FUNCTION__ << "StoppedState";
	setState(Phonon::StoppedState);
}

void MPlayerMediaObject::seek(qint64 milliseconds) {
	if (_process) {
		_process->writeToStdin("seek " + QString::number(milliseconds / 1000.0) + " 2");
	} else {
		qWarning() << __FUNCTION__ << "Error: _process is NULL";
	}
}

bool MPlayerMediaObject::hasVideo() const {
	if (_process) {
		return _process->hasVideo();
	} else {
		return false;
	}
}

bool MPlayerMediaObject::isSeekable() const {
	if (_process) {
		return _process->isSeekable();
	} else {
		return false;
	}
}

qint64 MPlayerMediaObject::currentTime() const {
	if (_process) {
		return _process->currentTime();
	} else {
		return 0;
	}
}

QString MPlayerMediaObject::errorString() const {
	return "";
}

qint64 MPlayerMediaObject::totalTime() const {
	if (_process) {
		return _process->totalTime();
	} else {
		return 0;
	}
}

}}	//Namespace Phonon::VLC_MPlayer
