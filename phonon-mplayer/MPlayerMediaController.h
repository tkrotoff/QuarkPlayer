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

#include "MediaController.h"

#include <libmplayer/MPlayerProcess.h>

namespace Phonon
{
namespace VLC_MPlayer
{

/**
 * MediaController specific code for MPlayer.
 *
 * @author Tanguy Krotoff
 */
class MPlayerMediaController : public MediaController {
public:

	MPlayerMediaController();
	virtual ~MPlayerMediaController();

	virtual void audioChannelAdded(int id, const QString & lang) = 0;
	virtual void subtitleAdded(int id, const QString & lang, const QString & type) = 0;
	virtual void titleAdded(int id, qint64 length) = 0;
	virtual void chapterAdded(int titleId, int chapters) = 0;
	virtual void mkvChapterAdded(int id, const QString & title, const QString & from, const QString & to) = 0;
	virtual void angleAdded(int titleId, int angles) = 0;

protected:

	//AudioChannel
	void setCurrentAudioChannel(const Phonon::AudioChannelDescription & audioChannel);
	QList<Phonon::AudioChannelDescription> availableAudioChannels() const;
	Phonon::AudioChannelDescription currentAudioChannel() const;

	//Subtitle
	void setCurrentSubtitle(const Phonon::SubtitleDescription & subtitle);
	QList<Phonon::SubtitleDescription> availableSubtitles() const;
	Phonon::SubtitleDescription currentSubtitle() const;

	//Angle
	void setCurrentAngle(int angleNumber);
	int availableAngles() const;
	int currentAngle() const;

#ifdef NEW_TITLE_CHAPTER_HANDLING
	//Chapter
	void setCurrentChapter(const Phonon::ChapterDescription & chapter);
	QList<Phonon::ChapterDescription> availableChapters() const;
	Phonon::ChapterDescription currentChapter() const;
#else
	//Chapter
	void setCurrentChapter(int chapterNumber);
	int availableChapters() const;
	int currentChapter() const;
#endif	//NEW_TITLE_CHAPTER_HANDLING

#ifdef NEW_TITLE_CHAPTER_HANDLING
	//Title
	void setCurrentTitle(const Phonon::TitleDescription & title);
	QList<Phonon::TitleDescription> availableTitles() const;
	Phonon::TitleDescription currentTitle() const;
#else
	//Title
	void setCurrentTitle(int titleNumber);
	int availableTitles() const;
	int currentTitle() const;
#endif	//NEW_TITLE_CHAPTER_HANDLING

	void setAutoplayTitles(bool autoplay);
	bool autoplayTitles() const;

	void clearMediaController();

	/** MPlayer process. */
	MPlayerProcess * _process;

private:

	void loadSubtitleFile(const QString & filename);
};

}}	//Namespace Phonon::VLC_MPlayer

#endif	//PHONON_VLC_MPLAYER_MPLAYERMEDIACONTROLLER_H
