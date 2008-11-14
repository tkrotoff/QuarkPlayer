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

#ifndef PHONON_VLC_MPLAYER_MPLAYERMEDIAOBJECT_H
#define PHONON_VLC_MPLAYER_MPLAYERMEDIAOBJECT_H

#include "MPlayerMediaController.h"

#include "../MediaObject.h"

#include <QtCore/QProcess>

namespace Phonon
{
namespace VLC_MPlayer
{

/**
 * MPlayer MediaObject.
 *
 * Encapsulates MPlayer MediaObject specific code.
 *
 * This is the "brain" of the MPlayer backend.
 * MPlayerMediaObject uses libmplayer in order to send commands and receive
 * events from the MPlayer process.
 *
 * @author Tanguy Krotoff
 */
class MPlayerMediaObject : public MediaObject, public MPlayerMediaController {
	Q_OBJECT
	Q_INTERFACES(Phonon::MediaObjectInterface Phonon::AddonInterface)
public:

	MPlayerMediaObject(QObject * parent);
	~MPlayerMediaObject();

	void pause();
	void stop();

	bool hasVideo() const;
	bool isSeekable() const;

	qint64 totalTime() const;

	QString errorString() const;

	/**
	 * Gets the MPlayerProcess used by this class.
	 *
	 * Cannot be NULL.
	 *
	 * @return the MPlayerProcess
	 */
	MPlayerProcess * getMPlayerProcess() const;

	///FIXME
	void setNextSource(const MediaSource & source);

signals:

	//MediaController signals
	void availableSubtitlesChanged();
	void availableAudioChannelsChanged();

	void availableChaptersChanged();
	void availableTitlesChanged();

	void availableChaptersChanged(int availableChapters);
	void availableTitlesChanged(int availableTitles);

	void availableAnglesChanged(int availableAngles);
	void angleChanged(int angleNumber);
	void chapterChanged(int chapterNumber);
	void titleChanged(int titleNumber);

protected:

	void loadMediaInternal(const QString & filename);
	void playInternal();
	void seekInternal(qint64 milliseconds);

	qint64 currentTimeInternal() const;

private slots:

	void finished(int exitCode, QProcess::ExitStatus exitStatus);

	void stateChangedInternal(MPlayerProcess::State state);

	void loadMediaInternal();

	void mediaLoaded();

	void mediaDataChanged(const MediaData & mediaData);

	//MediaController
	void audioChannelAdded(int id, const QString & lang);
	void subtitleAdded(int id, const QString & lang, const QString & type);
	void titleAdded(int id, qint64 length);
	void chapterAdded(int titleId, int chapters);
	void mkvChapterAdded(int id, const QString & title, const QString & from, const QString & to);
	void angleAdded(int titleId, int angles);

private:

	bool _playRequestReached;

	QString _filename;
};

}}	//Namespace Phonon::MPlayer

#endif	//PHONON_VLC_MPLAYER_MPLAYERMEDIAOBJECT_H
