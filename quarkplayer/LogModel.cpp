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

#include "LogModel.h"

#include "LogMessage.h"
#include "QuarkPlayerCoreLogger.h"

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
}

LogModel::~LogModel() {
}

LogMessage LogModel::logMessage(const QModelIndex & index) const {
	LogMessage msg;
	if (!index.isValid()) {
		QuarkPlayerCoreCritical() << "Invalid index:" << index;
	} else {
		int row = index.row();
		msg = _log[row];
	}

	return msg;
}

void LogModel::append(const LogMessage & msg) {
	int first = _log.size();
	int last = _log.size() + 1;

	//Insert rows inside QModel
	//This will tell the view the model has changed
	//and that the view needs to be updated
	beginInsertRows(QModelIndex(), first, last);
	_log += msg;
	endInsertRows();
	///

	//FIXME there is a Qt bug here:
	//LogWindow is created after LogModel
	//and thus LogWindow does not get updated
	//Using layoutChanged() we force LogWindow
	//to be updated
	//This bug occurs with QTreeView not with QTableView
	emit layoutChanged();
}

void LogModel::resume() {
	//_state = PlayState;
}

void LogModel::pause() {
	//_state = PauseState;
}

void LogModel::clear() {
	reset();
}

bool LogModel::save(const QString & fileName) const {
	QuarkPlayerCoreDebug() << "Save log file:" << fileName;

	bool success = false;

	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QXmlStreamWriter stream(&file);
		stream.setAutoFormatting(true);
		stream.writeStartDocument();

		stream.writeStartElement("Log");
		foreach (LogMessage msg, _log) {
			msg.write(stream);
		}
		stream.writeEndElement();

		stream.writeEndDocument();
		success = true;
	} else {
		QuarkPlayerCoreWarning() << "Couldn't open file:" << fileName;
	}
	file.close();

	return success;
}

bool LogModel::open(const QString & fileName) {
	QuarkPlayerCoreDebug() << "Open log file:" << fileName;

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
			QuarkPlayerCoreWarning() << "Error:" << stream.errorString()
				<< "file:" << fileName
				<< "line:" << stream.lineNumber()
				<< "column:" << stream.columnNumber();
		} else {
			success = true;
		}
	} else {
		QuarkPlayerCoreWarning() << "Couldn't open file:" << fileName;
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
			QuarkPlayerCoreCritical() << "Unknown section:" << section;
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
