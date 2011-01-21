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

#include "LogMessage.h"

#include "LoggerLogger.h"

#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>

const char * LogMessage::TIME_FORMAT = "hh:mm:ss.zzz";

LogMessage::LogMessage() {
	type = QtDebugMsg;

	//0 in case of error instead of -1 because QString::toInt() returns 0
	//if fails
	//__LINE__ should never return 0
	line = 0;
}

LogMessage::LogMessage(QtMsgType _type, const QString & msg) {

	//MinGW: "QP_LOGGER "C:\Documents and Settings\tkrotoff\Desktop\quarkplayer\trunk\quarkplayer-app\main.cpp" 64 QuarkPlayerCore main Current date and time: "ven. 29. oct. 12:13:26 2010" "
	//MinGW: "QP_LOGGER "C:\Documents and Settings\tkrotoff\Desktop\quarkplayer\trunk\quarkplayer\PluginManager.cpp" 69 QuarkPlayerCore findPluginDir Checking for plugins"
	//Visual C++ 2010: "QP_LOGGER "C:\Users\Alisson\Desktop\quarkplayer\trunk\quarkplayer-app\main.cpp" 64 QuarkPlayerCore main Current date and time: "Sat Oct 30 15:32:23 2010" "
	//Visual C++ 2010: "QP_LOGGER "C:\Users\Alisson\Desktop\quarkplayer\trunk\quarkplayer\PluginManager.cpp" 69 QuarkPlayerCore PluginManager::findPluginDir Checking for plugins"

	QString logLine(msg);
	logLine = logLine.trimmed();	//The message can contain begin and end spaces

	QString sourceCodeFileName;
	QString sourceCodeLineNumber;
	QString module;
	QString function;

	//Do not use regexp here as it might be slow

	static const QString LOGGER_STRING_TO_MATCH = "QP_LOGGER";
	if (logLine.startsWith(LOGGER_STRING_TO_MATCH)) {
		//+ 2 characters because of space + "
		logLine.remove(0, LOGGER_STRING_TO_MATCH.length() + 2);

		int index = logLine.indexOf('"');
		sourceCodeFileName = logLine.left(index);
		//+ 2 characters because of " + space
		logLine.remove(0, sourceCodeFileName.length() + 2);

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

	init(QTime::currentTime(), _type, sourceCodeFileName, sourceCodeLineNumber.toInt(), module, function, logLine);
}

void LogMessage::init(
	const QTime & _time,
	QtMsgType _type,
	const QString & _file, int _line,
	const QString & _module,
	const QString & _function,
	const QString & _message) {

	time = _time;
	type = _type;
	file = _file;
	line = _line;
	module = _module;
	function = _function;
	message = _message;
}

LogMessage::LogMessage(QXmlStreamReader & stream) {
	read(stream);
}

bool LogMessage::operator==(const LogMessage & msg) const {
	bool equal = false;

	if (time == msg.time
		&& type == msg.type
		&& file == msg.file
		&& line == msg.line
		&& module == msg.module
		&& function == msg.function
		&& message == msg.message) {
		equal = true;
	}

	return equal;
}

bool LogMessage::isEmpty() const {
	bool empty = false;

	if (time == QTime()
		&& type == QtDebugMsg
		&& file == QString()
		&& line == 0
		&& module == QString()
		&& function == QString()
		&& message == QString()) {
		empty = true;
	}

	return empty;
}

bool LogMessage::read(QXmlStreamReader & stream) {
	bool success = false;

	while (!stream.atEnd()) {
		stream.readNext();

		QString element = stream.name().toString();

		if (stream.isStartElement()) {
			if (element == "LogMessage") {
				//Do nothing
			}
			else if (element == "Time") {
				QString text = stream.readElementText();
				time = QTime::fromString(text, TIME_FORMAT);
			}
			else if (element == "Type") {
				QString text = stream.readElementText();
				type = stringToMsgType(text);
			}
			else if (element == "File") {
				file = stream.readElementText();
			}
			else if (element == "Line") {
				line = stream.readElementText().toInt();
			}
			else if (element == "Module") {
				module = stream.readElementText();
			}
			else if (element == "Function") {
				function = stream.readElementText();
			}
			else if (element == "Message") {
				message = stream.readElementText();
			}
			else {
				LoggerWarning() << "Unknown XML node:" << element;
			}
		}

		if (stream.isEndElement()) {
			if (element == "LogMessage") {
				success = true;
				//Stop here: we have read one message
				break;
			}
		}
	}

	if (stream.hasError()) {
		LoggerWarning() << "Error:" << stream.errorString()
			<< "line:" << stream.lineNumber()
			<< "column:" << stream.columnNumber();
	}

	return success;
}

QString LogMessage::toString() const {
	QString tmp = time.toString(TIME_FORMAT);
	tmp += ' ' + msgTypeToString(type);
	if (!module.isEmpty()) {
		tmp += ' ' + module;
	}
	if (!function.isEmpty()) {
		tmp += ' ' + function;
	}
	if (!message.isEmpty()) {
		tmp += ' ' + message;
	}

	return tmp;
}

void LogMessage::write(QXmlStreamWriter & stream) const {
	stream.writeStartElement("LogMessage");
		stream.writeTextElement("Time", time.toString(TIME_FORMAT));
		stream.writeTextElement("Type", msgTypeToString(type));
		stream.writeTextElement("File", file);
		stream.writeTextElement("Line", QString::number(line));
		stream.writeTextElement("Module", module);
		stream.writeTextElement("Function", function);
		stream.writeTextElement("Message", message);
	stream.writeEndElement();
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
		LoggerCritical() << "Unknown message type:" << type;
	}

	return str;
}

QtMsgType LogMessage::stringToMsgType(const QString & type) {
	QtMsgType msgType = QtDebugMsg;

	if (type == "Debug") {
		msgType = QtDebugMsg;
	} else if (type == "Warning") {
		msgType = QtWarningMsg;
	} else if (type == "Critical") {
		msgType = QtCriticalMsg;
	} else if (type == "Fatal") {
		msgType = QtFatalMsg;
	} else {
		LoggerCritical() << "Unknown message type:" << type;
	}

	return msgType;
}
