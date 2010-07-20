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

#include "LogMessage.h"

#include "QuarkPlayerCoreLogger.h"

LogMessage::LogMessage() {
}

LogMessage::LogMessage(const QTime & _time, QtMsgType _type, const QString & _module,
	const QString & _function, const QString & _msg) {

	time = _time;
	type = _type;
	module = _module;
	function = _function;
	msg = _msg;
}

QString LogMessage::toString() const {
	QString tmp = time.toString("hh:mm:ss.zzz");
	tmp += " " + msgTypeToString(type);
	tmp += " " + module;
	tmp += " " + function;
	tmp += " " + msg;

	return tmp;
}

QString LogMessage::msgTypeToString(QtMsgType type) {
	QString str;
	switch (type) {
	case QtDebugMsg:
		str = "Debug";
		break;
	case QtWarningMsg:
		str = "Warning";
		break;
	case QtCriticalMsg:
		str = "Critical";
		break;
	case QtFatalMsg:
		str = "Fatal";
		break;
	default:
		QuarkPlayerCoreCritical() << "Unknown message type:" << type;
	}

	return str;
}
