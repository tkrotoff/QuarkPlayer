/*
 * Copyright (c) 2010-2011 Tanguy Krotoff.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "Logger.h"

#include "LogModel.h"
#include "LogMessage.h"
#include "LoggerOutput.h"
#include "ConsoleOutput.h"
#include "XmlFileOutput.h"

#include <QtCore/QtGlobal>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>
#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>

#include <iostream>

Logger::Logger() {
	createLogModel();

	createOutputs();
}

Logger::~Logger() {
	//No need, will be deleted when Logger is deleted
	//+ Logger should never be deleted otherwise
	//Logger::myMessageOutput() will crash
	//delete _logModel;

	//Remove our message handler function
	//otherwise it will crash inside myMessageOutput()
	qInstallMsgHandler(NULL);

	//Delete all outputs
	foreach (LoggerOutput * output, _outputs) {
		delete output;
	}
	_outputs.clear();
	///
}

Logger & Logger::instance() {
	static Logger instance;
	return instance;
}

void Logger::createOutputs() {
	//Yes all outputs are loaded in memory all the time
	//Is this really a problem?
	//This allows LogWindow to list all outputs and provide a way
	//to enable/disable each output
	_outputs += new ConsoleOutput(true);
	_outputs += new XmlFileOutput(true, configDir() + "/QuarkPlayer.log");
}

QString Logger::configDir() const {
	//To avoid depending on TkConfig::configDir() and then to link with TkUtil
	//Each time this is modified, don't forget to modify TkConfig::TkConfig()

	//Application name & organization name cannot be empty
	Q_ASSERT(!QCoreApplication::applicationName().isEmpty());
	Q_ASSERT(!QCoreApplication::organizationName().isEmpty());

#ifdef Q_WS_WIN
	//Forces INI file format instead of using Windows registry database
	QSettings settings(QSettings::IniFormat, QSettings::UserScope,
		QCoreApplication::organizationName(), QCoreApplication::applicationName());
#else
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
		QCoreApplication::organizationName(), QCoreApplication::applicationName());
#endif	//Q_WS_WIN

	return QFileInfo(settings.fileName()).absolutePath();
}

QList<LoggerOutput *> & Logger::outputs() {
	return _outputs;
}

void Logger::createLogModel() {
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

LogModel * Logger::logModel() const {
	return _logModel;
}

void Logger::myMessageOutput(QtMsgType type, const char * msg) {
	//See http://blog.codeimproved.net/2010/03/logging-in-qt-land/
	//There is a risk of deadlock or endless loop if an Qt error occurs
	//in this code

	LogMessage logMsg(type, msg);

	QList<LoggerOutput *> & outputs = instance().outputs();
	foreach (LoggerOutput * output, outputs) {
		if (output->isEnabled()) {
			output->write(logMsg);
		}
	}

	emit instance().logMessageReceived(logMsg);
}
