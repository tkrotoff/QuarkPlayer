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

#include "VLCMediaController.h"

#include "vlc_loader.h"
#include "vlc_symbols.h"

namespace Phonon
{
namespace VLC_MPlayer
{

VLCMediaController::VLCMediaController()
	: MediaController() {

	_vlcMediaPlayer = NULL;
}

VLCMediaController::~VLCMediaController() {
}

//AudioChannel
void VLCMediaController::setCurrentAudioChannel(const Phonon::AudioChannelDescription & audioChannel) {
	qDebug() << __FUNCTION__;

	_currentAudioChannel = audioChannel;
}

QList<Phonon::AudioChannelDescription> VLCMediaController::availableAudioChannels() const {
	return _availableAudioChannels;
}

Phonon::AudioChannelDescription VLCMediaController::currentAudioChannel() const {
	return _currentAudioChannel;
}

//Subtitle
void VLCMediaController::setCurrentSubtitle(const Phonon::SubtitleDescription & subtitle) {
	qDebug() << __FUNCTION__;

	_currentSubtitle = subtitle;
	int id = _currentSubtitle.index();
	QString type = _currentSubtitle.property("type").toString();

	if (type == "file") {
		QString filename = _currentSubtitle.property("name").toString();
		if (!filename.isEmpty()) {
			p_libvlc_video_set_subtitle(_vlcMediaPlayer, filename.toAscii().data(), _vlcException);
		}
	}
}

QList<Phonon::SubtitleDescription> VLCMediaController::availableSubtitles() const {
	return _availableSubtitles;
}

Phonon::SubtitleDescription VLCMediaController::currentSubtitle() const {
	return _currentSubtitle;
}

//Title
void VLCMediaController::setCurrentTitle(int titleNumber) {
	_currentTitle = titleNumber;
}

void VLCMediaController::setAutoplayTitles(bool autoplay) {
	_autoplayTitles = autoplay;
}

int VLCMediaController::availableTitles() const {
	return _availableTitles;
}

int VLCMediaController::currentTitle() const {
	return _currentTitle;
}

bool VLCMediaController::autoplayTitles() const {
	return _autoplayTitles;
}

void VLCMediaController::nextTitle() {
	setCurrentTitle(_currentTitle++);
}

void VLCMediaController::previousTitle() {
	setCurrentTitle(_currentTitle--);
}

//Chapter
void VLCMediaController::setCurrentChapter(int chapterNumber) {
}

int VLCMediaController::availableChapters() const {
	return _availableChapters;
}

int VLCMediaController::currentChapter() const {
	return _currentChapter;
}

//Angle
void VLCMediaController::setCurrentAngle(int angleNumber) {
}

int VLCMediaController::availableAngles() const {
	return _availableAngles;
}

int VLCMediaController::currentAngle() const {
	return _currentAngle;
}

}}	//Namespace Phonon::VLC_MPlayer
