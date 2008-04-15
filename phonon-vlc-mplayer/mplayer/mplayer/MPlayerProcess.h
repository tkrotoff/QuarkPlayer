/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
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

#ifndef MPLAYERPROCESS_H
#define MPLAYERPROCESS_H

#include <QtCore/QString>

#include "MyProcess.h"
#include "MediaData.h"

/**
 * Creates a new MPlayer process.
 *
 * Permits to send commands to the MPlayer process via its slave mode
 * Permits to receive events from the MPlayer process
 *
 * Check the MPlayer slave mode documentation: http://www.mplayerhq.hu/DOCS/tech/slave.txt
 *
 * @see mplayer-input-cmdlist.txt
 * @see http://www.mplayerhq.hu/DOCS/tech/slave.txt
 * @author Ricardo Villalba
 * @author Tanguy Krotoff
 */
class MPlayerProcess : public MyProcess {
	Q_OBJECT
public:

	/**
	 * The state of the media.
	 *
	 * Taken and adapted from Phonon
	 *
	 * @see phononnamespace.h
	 */
	enum State {

		/**
		 * After construction it might take a while before the Player is
		 * ready to play(). Normally this doesn't happen for local
		 * files, but can happen for remote files where the asynchronous
		 * mimetype detection and prebuffering can take a while.
		 */
		LoadingState,

		/**
		 * The Player has a valid media file loaded and is ready for
		 * playing.
		 */
		//StoppedState,

		/**
		 * The Player reached the end of the stream/media/file.
		 */
		EndOfFileState,

		/**
		 * The Player is playing a media file.
		 */
		PlayingState,

		/**
		 * The Player is waiting for data to be able to continue
		 * playing.
		 */
		BufferingState,

		/**
		 * The Player is currently paused.
		 */
		PausedState,

		/**
		 * An unrecoverable error occurred. The Object is unusable in this state.
		 */
		ErrorState
	};

	MPlayerProcess(QObject * parent);
	~MPlayerProcess();

	/** Start the MPlayer process. */
	bool start(const QStringList & arguments, const QString & filename, int videoWidgetId, double seek);

	void stop();

	/**
	 * Sends a command to the MPlayer process.
	 *
	 * Example:
	 *
	 * @see http://www.mplayerhq.hu/DOCS/tech/slave.txt
	 * @see mplayer-input-cmdlist.txt
	 */
	void writeToStdin(const QString & command);

	MediaData getMediaData() const;

signals:

	/**
	 * Emitted when the state of the media has changed.
	 *
	 * @param state The state the Player is in now.
	 */
	void stateChanged(MPlayerProcess::State state);

	/**
	 * Gives the current position in the stream.
	 *
	 * Example: "MPlayer is playing at 28,5 seconds from a video file of a 45,0 seconds length"
	 *
	 * @param time position in the stream in seconds (i.e 28,5 seconds)
	 * @see Phonon::MediaObject::tick()
	 */
	void tick(double time);

	void receivedCurrentFrame(int frame);

	void receivedWindowResolution(int,int);

	/**
	 * If the stream contains a video or not.
	 *
	 * @see Phonon::MediaObject::hasVideoChanged()
	 */
	void hasVideoChanged(bool hasVideo);

	void receivedVO(QString);
	void receivedAO(QString);

	void mplayerFullyLoaded();

	void receivedStartingTime(double sec);

	void receivedCacheMessage(QString);
	void receivedCreatingIndex(QString);
	void receivedConnectingToMessage(QString);
	void receivedResolvingMessage(QString);

	/**
	 * @param filename screenshot filename
	 */
	void screenshotSaved(const QString & filename);

	void streamTitleAndUrl(const QString & title, const QString & url);

	void failedToParseMplayerVersion(QString line_with_mplayer_version);

private slots:

	/**
	 * Parses a line from the MPlayer process.
	 *
	 * @param line line to parse
	 */
	void parseLine(const QByteArray & line);

	void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:

	bool _notifiedMPlayerIsRunning;
	bool _endOfFileReached;

	MediaData _data;

	int _mplayerSvnRevision;
};

#endif	//MPLAYERPROCESS_H
