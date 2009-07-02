/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MediaInfo.h"

#include <tkutil/TkTime.h>

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

MediaInfo::MediaInfo() {
	clear();
}

MediaInfo::MediaInfo(const QString & filename) {
	clear();

	setFileName(filename);
}

MediaInfo::~MediaInfo() {
}

void MediaInfo::clear() {
	_fetched = false;

	//General
	_fileName.clear();
	_isUrl = false;
	//_fileType
	_fileSize = -1;
	_length = -1;
	_bitrate = -1;
	_encodedApplication.clear();
	_cueStartIndex = CUE_INDEX_INVALID;
	_cueEndIndex = CUE_INDEX_INVALID;
	_privateData.clear();

	//metaData
	_metadataHash.clear();

	//Audio
	_audioStreamCount = 0;
	_audioStreamHash.clear();

	//Video
	_videoStreamCount = 0;
	_videoStreamHash.clear();

	//Text
	_textStreamCount = 0;
	_textStreamHash.clear();

	//Stream
	_networkStreamHash.clear();
}

bool MediaInfo::fetched() const {
	return _fetched;
}

void MediaInfo::setFetched(bool fetched) {
	_fetched = fetched;
}

QString MediaInfo::fileName() const {
	return _fileName;
}

void MediaInfo::setFileName(const QString & filename) {
	_fileName = filename.trimmed();

	//This avoid a stupid bug: comparing a filename with \ separator and another with /
	//By replacing any \ by /, we don't have any comparison problem
	//This is needed by FileSearchModel.cpp
	//and PlaylistModel::updateMediaInfo()
	//_fileName.replace("\\", "/");
}

bool MediaInfo::isUrl() const {
	return _isUrl;
}

void MediaInfo::setUrl(bool url) {
	_isUrl = url;
}

bool MediaInfo::isUrl(const QString & filename) {
	//A filename that contains a host/server name is a remote/network media
	return !QUrl(filename).host().isEmpty();
}

FileType MediaInfo::fileType() const {
	return _fileType;
}

void MediaInfo::setFileType(FileType fileType) {
	_fileType = fileType;
}

QString MediaInfo::fileSize() const {
	if (_fileSize > 0) {
		return QString::number(_fileSize / 1024 / 1024.0);
	} else {
		return QString();
	}
}

void MediaInfo::setFileSize(int fileSize) {
	_fileSize = fileSize;
}

QString MediaInfo::lengthFormatted() const {
	if (_length > 0) {
		return TkTime::convertSeconds(_length);
	} else {
		return QString();
	}
}

int MediaInfo::lengthSeconds() const {
	return _length;
}

int MediaInfo::lengthMilliseconds() const {
	return (_length * 1000);
}

void MediaInfo::setLength(int length) {
	_length = length;
}

qint64 MediaInfo::parseCueIndexString(const QString & cueIndexString) {
	//Format: [mm:ss:ff]
	//mm:ss:ff - Starting time in minutes, seconds, and frames (75 frames/second)
	//Example:
	//00:00:00
	//02:34:50

	qint64 cueIndex = CUE_INDEX_INVALID;
	QStringList tmp(cueIndexString.split(':', QString::SkipEmptyParts));
	if (tmp.size() == 3) {
		int minutes = tmp[0].toInt();
		int seconds = tmp[1].toInt();
		int frames = tmp[2].toInt();
		cueIndex = (minutes * 60 * 1000) + (seconds * 1000) + ((frames / 75.0) * 1000);
		//There is an approximation since _cueIndex is a qint64 and not a double
	} else {
		qCritical() << __FUNCTION__ << "Error: incorrect CUE sheet index string:" << cueIndex;
	}
	return cueIndex;
}

void MediaInfo::setCueStartIndex(const QString & cueIndex) {
	_cueStartIndex = parseCueIndexString(cueIndex);
}

void MediaInfo::setCueEndIndex(const QString & cueIndex) {
	_cueEndIndex = parseCueIndexString(cueIndex);
}

qint64 MediaInfo::cueStartIndex() const {
	return _cueStartIndex;
}

qint64 MediaInfo::cueEndIndex() const {
	return _cueEndIndex;
}

QString MediaInfo::cueIndexFormatted(qint64 cueIndex) {
	//If cueIndex is invalid, then it will return an empty string
	QString str;
	if (cueIndex != CUE_INDEX_INVALID) {
		int minutes = (cueIndex / 60 / 1000) % 60;
		int seconds = (cueIndex / 1000) % 60;
		int frames = ((cueIndex * 75) / 1000) % 75;

		int fieldWidth = 2;
		int base = 10;
		QLatin1Char fillChar('0');
		str = QString("%1:%2:%3")
			.arg(minutes, fieldWidth, base, fillChar)
			.arg(seconds, fieldWidth, base, fillChar)
			.arg(frames, fieldWidth, base, fillChar);
	}
	return str;
}

QString MediaInfo::cueStartIndexFormatted() const {
	return cueIndexFormatted(_cueStartIndex);
}

QString MediaInfo::cueEndIndexFormatted() const {
	return cueIndexFormatted(_cueEndIndex);
}

QString MediaInfo::bitrate() const {
	if (_bitrate > 0) {
		return QString::number(_bitrate);
	} else {
		return QString();
	}
}

void MediaInfo::setBitrate(int bitrate) {
	_bitrate = bitrate;
}

QString MediaInfo::encodedApplication() const {
	return _encodedApplication;
}

void MediaInfo::setEncodedApplication(const QString & encodedApplication) {
	_encodedApplication = encodedApplication.trimmed();
}

//Metadata
QString MediaInfo::metadataValue(Metadata metadata) const {
	return _metadataHash.value(metadata);
}

void MediaInfo::insertMetadata(Metadata metadata, const QString & value) {
	_metadataHash.insert(metadata, value.trimmed());
}

//Audio
int MediaInfo::audioStreamCount() const {
	return _audioStreamCount;
}

QString MediaInfo::audioStreamValue(int audioStreamId, AudioStream audioStream) const {
	//Constructs the key
	//Example: audioStreamId = 1, audioStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(audioStreamId) + QString::number(audioStream)).toInt();

	return _audioStreamHash.value(key);
}

void MediaInfo::insertAudioStream(int audioStreamId, AudioStream audioStream, const QString & value) {
	if (audioStreamId + 1> _audioStreamCount) {
		_audioStreamCount = audioStreamId + 1;
	}

	//Constructs the key
	//Example: audioStreamId = 1, audioStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(audioStreamId) + QString::number(audioStream)).toInt();

	_audioStreamHash.insert(key, value.trimmed());
}

//Video
int MediaInfo::videoStreamCount() const {
	return _videoStreamCount;
}

QString MediaInfo::videoStreamValue(int videoStreamId, VideoStream videoStream) const {
	//Constructs the key
	//Example: videoStreamId = 1, videoStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(videoStreamId) + QString::number(videoStream)).toInt();

	return _videoStreamHash.value(key);
}

void MediaInfo::insertVideoStream(int videoStreamId, VideoStream videoStream, const QString & value) {
	if (videoStreamId + 1> _videoStreamCount) {
		_videoStreamCount = videoStreamId + 1;
	}

	//Constructs the key
	//Example: videoStreamId = 1, videoStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(videoStreamId) + QString::number(videoStream)).toInt();

	_videoStreamHash.insert(key, value.trimmed());
}

//Text
int MediaInfo::textStreamCount() const {
	return _textStreamCount;
}

QString MediaInfo::textStreamValue(int textStreamId, TextStream textStream) const {
	//Constructs the key
	//Example: textStreamId = 1, textStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(textStreamId) + QString::number(textStream)).toInt();

	return _textStreamHash.value(key);
}

void MediaInfo::insertTextStream(int textStreamId, TextStream textStream, const QString & value) {
	if (textStreamId + 1> _textStreamCount) {
		_textStreamCount = textStreamId + 1;
	}

	//Constructs the key
	//Example: textStreamId = 1, textStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(textStreamId) + QString::number(textStream)).toInt();

	_textStreamHash.insert(key, value.trimmed());
}

//Network stream
QString MediaInfo::networkStreamValue(NetworkStream networkStream) const {
	return _networkStreamHash.value(networkStream);
}

void MediaInfo::insertNetworkStream(NetworkStream networkStream, const QString & value) {
	_networkStreamHash.insert(networkStream, value.trimmed());
}

void MediaInfo::setPrivateData(const QString & privateData) {
	_privateData = privateData;
}

QString MediaInfo::privateData() const {
	return _privateData;
}
