/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "LogMessageHandler.h"

#include "LogModel.h"
#include "LogMessage.h"

#include <QtCore/QtGlobal>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>

#include <iostream>

LogMessageHandler::LogMessageHandler() {
	qRegisterMetaType<LogMessage>("LogMessage");

	//Parent is "NULL" instead of "this" otherwise we end up with
	//the following warning from Qt:
	//QObject::startTimer: QTimer can only be used with threads started with QThread
	_logModel = new LogModel(NULL);

	//QAbstractListModel is not thread-safe + must be created inside GUI thread
	//so we must use a queued signal
	connect(this, SIGNAL(logMessageReceived(const LogMessage &)),
		_logModel, SLOT(append(const LogMessage &)));
}

LogMessageHandler::~LogMessageHandler() {
	//No need, will be deleted when LogMessageHandler is deleted
	//+ LogMessageHandler should never be deleted otherwise
	//LogMessageHandler::myMessageOutput() will crash
	//delete _logModel;

	//Remove our message handler function
	//otherwise it will crash inside myMessageOutput()
	qInstallMsgHandler(NULL);
}

LogMessageHandler & LogMessageHandler::instance() {
	static LogMessageHandler instance;
	return instance;
}

LogModel * LogMessageHandler::logModel() const {
	return _logModel;
}

void LogMessageHandler::myMessageOutput(QtMsgType type, const char * msg) {
	//See http://blog.codeimproved.net/2010/03/logging-in-qt-land/
	//There is a risk of deadlock or endless loop if an Qt error occurs
	//in this code

	//MinGW: "QP_LOGGER C:\Documents and Settings\tkrotoff\Desktop\quarkplayer\trunk\quarkplayer-app\main.cpp 64 QuarkPlayerCore main Current date and time: "ven. 29. oct. 12:13:26 2010" "
	//MinGW: "QP_LOGGER C:\Documents and Settings\tkrotoff\Desktop\quarkplayer\trunk\quarkplayer\PluginManager.cpp 69 QuarkPlayerCore findPluginDir Checking for plugins"
	//Visual C++ 2010:

	QString logLine(msg);
	logLine = logLine.trimmed();	//The message can contain begin and end spaces

	QString sourceCodeFileName;
	QString sourceCodeLineNumber;
	QString module;
	QString function;

	//Do not use regexp here as it might be slow

	static const QString LOGGER_STRING_TO_MATCH = "QP_LOGGER";
	if (logLine.startsWith(LOGGER_STRING_TO_MATCH)) {
		logLine.remove(0, LOGGER_STRING_TO_MATCH.length() + 1);

		int index = logLine.indexOf(' ');
		sourceCodeFileName = logLine.left(index);
		logLine.remove(0, sourceCodeFileName.length() + 1);

		index = logLine.indexOf(' ');
		sourceCodeLineNumber = logLine.left(index);
		logLine.remove(0, sourceCodeLineNumber.length() + 1);

		index = logLine.indexOf(' ');
		module = logLine.left(index);
		logLine.remove(0, module.length() + 1);

		index = logLine.indexOf(' ');
		function = logLine.left(index);
		logLine.remove(0, function.length() + 1);
	} else {
		//Special case of MPlayer, parses messages from phonon-mplayer
		//MPlayer messages are logged this way:
		//qDebug() << "MPlayer" << line.toUtf8().constData()
		//See method MPlayerProcess::parseLine(const QString & line)
		static const QString MPLAYER_STRING_TO_MATCH = "MPlayer";
		if (logLine.startsWith(MPLAYER_STRING_TO_MATCH)) {
			module = MPLAYER_STRING_TO_MATCH;
			logLine.remove(0, MPLAYER_STRING_TO_MATCH.length() + 1);
		}

		else {
			//std::cerr << "Error, string does not match: " << logLine.toUtf8().constData() << "." << std::endl;
			//std::cerr << "Error, string to match: " << LOGGER_STRING_TO_MATCH.toUtf8().constData() << "." << std::endl;
		}
	}

	LogMessage logMsg(QTime::currentTime(), type, sourceCodeFileName, sourceCodeLineNumber.toInt(), module, function, logLine);

	printLogMessage(logMsg);

	emit LogMessageHandler::instance().logMessageReceived(logMsg);
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
