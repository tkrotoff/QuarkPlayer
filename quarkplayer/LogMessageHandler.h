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

#ifndef LOGMESSAGEHANDLER_H
#define LOGMESSAGEHANDLER_H

#include <quarkplayer/quarkplayer_export.h>

#include <TkUtil/Singleton.h>

#include <QtCore/QObject>
#include <QtCore/QList>

class LogModel;
class LogMessage;
enum QtMsgType;

/**
 * Custom message handler for qDebug(), qWarning() and others.
 *
 * Instead of re-inventing a log system, let's use qDebug() and friends.
 * In order to customize log messages we use this class.
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API LogMessageHandler : public QObject, public Singleton<LogMessageHandler> {
	Q_OBJECT
	friend class Singleton<LogMessageHandler>;
public:

	/**
	 * Gets the model for the log.
	 *
	 * This is used by LogWindow.
	 */
	LogModel * logModel() const;

	/**
	 * Custom message handler for qDebug(), qWarning() and others.
	 */
	static void myMessageOutput(QtMsgType type, const char * msg);

private:

	/**
	 * Prints the log message inside the console output.
	 *
	 * Uses Qt internal function qt_message_output()
	 */
	static void printLogMessage(const LogMessage & msg);

	LogModel * _logModel;

	LogMessageHandler();

	~LogMessageHandler();
};

#endif	//LOGMESSAGEHANDLER_H
