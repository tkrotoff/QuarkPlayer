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

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <quarkplayer/QuarkPlayerExport.h>

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>

class LogMessage;

/**
 * Model for LogWindow.
 *
 * This is used to show QuarkPlayer logs.
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API LogModel : public QAbstractListModel {
	Q_OBJECT
public:

	static const int COLUMN_TIME;
	static const int COLUMN_TYPE;
	static const int COLUMN_FILE;
	static const int COLUMN_LINE;
	static const int COLUMN_MODULE;
	static const int COLUMN_FUNCTION;
	static const int COLUMN_MESSAGE;
	static const int COLUMN_COUNT;

	LogModel(QObject * parent);

	~LogModel();

	/**
	 * Resumes the log.
	 *
	 * Does nothing if not in pause state.
	 */
	void resume();

	/**
	 * Pauses the log.
	 *
	 * Does nothing if not in play state.
	 */
	void pause();

	/**
	 * Resets the model.
	 */
	void clear();

	/**
	 * Gets the log message given its index.
	 *
	 * @return LogMessage object, can be empty if the index is invalid
	 */
	LogMessage logMessage(const QModelIndex & index) const;

	/**
	 * Saves log messages into a file.
	 *
	 * @param fileName file where to save the log messages
	 * @return true if success; false otherwise
	 */
	bool save(const QString & fileName) const;

	/**
	 * Opens a log file.
	 *
	 * @param fileName log file to open
	 * @return true if success; false otherwise
	 */
	bool open(const QString & fileName);

	/**
	 * @name Inherited from QAbstractListModel
	 * @{
	 */

	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;

	/** @} */

public slots:

	/**
	 * Appends a new log message.
	 *
	 * QAbstractListModel is not thread-safe + must be created inside GUI thread
	 * so we must use a queued signal otherwise we end up with the following error message:
	 * <pre>Warning QTreeView::rowsInserted internal representation of the model
	 * has been corrupted, resetting</pre
	 *
	 * @see http://lists.trolltech.com/pipermail/qt-interest/2009-June/009215.html
	 * @see http://www.qtcentre.org/threads/28150-Subclassi%C4%B1ng-QAbstractItemModel-for-a-thread-safe-tree-model
	 * @see http://forum.qtfr.org/viewtopic.php?pid=74128
	 *
	 * @param msg add a new log message to the model
	 *
	 * @see LogMessageHandler
	 */
	void append(const LogMessage & msg);

private:

	/** Keeps all the log messages. */
	QList<LogMessage> _log;
};

#endif	//LOGMODEL_H
