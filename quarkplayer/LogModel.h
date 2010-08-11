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

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <quarkplayer/QuarkPlayerExport.h>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>

class LogMessage;

/**
 * Model for LogWindow.
 *
 * This is used to show QuarkPlayer logs.
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API LogModel : public QAbstractItemModel {
	Q_OBJECT
public:

	static const int COLUMN_TIME;
	static const int COLUMN_TYPE;
	static const int COLUMN_MODULE;
	static const int COLUMN_FUNCTION;
	static const int COLUMN_MSG;

	LogModel(QObject * parent);

	~LogModel();

	/**
	 * Appends a new log message.
	 */
	void appendLogMsg(const LogMessage & msg);

	/**
	 * Gets the log message given its index.
	 *
	 * @return LogMessage object, can be empty if the index is invalid
	 */
	LogMessage logMessage(const QModelIndex & index) const;

	/**
	 * @name Inherited from QAbstractItemModel
	 * @{
	 */

	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;

	/** @} */

private:

	/** Keeps all the log messages. */
	QList<LogMessage> _log;
};

#endif	//LOGMODEL_H
