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

#include "MPlayerMediaController.h"

#include <mplayer/MPlayerLoader.h>

namespace Phonon
{
namespace VLC_MPlayer
{

MPlayerMediaController::MPlayerMediaController()
	: MediaController() {
}

MPlayerMediaController::~MPlayerMediaController() {
}

//AudioStream
void MPlayerMediaController::audioStreamAdded(int id, const QString & lang) {
	qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
	properties.insert("name", lang);
	properties.insert("description", lang);

	_availableAudioStreams << Phonon::AudioStreamDescription(id, properties);
}

void MPlayerMediaController::setCurrentAudioStream(const Phonon::AudioStreamDescription & stream) {
	qDebug() << __FUNCTION__;

	_currentAudioStream = stream;
	_process->writeToStdin("switch_audio " + QString::number(_currentAudioStream.index()));
}

QList<Phonon::AudioStreamDescription> MPlayerMediaController::availableAudioStreams() const {
	return _availableAudioStreams;
}

Phonon::AudioStreamDescription MPlayerMediaController::currentAudioStream() const {
	return _currentAudioStream;
}

//SubtitleStream
void MPlayerMediaController::subtitleStreamAdded(int id, const QString & lang, const QString & type) {
	qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
	properties.insert("name", lang);
	properties.insert("description", lang);
	properties.insert("type", type);

	_availableSubtitleStreams << Phonon::SubtitleStreamDescription(id, properties);
}

void MPlayerMediaController::setCurrentSubtitleStream(const Phonon::SubtitleStreamDescription & stream) {
	qDebug() << __FUNCTION__;

	_currentSubtitleStream = stream;
	_process->writeToStdin("sub_demux " + QString::number(_currentSubtitleStream.index()));
}

QList<Phonon::SubtitleStreamDescription> MPlayerMediaController::availableSubtitleStreams() const {
	return _availableSubtitleStreams;
}

Phonon::SubtitleStreamDescription MPlayerMediaController::currentSubtitleStream() const {
	return _currentSubtitleStream;
}

//Title
void MPlayerMediaController::titleAdded(int id, qint64 length) {
	if (_availableTitles < id) {
		_availableTitles = id;
	}

	qDebug() << __FUNCTION__ << "Titles: " << _availableTitles;
}

void MPlayerMediaController::setCurrentTitle(int titleNumber) {
	_currentTitle = titleNumber;

	//DVDNAV only
	//otherwise needs to restart MPlayerProcess
	//with parameter: dvd://titleNumber
	//_process->writeToStdin("switch_title " + QString::number(_currentTitle));

	MPlayerLoader::restart(_process, "dvd://" + QString::number(_currentTitle));

	clearAllButTitle();
}

void MPlayerMediaController::clearAllButTitle() {
	_currentAngle = 1;
	_availableAngles = 1;

	_currentChapter = 1;
	_availableChapters = 1;

	_availableAudioStreams.clear();

	_availableSubtitleStreams.clear();
}

void MPlayerMediaController::setAutoplayTitles(bool autoplay) {
	_autoplayTitles = autoplay;
}

int MPlayerMediaController::availableTitles() const {
	return _availableTitles;
}

int MPlayerMediaController::currentTitle() const {
	return _currentTitle;
}

bool MPlayerMediaController::autoplayTitles() const {
	return _autoplayTitles;
}

void MPlayerMediaController::nextTitle() {
	setCurrentTitle(_currentTitle++);
}

void MPlayerMediaController::previousTitle() {
	setCurrentTitle(_currentTitle--);
}

//Chapter
void MPlayerMediaController::chapterAdded(int titleId, int chapters) {
	if (titleId == _currentTitle) {
		_availableChapters = chapters;
		qDebug() << __FUNCTION__ << "Chapters: " << _availableChapters;
	}
}

void MPlayerMediaController::setCurrentChapter(int chapterNumber) {
	qDebug() << __FUNCTION__;

	_currentChapter = chapterNumber;
	_process->writeToStdin("seek_chapter " + QString::number(_currentChapter) +" 1");
}

int MPlayerMediaController::availableChapters() const {
	return _availableChapters;
}

int MPlayerMediaController::currentChapter() const {
	return _currentChapter;
}

//Angle
void MPlayerMediaController::angleAdded(int titleId, int angles) {
	if (titleId == _currentTitle) {
		_availableAngles = angles;
		qDebug() << __FUNCTION__ << "Angles: " << _availableAngles;
	}
}

void MPlayerMediaController::setCurrentAngle(int angleNumber) {
	qDebug() << __FUNCTION__;

	_currentAngle = angleNumber;
	_process->writeToStdin("switch_angle " + QString::number(_currentAngle));
}

int MPlayerMediaController::availableAngles() const {
	return _availableAngles;
}

int MPlayerMediaController::currentAngle() const {
	return _currentAngle;
}

}}	//Namespace Phonon::VLC_MPlayer
