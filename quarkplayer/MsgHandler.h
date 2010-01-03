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

#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include <quarkplayer/quarkplayer_export.h>

#include <TkUtil/Singleton.h>

#include <QtCore/QObject>
#include <QtCore/QString>

enum QtMsgType;

/**
 * Custom message handler for qDebug(), qWarning() and others.
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API MsgHandler : public QObject, public Singleton<MsgHandler> {
	Q_OBJECT
	friend class Singleton<MsgHandler>;
public:

	/**
	 * Specific to the MPlayer backend: contains the MPlayer real output.
	 */
	QString mplayerLog;

	/**
	 * Custom message handler for qDebug(), qWarning() and others.
	 */
	static void myMessageOutput(QtMsgType type, const char * msg);

signals:

	/**
	 * A new line from MPlayer output has been generated.
	 *
	 * @param line MPlayer output line
	 */
	void mplayerLogLineAvailable(const QString & line);

private:

	MsgHandler();

	~MsgHandler();
};

#endif	//MSGHANDLER_H
