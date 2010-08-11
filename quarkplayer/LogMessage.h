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

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include <quarkplayer/QuarkPlayerExport.h>

#include <QtCore/QString>
#include <QtCore/QTime>

enum QtMsgType;

/**
 * A message inside the log system.
 *
 * @see LogMessageHandler
 * @see LogWindow
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API LogMessage {
public:

	LogMessage();

	LogMessage(const QTime & time, QtMsgType type, const QString & module,
		const QString & function, const QString & msg);

	//FIXME isEmpty() API?

	QTime time;
	QtMsgType type;
	QString module;
	QString function;
	QString msg;

	QString toString() const;

	/**
	 * String representation of enum QtMsgType.
	 */
	static QString msgTypeToString(QtMsgType type);
};

#endif	//LOGMESSAGE_H
