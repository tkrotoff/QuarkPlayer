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

LogMessage::LogMessage(
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
				QuarkPlayerCoreWarning() << "Unknown XML node:" << element;
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
		QuarkPlayerCoreWarning() << "Error:" << stream.errorString()
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
		QuarkPlayerCoreCritical() << "Unknown message type:" << type;
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
		QuarkPlayerCoreCritical() << "Unknown message type:" << type;
	}

	return msgType;
}
