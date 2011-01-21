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

#include "LogModel.h"

#include "LogMessage.h"
#include "LoggerLogger.h"
#include "XmlFileOutput.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>

const int LogModel::COLUMN_TIME = 0;
const int LogModel::COLUMN_TYPE = 1;
const int LogModel::COLUMN_FILE = 2;
const int LogModel::COLUMN_LINE = 3;
const int LogModel::COLUMN_MODULE = 4;
const int LogModel::COLUMN_FUNCTION = 5;
const int LogModel::COLUMN_MESSAGE = 6;
const int LogModel::COLUMN_COUNT = LogModel::COLUMN_MESSAGE + 1;

LogModel::LogModel(QObject * parent)
	: QAbstractListModel(parent) {

#ifdef HACK_ABOUT_TO_QUIT
	_aboutToQuit = false;
	QCoreApplication * app = QCoreApplication::instance();
	Q_ASSERT(app);
	connect(app, SIGNAL(aboutToQuit()), SLOT(aboutToQuit()));
#endif	//HACK_ABOUT_TO_QUIT

	_state = PlayingState;
}

LogModel::~LogModel() {
}

LogMessage LogModel::logMessage(const QModelIndex & index) const {
	LogMessage msg;
	if (!index.isValid()) {
		LoggerCritical() << "Invalid index:" << index;
	} else {
		int row = index.row();
		msg = _log[row];
	}

	return msg;
}

#ifdef HACK_ABOUT_TO_QUIT
void LogModel::aboutToQuit() {
	_aboutToQuit = true;
}
#endif	//HACK_ABOUT_TO_QUIT

void LogModel::append(const LogMessage & msg) {
#ifdef HACK_ABOUT_TO_QUIT
	//HACK
	//Otherwise it crashes inside endInsertRows() when
	//the application quits
	if (_aboutToQuit) {
		return;
	}
#endif	//HACK_ABOUT_TO_QUIT

	if (_state == PlayingState) {
		int first = _log.size();
		int last = _log.size() + 1;

		//Insert rows inside QModel
		//This will tell the view the model has changed
		//and that the view needs to be updated
		beginInsertRows(QModelIndex(), first, last);
		_log += msg;
		endInsertRows();
		///

#ifdef HACK_LAYOUT_CHANGED
		//FIXME there is a Qt bug here:
		//LogWindow is created after LogModel
		//and thus LogWindow does not get updated
		//Using layoutChanged() we force LogWindow
		//to be updated
		//This bug occurs with QTreeView not with QTableView
		emit layoutChanged();
#endif	//HACK_LAYOUT_CHANGED
	}
}

LogModel::State LogModel::state() const {
	return _state;
}

void LogModel::resume() {
	_state = PlayingState;
}

void LogModel::pause() {
	_state = PausedState;
}

void LogModel::clear() {
	int first = 0;
	int last = _log.size();

	beginRemoveRows(QModelIndex(), first, last);
	_log.clear();
	endRemoveRows();
}

bool LogModel::save(const QString & fileName) const {
	LoggerDebug() << "Save log file:" << fileName;

	bool success = false;

	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QXmlStreamWriter stream(&file);
		stream.setAutoFormatting(true);
		stream.writeStartDocument();

		stream.writeStartElement(XmlFileOutput::XML_ROOT_ELEMENT);
		foreach (LogMessage msg, _log) {
			msg.write(stream);
		}
		stream.writeEndElement();

		stream.writeEndDocument();
		success = true;
	} else {
		LoggerWarning() << "Couldn't open file:" << fileName;
	}
	file.close();

	return success;
}

bool LogModel::open(const QString & fileName) {
	LoggerDebug() << "Open log file:" << fileName;

	bool success = false;

	QFile file(fileName);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		_log.clear();
		reset();

		QXmlStreamReader stream(&file);
		if (stream.readNextStartElement()) {
			if (stream.name() == "Log") {

				while (!stream.atEnd()) {
					stream.readNext();

					if (stream.isStartElement()) {
						append(LogMessage(stream));
					}
				}

			}
		}

		if (stream.hasError()) {
			LoggerWarning() << "Error:" << stream.errorString()
				<< "file:" << fileName
				<< "line:" << stream.lineNumber()
				<< "column:" << stream.columnNumber();
		} else {
			success = true;
		}
	} else {
		LoggerWarning() << "Couldn't open file:" << fileName;
	}
	file.close();

	return success;
}

int LogModel::columnCount(const QModelIndex & parent) const {
	Q_UNUSED(parent);
	return COLUMN_COUNT;
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const {
	QVariant tmp;

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case COLUMN_TIME:
			tmp = tr("Time");
			break;
		case COLUMN_TYPE:
			tmp = tr("Type");
			break;
		case COLUMN_FILE:
			tmp = tr("File");
			break;
		case COLUMN_LINE:
			tmp = tr("Line");
			break;
		case COLUMN_MODULE:
			tmp = tr("Module");
			break;
		case COLUMN_FUNCTION:
			tmp = tr("Function");
			break;
		case COLUMN_MESSAGE:
			tmp = tr("Message");
			break;
		default:
			LoggerCritical() << "Unknown section:" << section;
		}
	}

	return tmp;
}

QVariant LogModel::data(const QModelIndex & index, int role) const {
	QVariant tmp;

	if (!index.isValid() || index.model() != this) {
		return tmp;
	}

	int row = index.row();
	int column = index.column();

	LogMessage msg(_log[row]);

	switch (role) {
	case Qt::DisplayRole: {
		switch (column) {
		case COLUMN_TIME:
			tmp = msg.time.toString(LogMessage::TIME_FORMAT);
			break;
		case COLUMN_TYPE:
			tmp = LogMessage::msgTypeToString(msg.type);
			break;
		case COLUMN_FILE:
			tmp = msg.file;
			break;
		case COLUMN_LINE:
			if (msg.line > 0) {
				tmp = msg.line;
			} else {
				//Nothing to display
			}
			break;
		case COLUMN_MODULE:
			tmp = msg.module;
			break;
		case COLUMN_FUNCTION:
			tmp = msg.function;
			break;
		case COLUMN_MESSAGE:
			tmp = msg.message;
			break;
		}
		break;
	}
	}

	return tmp;
}

int LogModel::rowCount(const QModelIndex & parent) const {
	if (parent.isValid()) {
		return 0;
	}

	return _log.size();
}
