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

#ifndef LOGGER_H
#define LOGGER_H

#include <Logger/LoggerExport.h>

#include <TkUtil/Singleton.h>

#include <QtCore/QList>
#include <QtCore/QObject>

class LogModel;
class LogMessage;
class LoggerOutput;

enum QtMsgType;

/**
 * Custom message handler for qDebug(), qWarning() and others.
 *
 * Instead of re-inventing a log system, let's use qDebug() and friends.
 * In order to customize log messages we use this class.
 *
 * @see LogMessage
 * @author Tanguy Krotoff
 */
class LOGGER_API Logger : public QObject, public Singleton {
	Q_OBJECT
public:

	/**
	 * Singleton.
	 *
	 * Initiates LogModel (which inherits QAbstractListModel).
	 * LogModel gets all log messages, so in order to not miss any we must create
	 * LogModel before any log message is sent. The best way to achieve this is to
	 * create LogModel inside Logger constructor.
	 * Yes LogModel should be independant of Logger but by creating LogModel
	 * inside Logger constructor we simplify the code.
	 *
	 * Be sure to call this method inside the GUI thread.
	 * Messages sent by Logger to LogModel are done via slot/signal.
	 * QAbstractListModel is not thread-safe + must be created inside GUI thread
	 * so we must use a queued signal otherwise we end up with the following error message:
	 * <pre>Warning QTreeView::rowsInserted internal representation of the model
	 * has been corrupted, resetting</pre>
	 *
	 * @see createLogModel()
	 * @see LogModel::append()
	 */
	static Logger & instance();

	/**
	 * Gets the list of Logger output.
	 *
	 * How to add a Logger output:
	 * <pre>Logger::instance().outputs() += new ConsoleOutput();</pre>
	 *
	 * How to remove a Logger output:
	 * <pre>
	 * LoggerOutput * outputToRemove = NULL;
	 * foreach (LoggerOutput * output, Logger::instance().outputs()) {
	 * 	if (output->name() == "Console") {
	 * 		outputToRemove = output;
	 * 		break;
	 * 	}
	 * }
	 * if (outputToRemove) {
	 * 	Logger::instance().outputs().removeAll(outputToRemove);
	 * }
	 * </pre>
	 */
	QList<LoggerOutput *> & outputs();

	/**
	 * Gets the model for the log.
	 *
	 * This is used by LogWindow.
	 */
	LogModel * logModel() const;

	/**
	 * Custom message handler for qDebug(), qWarning() and others.
	 *
	 * Write this inside your main function:
	 * <pre>qInstallMsgHandler(Logger::myMessageOutput);</pre>
	 * This installs the message handler.
	 */
	static void myMessageOutput(QtMsgType type, const char * msg);

signals:

	/**
	 * A LogMessage has been received.
	 *
	 * This signal is an internal one used to communicate with LogModel.
	 *
	 * LogModel inherits from QAbstractListModel which is not thread-safe.
	 * QAbstractListModel must also be created inside GUI thread.
	 * So we must use a queued signal.
	 */
	void logMessageReceived(const LogMessage & msg);

private:

	Logger();

	~Logger();

	/**
	 * Creates LogModel.
	 *
	 * See instance() method javadoc for explanations.
	 * See constructor implementation.
	 */
	void createLogModel();

	/**
	 * Creates all the outputs.
	 *
	 * @see LoggerOutput
	 */
	void createOutputs();

	/**
	 * Gets the path where the log file is saved.
	 *
	 * Last character is not a '/', you will have to add it yourself if needed.
	 *
	 * @return configuration path
	 */
	QString configDir() const;

	/**
	 * Keeps track of LogModel.
	 *
	 * See instance() method javadoc for explanations.
	 */
	LogModel * _logModel;

	/** List of enabled outputs. */
	QList<LoggerOutput *> _outputs;
};

#endif	//LOGGER_H
