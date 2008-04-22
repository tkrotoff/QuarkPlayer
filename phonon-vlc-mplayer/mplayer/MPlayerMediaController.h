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

#ifndef PHONON_VLC_MPLAYER_MPLAYERMEDIACONTROLLER_H
#define PHONON_VLC_MPLAYER_MPLAYERMEDIACONTROLLER_H

#include "../MediaController.h"

#include <mplayer/MPlayerProcess.h>

namespace Phonon
{
namespace VLC_MPlayer
{

/**
 *
 *
 * @author Tanguy Krotoff
 */
class MPlayerMediaController : public MediaController {
public:

	MPlayerMediaController();
	virtual ~MPlayerMediaController();

	virtual void audioStreamAdded(int id, const QString & lang) = 0;
	virtual void subtitleStreamAdded(int id, const QString & lang, const QString & type) = 0;
	virtual void titleAdded(int id, qint64 length) = 0;
	virtual void chapterAdded(int titleId, int chapters) = 0;
	virtual void angleAdded(int titleId, int angles) = 0;

protected:

	//AudioStream
	void setCurrentAudioStream(const Phonon::AudioStreamDescription & stream);
	QList<Phonon::AudioStreamDescription> availableAudioStreams() const;
	Phonon::AudioStreamDescription currentAudioStream() const;

	//SubtitleStream
	void setCurrentSubtitleStream(const Phonon::SubtitleStreamDescription & stream);
	QList<Phonon::SubtitleStreamDescription> availableSubtitleStreams() const;
	Phonon::SubtitleStreamDescription currentSubtitleStream() const;

	//Angle
	void setCurrentAngle(int angleNumber);
	int availableAngles() const;
	int currentAngle() const;

	//Chapter
	void setCurrentChapter(int chapterNumber);
	int availableChapters() const;
	int currentChapter() const;

	//Title
	void setCurrentTitle(int titleNumber);
	void setAutoplayTitles(bool autoplay);
	int availableTitles() const;
	int currentTitle() const;
	bool autoplayTitles() const;
	void nextTitle();
	void previousTitle();


	/** MPlayer process. */
	MPlayerProcess * _process;

private:

	void clearAllButTitle();
};

}}	//Namespace Phonon::VLC_MPlayer

#endif	//PHONON_VLC_MPLAYER_MPLAYERMEDIACONTROLLER_H
