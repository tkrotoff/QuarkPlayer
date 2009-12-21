/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <TkUtil/TkTime.h>

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

MediaInfo::MediaInfo() {
	qRegisterMetaType<MediaInfo>("MediaInfo");

	clear();
}

MediaInfo::MediaInfo(const QString & filename) {
	qRegisterMetaType<MediaInfo>("MediaInfo");

	clear();

	setFileName(filename);
}

MediaInfo::~MediaInfo() {
}

bool MediaInfo::operator==(const MediaInfo & mediaInfo) const {
	bool equal = false;

	equal |= _fetched == mediaInfo._fetched;
	equal |= _fileName == mediaInfo._fileName;
	//equal |= _fileType == mediaInfo._fileType;
	equal |= _fileSize == mediaInfo._fileSize;

	equal |= _duration == mediaInfo._duration;

	equal |= _bitrate == mediaInfo._bitrate;
	equal |= _encodedApplication == mediaInfo._encodedApplication;

	equal |= _cueStartIndex == mediaInfo._cueStartIndex;
	equal |= _cueEndIndex == mediaInfo._cueEndIndex;

	equal |= _metaDataHash == mediaInfo._metaDataHash;

	equal |= _audioStreamCount == mediaInfo._audioStreamCount;
	equal |= _audioStreamHash == mediaInfo._audioStreamHash;

	equal |= _videoStreamCount == mediaInfo._videoStreamCount;
	equal |= _videoStreamHash == mediaInfo._videoStreamHash;

	equal |= _textStreamCount == mediaInfo._textStreamCount;
	equal |= _textStreamHash == mediaInfo._textStreamHash;

	equal |= _networkStreamHash == mediaInfo._networkStreamHash;

	equal |= _extendedMetaData == mediaInfo._extendedMetaData;

	return equal;
}

void MediaInfo::clear() {
	_fetched = false;

	//General
	_fileName.clear();
	//_fileType
	_fileSize = -1;
	_duration = -1;
	_bitrate = -1;
	_encodedApplication.clear();
	_cueStartIndex = CUE_INDEX_INVALID;
	_cueEndIndex = CUE_INDEX_INVALID;

	//metaData
	_metaDataHash.clear();

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

	_extendedMetaData.clear();
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

int MediaInfo::fileSize() const {
	if (_fileSize > 0) {
		return _fileSize / 1024 / 1024.0;
	} else {
		//_fileSize should be -1
		return _fileSize;
	}
}

void MediaInfo::setFileSize(int kilobytes) {
	_fileSize = kilobytes;
}

QString MediaInfo::durationFormatted() const {
	if (_duration > 0) {
		return TkTime::convertMilliseconds(_duration);
	} else {
		return QString();
	}
}

qint64 MediaInfo::durationSecs() const {
	return _duration / 1000.0;
}

qint64 MediaInfo::durationMSecs() const {
	return _duration;
}

void MediaInfo::setDurationSecs(qint64 seconds) {
	_duration = 1000 * seconds;
}

void MediaInfo::setDurationMSecs(qint64 milliseconds) {
	_duration = milliseconds;
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

int MediaInfo::bitrate() const {
	return _bitrate;
}

void MediaInfo::setBitrate(int kbps) {
	_bitrate = kbps;
}

QString MediaInfo::encodedApplication() const {
	return _encodedApplication;
}

void MediaInfo::setEncodedApplication(const QString & encodedApplication) {
	_encodedApplication = encodedApplication.trimmed();
}

//MetaData
QVariant MediaInfo::metaDataValue(MetaData metaData) const {
	return _metaDataHash.value(metaData);
}

void MediaInfo::insertMetaData(MetaData metaData, const QVariant & value) {
	_metaDataHash.insert(metaData, value);
}

//Audio
int MediaInfo::audioStreamCount() const {
	return _audioStreamCount;
}

QVariant MediaInfo::audioStreamValue(int audioStreamId, AudioStream audioStream) const {
	//Constructs the key
	//Example: audioStreamId = 1, audioStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(audioStreamId) + QString::number(audioStream)).toInt();

	return _audioStreamHash.value(key);
}

void MediaInfo::insertAudioStream(int audioStreamId, AudioStream audioStream, const QVariant & value) {
	if (audioStreamId + 1> _audioStreamCount) {
		_audioStreamCount = audioStreamId + 1;
	}

	//Constructs the key
	//Example: audioStreamId = 1, audioStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(audioStreamId) + QString::number(audioStream)).toInt();

	_audioStreamHash.insert(key, value);
}

//Video
int MediaInfo::videoStreamCount() const {
	return _videoStreamCount;
}

QVariant MediaInfo::videoStreamValue(int videoStreamId, VideoStream videoStream) const {
	//Constructs the key
	//Example: videoStreamId = 1, videoStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(videoStreamId) + QString::number(videoStream)).toInt();

	return _videoStreamHash.value(key);
}

void MediaInfo::insertVideoStream(int videoStreamId, VideoStream videoStream, const QVariant & value) {
	if (videoStreamId + 1> _videoStreamCount) {
		_videoStreamCount = videoStreamId + 1;
	}

	//Constructs the key
	//Example: videoStreamId = 1, videoStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(videoStreamId) + QString::number(videoStream)).toInt();

	_videoStreamHash.insert(key, value);
}

//Text
int MediaInfo::textStreamCount() const {
	return _textStreamCount;
}

QVariant MediaInfo::textStreamValue(int textStreamId, TextStream textStream) const {
	//Constructs the key
	//Example: textStreamId = 1, textStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(textStreamId) + QString::number(textStream)).toInt();

	return _textStreamHash.value(key);
}

void MediaInfo::insertTextStream(int textStreamId, TextStream textStream, const QVariant & value) {
	if (textStreamId + 1> _textStreamCount) {
		_textStreamCount = textStreamId + 1;
	}

	//Constructs the key
	//Example: textStreamId = 1, textStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(textStreamId) + QString::number(textStream)).toInt();

	_textStreamHash.insert(key, value);
}

//Network stream
QVariant MediaInfo::networkStreamValue(NetworkStream networkStream) const {
	return _networkStreamHash.value(networkStream);
}

void MediaInfo::insertNetworkStream(NetworkStream networkStream, const QVariant & value) {
	_networkStreamHash.insert(networkStream, value);
}

//Extended metadata
void MediaInfo::setExtendedMetaData(const QString & key, const QVariant & value) {
	_extendedMetaData.insert(key, value);
}

QVariant MediaInfo::extendedMetaData(const QString & key) const {
	return _extendedMetaData.value(key);
}
