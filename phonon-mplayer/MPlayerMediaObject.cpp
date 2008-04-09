/*
 * MPlayer backend for the Phonon library
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

#include "VideoWidget.h"

#include <mplayer/MPlayerProcess.h>
#include <mplayer/MPlayerLoader.h>

namespace Phonon
{
namespace MPlayer
{

MPlayerMediaObject::MPlayerMediaObject(QObject * parent)
	: QObject(parent) {

	_currentState = Phonon::LoadingState;
	_process = NULL;

	_currentTime = 0;
	_totalTime = 0;
}

MPlayerMediaObject::~MPlayerMediaObject() {
}

void MPlayerMediaObject::loadMedia(const QString & filename) {
	if (_filename == filename) {
		//Already loaded
		return;
	}

	_filename = filename;

	_process = new MPlayerProcess(this);
	_process->clearArguments();
	_process->addArgument(MPLAYER_EXE);
	_process->addArgument("-identify");
	_process->addArgument("-frames");
	_process->addArgument("0");
	_process->addArgument(_filename);

	_process->start();
	_process->waitForFinished();

	//Default MediaObject state is Phonon::LoadingState
	_currentState = Phonon::LoadingState;

	MediaData mediaData = _process->mediaData();

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

	emit metaDataChanged(metaDataMap);

	//duration should be in milliseconds
	_totalTime = mediaData.duration * 1000; 
	emit totalTimeChanged(_totalTime);

	emit hasVideoChanged(!mediaData.novideo);
}

void MPlayerMediaObject::tickInternal(double seconds) {
	//We are now playing the file
	if (_currentState != Phonon::PlayingState) {
		setState(Phonon::PlayingState);
	}

	//time should be in milliseconds
	_currentTime = seconds * 1000;
	emit tick(_currentTime);
}

void MPlayerMediaObject::setState(Phonon::State newState) {
	_currentState = newState;
	emit stateChanged(_currentState);
}

void MPlayerMediaObject::play() {
	MPlayerLoader * loader = new MPlayerLoader(_process, this);

	connect(_process, SIGNAL(receivedPause()),
		SLOT(stateChangedPause()));
	connect(_process, SIGNAL(receivedCurrentSec(double)),
		SLOT(tickInternal(double)));
	connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)),
		SLOT(stateChangedStop(int, QProcess::ExitStatus)));

	loader->startMPlayerProcess(_filename, (int) VideoWidget::_videoWidgetId);
}

void MPlayerMediaObject::stateChangedPlay() {
	setState(Phonon::PlayingState);
}

void MPlayerMediaObject::pause() {
	_process->writeToStdin("pause");
}

void MPlayerMediaObject::stateChangedPause() {
	setState(Phonon::PausedState);
}

void MPlayerMediaObject::stop() {
	_process->writeToStdin("quit");
}

void MPlayerMediaObject::stateChangedStop(int exitCode, QProcess::ExitStatus exitStatus) {
	switch (exitStatus) {
	case QProcess::NormalExit:
		qDebug() << "MPlayer process exited normally";
		break;
	case QProcess::CrashExit:
		qCritical() << __FUNCTION__ << "MPlayer process crashed";
		break;
	}

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

Phonon::State MPlayerMediaObject::state() const {
	return _currentState;
}

QString MPlayerMediaObject::errorString() const {
	return "";
}

qint64 MPlayerMediaObject::totalTime() const {
	return _totalTime;
}

}}	//Namespace Phonon::MPlayer
