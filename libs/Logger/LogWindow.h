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

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <Logger/LoggerExport.h>

#include <QtGui/QMainWindow>

class LogModel;

class QTreeView;
class QModelIndex;
class QPushButton;

/**
 * Window showing current log.
 *
 * LogWindow is a QMainWindow with Qt::WindowFlags being Qt::Dialog
 * instead of default QMainWindow Qt::WindowFlags. This way LogWindow
 * is centered and destroyed when its parent is.
 *
 * @author Tanguy Krotoff
 */
class LOGGER_API LogWindow : public QMainWindow {
	Q_OBJECT
public:

	/**
	 * Creates the log viewer/log window.
	 */
	LogWindow(QWidget * parent);

	~LogWindow();

private slots:

	void retranslate();

	/**
	 * Opens a log file.
	 */
	void open();

	/**
	 * Saves current log file.
	 */
	void save();

	/**
	 * Clears/resets the log viewer.
	 */
	void clear();

	/**
	 * The play/pause button has been clicked by the user.
	 */
	void playPauseButtonClicked();

	/**
	 * Opens the source code file using the default text editor.
	 *
	 * The default text editor depends on the file extension.
	 * Internal implementation uses QDesktopServices::openUrl().
	 *
	 * @param index model index that references the source code file
	 */
	void openSourceFile(const QModelIndex & index);

private slots:

	void setEnabledLoggerOutput(QObject * object);

private:

	void setupUi();

	void populateActionCollection();

	QToolBar * _toolBar;

	QPushButton * _outputsButton;

	QMenu * _menuOutputs;

	LogModel * _model;

	QTreeView * _view;

	/**
	 * Latest row inserted, saved as a number.
	 */
	int _lastRow;
};

#endif	//LOGWINDOW_H
