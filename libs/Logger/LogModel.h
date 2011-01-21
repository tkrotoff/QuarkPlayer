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

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <Logger/LoggerExport.h>

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>

class LogMessage;

/**
 * Model for LogWindow.
 *
 * This is used to show the logs.
 *
 * @author Tanguy Krotoff
 */
class LOGGER_API LogModel : public QAbstractListModel {
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

	/** LogModel state. */
	enum State {
		PlayingState,
		PausedState
	};

	LogModel(QObject * parent);

	~LogModel();

	/**
	 * Returns the current state.
	 *
	 * @return current state
	 */
	State state() const;

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
	 * XML format.
	 *
	 * @param fileName file where to save the XML log messages
	 * @return true if success; false otherwise
	 */
	bool save(const QString & fileName) const;

	/**
	 * Opens a log file.
	 *
	 * XML format.
	 *
	 * @param fileName XML log file to open
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
	 * @see Logger
	 */
	void append(const LogMessage & msg);

private slots:

#ifdef HACK_ABOUT_TO_QUIT
	/**
	 * QCoreApplication::aboutToQuit().
	 *
	 * HACK
	 * Catches aboutToQuit() otherwise it crashes inside endInsertRows() when
	 * the application quits. This is a Qt bug workaround.
	 * Tested under Qt 4.6.2 Ubuntu 9.10.
	 */
	void aboutToQuit();
#endif	//HACK_ABOUT_TO_QUIT

private:

	/** Keeps all the log messages. */
	QList<LogMessage> _log;

#ifdef HACK_ABOUT_TO_QUIT
	/** See aboutToQuit() slot. */
	bool _aboutToQuit;
#endif	//HACK_ABOUT_TO_QUIT

	/** LogModel state. */
	State _state;
};

#endif	//LOGMODEL_H
