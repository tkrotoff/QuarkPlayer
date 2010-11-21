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

	LogMessage logMsg(type, msg);

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
