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

//AudioChannel
void MPlayerMediaController::audioChannelAdded(int id, const QString & lang) {
	qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
	properties.insert("name", lang);
	properties.insert("description", "");

	_availableAudioChannels << Phonon::AudioChannelDescription(id, properties);
}

void MPlayerMediaController::setCurrentAudioChannel(const Phonon::AudioChannelDescription & audioChannel) {
	qDebug() << __FUNCTION__;

	_currentAudioChannel = audioChannel;
	_process->sendCommand("switch_audio " + QString::number(_currentAudioChannel.index()));
}

QList<Phonon::AudioChannelDescription> MPlayerMediaController::availableAudioChannels() const {
	return _availableAudioChannels;
}

Phonon::AudioChannelDescription MPlayerMediaController::currentAudioChannel() const {
	return _currentAudioChannel;
}

//Subtitle
void MPlayerMediaController::subtitleAdded(int id, const QString & lang, const QString & type) {
	qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
	properties.insert("name", lang);
	properties.insert("description", "");
	properties.insert("type", type);

	_availableSubtitles << Phonon::SubtitleDescription(id, properties);
}

void MPlayerMediaController::loadSubtitleFile(const QString & filename) {
	if (!filename.isEmpty()) {
		clearAllButTitle();

		//Loads the selected subtitle file
		QStringList args;
		args << "-sub";
		args << filename;

		MPlayerLoader::restart(_process, args);
	}
}

void MPlayerMediaController::setCurrentSubtitle(const Phonon::SubtitleDescription & subtitle) {
	qDebug() << __FUNCTION__;

	_currentSubtitle = subtitle;
	int id = _currentSubtitle.index();
	if (id == -1) {
		//sub_source [source]
		//  Display first subtitle from [source]. Here [source] is an integer:
		//  SUB_SOURCE_SUBS   (0) for file subs
		//  SUB_SOURCE_VOBSUB (1) for VOBsub files
		//  SUB_SOURCE_DEMUX  (2) for subtitle embedded in the media file or DVD subs.
		//  If [source] is -1, will turn off subtitle display. If [source] less than -1,
		//  will cycle between the first subtitle of each currently available sources.
		_process->sendCommand("sub_source -1");
	} else {
		QString type = _currentSubtitle.property("type").toString();
		if (type.compare("vob", Qt::CaseInsensitive)) {
			_process->sendCommand("sub_vob " + QString::number(id));
		}

		else if (type.compare("sub", Qt::CaseInsensitive)) {
			_process->sendCommand("sub_demux " + QString::number(id));
		}

		else if (type.compare("sid", Qt::CaseInsensitive)) {
			_process->sendCommand("sub_demux " + QString::number(id));
		}

		else if (type.compare("file", Qt::CaseInsensitive)) {
			QString filename = _currentSubtitle.property("name").toString();

			if (_availableSubtitles.contains(_currentSubtitle)) {
				//If already in the list of subtitles
				//then no need to load the subtitle and restart MPlayer
				_process->sendCommand("sub_file " + QString::number(id));
			} else {
				//This is a new subtitle file
				//We must load it and restart MPlayer
				loadSubtitleFile(filename);
			}
		}

		else {
			qCritical() << __FUNCTION__ << "Error: unknown subtitle type:" << type;
		}
	}
}

QList<Phonon::SubtitleDescription> MPlayerMediaController::availableSubtitles() const {
	return _availableSubtitles;
}

Phonon::SubtitleDescription MPlayerMediaController::currentSubtitle() const {
	return _currentSubtitle;
}

//Title
void MPlayerMediaController::titleAdded(int id, qint64 length) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	QHash<QByteArray, QVariant> properties;
	properties.insert("name", id);
	properties.insert("description", length);

	_availableTitles << Phonon::TitleDescription(id, properties);
#else
	if (_availableTitles < id) {
		_availableTitles = id;
	}

	qDebug() << __FUNCTION__ << "Titles: " << _availableTitles;
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

#ifdef NEW_TITLE_CHAPTER_HANDLING

void MPlayerMediaController::setCurrentTitle(const Phonon::TitleDescription & title) {
	_currentTitle = title;

	//DVDNAV only
	//otherwise needs to restart MPlayerProcess
	//with parameter: dvd://titleNumber
	//_process->sendCommand("switch_title " + QString::number(_currentTitle));

	clearAllButTitle();
	MPlayerLoader::restart(_process, QStringList(), "dvd://" + QString::number(_currentTitle.index()));
}

void MPlayerMediaController::clearAllButTitle() {
	_currentAngle = 0;
	_availableAngles = 0;

	//_currentChapter = 0;
	_availableChapters.clear();

	_availableAudioChannels.clear();

	_availableSubtitles.clear();
}

QList<Phonon::TitleDescription> MPlayerMediaController::availableTitles() const {
	return _availableTitles;
}

Phonon::TitleDescription MPlayerMediaController::currentTitle() const {
	return _currentTitle;
}

#else

void MPlayerMediaController::setCurrentTitle(int titleNumber) {
	_currentTitle = titleNumber;

	//DVDNAV only
	//otherwise needs to restart MPlayerProcess
	//with parameter: dvd://titleNumber
	//_process->sendCommand("switch_title " + QString::number(_currentTitle));

	clearAllButTitle();
	MPlayerLoader::restart(_process, QStringList(), "dvd://" + QString::number(_currentTitle));
}

void MPlayerMediaController::clearAllButTitle() {
	_currentAngle = 0;
	_availableAngles = 0;

	_currentChapter = 0;
	_availableChapters = 0;

	_availableAudioChannels.clear();

	_availableSubtitles.clear();
}

int MPlayerMediaController::availableTitles() const {
	return _availableTitles;
}

int MPlayerMediaController::currentTitle() const {
	return _currentTitle;
}

#endif	//NEW_TITLE_CHAPTER_HANDLING

void MPlayerMediaController::setAutoplayTitles(bool autoplay) {
	_autoplayTitles = autoplay;
}

bool MPlayerMediaController::autoplayTitles() const {
	return _autoplayTitles;
}

//Chapter

#ifdef NEW_TITLE_CHAPTER_HANDLING

void MPlayerMediaController::chapterAdded(int titleId, int chapters) {
	//DVD chapter added
	if (titleId == _currentTitle.index()) {
		_availableChapters.clear();
		for (int i = 0; i < chapters; i++) {
			QHash<QByteArray, QVariant> properties;
			properties.insert("name", i);
			properties.insert("description", "");

			_availableChapters << Phonon::ChapterDescription(i, properties);
		}

		qDebug() << __FUNCTION__ << "Chapters: " << _availableChapters.size();
	}
}

void MPlayerMediaController::mkvChapterAdded(int id, const QString & title, const QString & from, const QString & to) {
	//Matroska chapter added
	QHash<QByteArray, QVariant> properties;
	properties.insert("name", QString::number(id) + " " + title + " (" + from + ")");
	properties.insert("description", "");

	_availableChapters << Phonon::ChapterDescription(id, properties);
	qDebug() << __FUNCTION__ << "Chapter id: " << id << "title:" << title;
}

void MPlayerMediaController::setCurrentChapter(const Phonon::ChapterDescription & chapter) {
	qDebug() << __FUNCTION__;

	_currentChapter = chapter;
	_process->sendCommand("seek_chapter " + QString::number(_currentChapter.index()) + " 1");
}

QList<Phonon::ChapterDescription> MPlayerMediaController::availableChapters() const {
	return _availableChapters;
}

Phonon::ChapterDescription MPlayerMediaController::currentChapter() const {
	return _currentChapter;
}

#else

void MPlayerMediaController::chapterAdded(int titleId, int chapters) {
	//DVD chapter added
	if (titleId == _currentTitle) {
		_availableChapters = chapters;
		qDebug() << __FUNCTION__ << "Chapters: " << _availableChapters;
	}
}

void MPlayerMediaController::mkvChapterAdded(int id, const QString & title, const QString & from, const QString & to) {
	//Matroska chapter added
	if (_availableChapters < id) {
		_availableChapters = id;
		qDebug() << __FUNCTION__ << "Chapter id: " << _availableChapters << "title:" << title;
	}
}

void MPlayerMediaController::setCurrentChapter(int chapterNumber) {
	qDebug() << __FUNCTION__;

	_currentChapter = chapterNumber;
	_process->sendCommand("seek_chapter " + QString::number(_currentChapter) + " 1");
}

int MPlayerMediaController::availableChapters() const {
	return _availableChapters;
}

int MPlayerMediaController::currentChapter() const {
	return _currentChapter;
}

#endif	//NEW_TITLE_CHAPTER_HANDLING

//Angle
void MPlayerMediaController::angleAdded(int titleId, int angles) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	if (titleId == _currentTitle.index()) {
#else
	if (titleId == _currentTitle) {
#endif	//NEW_TITLE_CHAPTER_HANDLING
		_availableAngles = angles;
		qDebug() << __FUNCTION__ << "Angles: " << _availableAngles;
	}
}

void MPlayerMediaController::setCurrentAngle(int angleNumber) {
	qDebug() << __FUNCTION__;

	_currentAngle = angleNumber;
	_process->sendCommand("switch_angle " + QString::number(_currentAngle));
}

int MPlayerMediaController::availableAngles() const {
	return _availableAngles;
}

int MPlayerMediaController::currentAngle() const {
	return _currentAngle;
}

}}	//Namespace Phonon::VLC_MPlayer