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
 * Check the MPlayer slave mode protocol documentation: http://www.mplayerhq.hu/DOCS/tech/slave.txt
 *
 * @author Ricardo Villalba
 * @author Tanguy Krotoff
 */
class MPlayerProcess : public MyProcess {
	Q_OBJECT
public:

	MPlayerProcess(QObject * parent);
	virtual ~MPlayerProcess();

	/** Start the MPlayer process. */
	bool start(const QString & program, const QStringList & arguments);

	/**
	 * Sends a command to the MPlayer process.
	 *
	 * Example:
	 *
	 * @see http://www.mplayerhq.hu/DOCS/tech/slave.txt
	 */
	void writeToStdin(const QString & command);

	MediaData mediaData() { return md; };

signals:

	void finished();

	void lineAvailable(QString line);

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

	void pause();

	void receivedWindowResolution(int,int);

	/**
	 * The stream does not contain a video.
	 *
	 * @see Phonon::MediaObject::hasVideoChanged()
	 */
	void hasNoVideo();

	void receivedVO(QString);
	void receivedAO(QString);

	/**
	 * The end of the stream/file (video/audio) has been reached.
	 */
	void endOfFile();

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

	MediaData md;

	int _mplayerSvnRevision;
};

#endif	//MPLAYERPROCESS_H
