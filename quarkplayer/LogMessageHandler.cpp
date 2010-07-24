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

#include "LogMessageHandler.h"

#include "LogModel.h"
#include "LogMessage.h"

#include <QtCore/QtGlobal>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>

LogMessageHandler::LogMessageHandler() {
	_logModel = new LogModel(this);
}

LogMessageHandler::~LogMessageHandler() {
	//No need, will be deleted when LogMessageHandler is deleted
	//+ LogMessageHandler should never be deleted otherwise
	//LogMessageHandler::myMessageOutput() will crash
	//delete _logModel;
}

LogModel * LogMessageHandler::logModel() const {
	return _logModel;
}

void LogMessageHandler::myMessageOutput(QtMsgType type, const char * msg) {
	QString logLine(msg);
	//logLine = logLine.trimmed();

	QString module;
	QString function;

	//Do not use regexp here as it might be slow

	static const QString internalStringToMatch("QP_LOGGER");
	if (logLine.startsWith(internalStringToMatch)) {
		logLine.remove(0, internalStringToMatch.length() + 1);

		int index = logLine.indexOf(' ');
		module = logLine.left(index);
		logLine.remove(0, module.length() + 1);

		index = logLine.indexOf(' ');
		function = logLine.left(index);
		logLine.remove(0, function.length() + 1);
	}

	//Special case of MPlayer, parses messages from phonon-mplayer
	else if (logLine.startsWith("MPlayer")) {
		module = "MPlayer";
		logLine.remove(0, QString("MPlayer").length() + 1);
	}

	LogMessage logMsg(QTime::currentTime(), type, module, function, logLine);

	printLogMessage(logMsg);

	LogMessageHandler::instance()._logModel->appendLogMsg(logMsg);
}

void LogMessageHandler::printLogMessage(const LogMessage & msg) {
	QString tmp = msg.toString();

	qInstallMsgHandler(NULL);

	//Back to the default Qt message handler
	//This is an internal function from Qt
	//Read file C:\Qt\4.6.0\src\corelib\global\qglobal.cpp
	qt_message_output(msg.type, tmp.toUtf8().constData());

	qInstallMsgHandler(LogMessageHandler::myMessageOutput);
}