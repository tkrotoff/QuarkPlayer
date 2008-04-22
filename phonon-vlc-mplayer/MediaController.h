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

#ifndef PHONON_VLC_MPLAYER_MEDIACONTROLLER_H
#define PHONON_VLC_MPLAYER_MEDIACONTROLLER_H

#include <phonon/addoninterface.h>
#include <phonon/objectdescription.h>

namespace Phonon
{
namespace VLC_MPlayer
{

/**
 *
 *
 * @author Tanguy Krotoff
 */
class MediaController : public AddonInterface {
public:

	MediaController();
	virtual ~MediaController();

	bool hasInterface(Interface iface) const;

	QVariant interfaceCall(Interface iface, int command, const QList<QVariant> & arguments = QList<QVariant>());

	//MediaController signals
	virtual void availableSubtitlesChanged() = 0;
	virtual void availableAudioChannelsChanged() = 0;
	virtual void availableAnglesChanged(int availableAngles) = 0;
	virtual void availableChaptersChanged(int availableChapters) = 0;
	virtual void availableTitlesChanged(int availableTitles) = 0;
	virtual void angleChanged(int angleNumber) = 0;
	virtual void chapterChanged(int chapterNumber) = 0;
	virtual void titleChanged(int titleNumber) = 0;

protected:

	//AudioStream
	virtual void setCurrentAudioStream(const Phonon::AudioStreamDescription & stream) = 0;
	virtual QList<Phonon::AudioStreamDescription> availableAudioStreams() const = 0;
	virtual Phonon::AudioStreamDescription currentAudioStream() const = 0;

	//SubtitleStream
	virtual void setCurrentSubtitleStream(const Phonon::SubtitleStreamDescription & stream) = 0;
	virtual QList<Phonon::SubtitleStreamDescription> availableSubtitleStreams() const = 0;
	virtual Phonon::SubtitleStreamDescription currentSubtitleStream() const = 0;

	//Angle
	virtual void setCurrentAngle(int angleNumber) = 0;
	virtual int availableAngles() const = 0;
	virtual int currentAngle() const = 0;

	//Chapter
	virtual void setCurrentChapter(int chapterNumber) = 0;
	virtual int availableChapters() const = 0;
	virtual int currentChapter() const = 0;

	//Title
	virtual void setCurrentTitle(int titleNumber) = 0;
	virtual void setAutoplayTitles(bool autoplay) = 0;
	virtual int availableTitles() const = 0;
	virtual int currentTitle() const = 0;
	virtual bool autoplayTitles() const = 0;
	virtual void nextTitle() = 0;
	virtual void previousTitle() = 0;

	Phonon::AudioStreamDescription _currentAudioStream;
	QList<Phonon::AudioStreamDescription> _availableAudioStreams;

	Phonon::SubtitleStreamDescription _currentSubtitleStream;
	QList<Phonon::SubtitleStreamDescription> _availableSubtitleStreams;

	int _currentAngle;
	int _availableAngles;

	int _currentChapter;
	int _availableChapters;

	int _currentTitle;
	int _availableTitles;

	bool _autoplayTitles;

private:
};

}}	//Namespace Phonon::VLC_MPlayer

#endif	//PHONON_VLC_MPLAYER_MEDIACONTROLLER_H
