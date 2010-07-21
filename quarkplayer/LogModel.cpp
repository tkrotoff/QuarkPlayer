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

const int LogModel::COLUMN_TIME = 0;
const int LogModel::COLUMN_TYPE = 1;
const int LogModel::COLUMN_MODULE = 2;
const int LogModel::COLUMN_FUNCTION = 3;
const int LogModel::COLUMN_MSG = 4;
static const int COLUMN_COUNT = LogModel::COLUMN_MSG + 1;

LogModel::LogModel(QObject * parent)
	: QAbstractItemModel(parent) {
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

void LogModel::appendLogMsg(const LogMessage & msg) {
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
	//Also there is no way for _log to be outside this class
	emit layoutChanged();
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
		case COLUMN_MODULE:
			tmp = tr("Module");
			break;
		case COLUMN_FUNCTION:
			tmp = tr("Function");
			break;
		case COLUMN_MSG:
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
			tmp = msg.time.toString("hh:mm:ss.zzz");
			break;
		case COLUMN_TYPE:
			tmp = LogMessage::msgTypeToString(msg.type);
			break;
		case COLUMN_MODULE:
			tmp = msg.module;
			break;
		case COLUMN_FUNCTION:
			tmp = msg.function;
			break;
		case COLUMN_MSG:
			tmp = msg.msg;
			break;
		}
		break;
	}
	}

	return tmp;
}

QModelIndex LogModel::index(int row, int column, const QModelIndex & parent) const {
	if (parent.isValid()) {
		return QModelIndex();
	}

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	return createIndex(row, column);
}

QModelIndex LogModel::parent(const QModelIndex & index) const {
	Q_UNUSED(index);
	return QModelIndex();
}

int LogModel::rowCount(const QModelIndex & parent) const {
	if (parent.isValid()) {
		return 0;
	}

	return _log.size();
}
