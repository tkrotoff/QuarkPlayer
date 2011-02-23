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

#include "LogWindow.h"

#include "Logger.h"
#include "LogModel.h"
#include "LogMessage.h"
#include "LoggerOutput.h"
#include "LoggerLogger.h"

#include <TkUtil/TkFileDialog.h>
#include <TkUtil/TkAction.h>
#include <TkUtil/TkToolBar.h>
#include <TkUtil/LanguageChangeEventFilter.h>
#include <TkUtil/Actions.h>

#include <QtGui/QtGui>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QFileInfo>

/**
 * An ItemDelegate which colors log message text.
 *
 * For example, critical log messages will appear in red.
 * Warning messages will appear in orange.
 *
 * @author Tanguy Krotoff
 */
class LogItemDelegate : public QStyledItemDelegate {
public:

	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
		QStyleOptionViewItem myOption(option);

		const LogModel * model = dynamic_cast<const LogModel *>(index.model());
		Q_ASSERT(model);

		LogMessage msg = model->logMessage(index);
		QtMsgType type = msg.type;
		switch (type) {
		case QtDebugMsg:
			//Do nothing, let it be black
			break;
		case QtWarningMsg:
			//Orange
			myOption.palette.setColor(QPalette::Text, QColor(255, 165, 0));
			break;
		case QtCriticalMsg:
			myOption.palette.setColor(QPalette::Text, Qt::red);
			break;
		case QtFatalMsg:
			myOption.palette.setColor(QPalette::Text, Qt::red);
			break;
		}

		QStyledItemDelegate::paint(painter, myOption, index);
	}
};


LogWindow::LogWindow(QWidget * parent)
	: QMainWindow(parent, Qt::Dialog) {

	_model = Logger::instance().logModel();

	populateActionCollection();

	setupUi();

	connect(Actions::get("LogWindow.Open"), SIGNAL(triggered()), SLOT(open()));
	connect(Actions::get("LogWindow.Save"), SIGNAL(triggered()), SLOT(save()));
	connect(Actions::get("LogWindow.Clear"), SIGNAL(triggered()), SLOT(clear()));
	connect(Actions::get("LogWindow.PlayPause"), SIGNAL(triggered()), SLOT(playPauseButtonClicked()));

	RETRANSLATE(this);
	retranslate();
}

LogWindow::~LogWindow() {
}

void LogWindow::setupUi() {
	resize(650, 450);

	_view = new QTreeView();
	_view->setModel(_model);
	_view->setItemDelegate(new LogItemDelegate());
	_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	_view->setRootIsDecorated(false);
	_view->setAllColumnsShowFocus(true);
	//_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	connect(_view, SIGNAL(activated(const QModelIndex &)),
		SLOT(openSourceFile(const QModelIndex &)));

	connect(_model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
		_view, SLOT(scrollToBottom()));

	//Default rows and columns sizes
	_view->resizeColumnToContents(LogModel::COLUMN_TIME);
	_view->hideColumn(LogModel::COLUMN_TYPE);
	_view->hideColumn(LogModel::COLUMN_FILE);
	_view->hideColumn(LogModel::COLUMN_LINE);
	_view->resizeColumnToContents(LogModel::COLUMN_MODULE);
	_view->resizeColumnToContents(LogModel::COLUMN_FUNCTION);
	_view->setColumnWidth(LogModel::COLUMN_MESSAGE, 400);
	//_view->resizeColumnsToContents();
	_view->setUniformRowHeights(true);
	//_view->resizeRowsToContents();
	///

#ifdef HACK_DATA_CHANGED
	//FIXME there is a Qt bug here:
	//LogWindow is created after LogModel
	//and thus LogWindow does not get updated
	//Using dataChanged() we force LogWindow
	//to be updated
	//This bug occurs with QTreeView not with QTableView
	QModelIndex topLeft = _model->index(0, LogModel::COLUMN_TIME);
	QModelIndex bottomRight = _model->index(_model->rowCount(), _model->columnCount());
	_view->dataChanged(topLeft, bottomRight);
	///
#endif	//HACK_DATA_CHANGED

	_view->scrollToBottom();

	setCentralWidget(_view);

	_toolBar = new QToolBar();
	TkToolBar::setToolButtonStyle(_toolBar);
	_toolBar->addAction(Actions::get("LogWindow.Open"));
	_toolBar->addAction(Actions::get("LogWindow.Save"));
	_toolBar->addAction(Actions::get("LogWindow.Clear"));
	_toolBar->addAction(Actions::get("LogWindow.PlayPause"));

	_outputsButton = new QPushButton();
	_menuOutputs = new QMenu();
	_outputsButton->setMenu(_menuOutputs);
	_toolBar->addWidget(_outputsButton);

	//Populate the outputs menu
	QSignalMapper * signalMapper = new QSignalMapper(this);
	QList<LoggerOutput *> & outputs = Logger::instance().outputs();
	foreach (LoggerOutput * output, outputs) {
		QAction * action = _menuOutputs->addAction(output->name(),
						signalMapper, SLOT(map()));
		signalMapper->setMapping(action, output);
		action->setCheckable(true);
		action->setChecked(output->isEnabled());
	}
	connect(signalMapper, SIGNAL(mapped(QObject *)),
		SLOT(setEnabledLoggerOutput(QObject *)));
	///

	addToolBar(_toolBar);
}

void LogWindow::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	Q_ASSERT(app);

	Actions::add("LogWindow.Open", new TkAction(app, QKeySequence::Open));
	Actions::add("LogWindow.Save", new TkAction(app, QKeySequence::Save));
	Actions::add("LogWindow.Clear", new QAction(app));
	Actions::add("LogWindow.PlayPause", new QAction(app));
	//TODO add level action
}

void LogWindow::retranslate() {
	setWindowTitle(tr("Message Log"));

	Actions::get("LogWindow.Open")->setText(tr("&Open"));
	Actions::get("LogWindow.Open")->setIcon(QIcon::fromTheme("document-open"));

	Actions::get("LogWindow.Save")->setText(tr("&Save"));
	Actions::get("LogWindow.Save")->setIcon(QIcon::fromTheme("document-save"));

	Actions::get("LogWindow.Clear")->setText(tr("&Clear"));
	Actions::get("LogWindow.Clear")->setIcon(QIcon::fromTheme("edit-clear"));

	QAction * action = Actions::get("LogWindow.PlayPause");
	LogModel::State state = _model->state();
	switch (state) {
	case LogModel::PlayingState:
		action->setText(tr("&Pause"));
		action->setIcon(QIcon::fromTheme("media-playback-pause"));
		break;
	case LogModel::PausedState:
		action->setText(tr("&Play"));
		action->setIcon(QIcon::fromTheme("media-playback-start"));
		break;
	default:
		LoggerCritical() << "Unknown state:" << state;
	}

	_outputsButton->setText(tr("Outputs"));

	_toolBar->setWindowTitle(tr("ToolBar"));
}

void LogWindow::open() {
	QString fileName = TkFileDialog::getOpenFileName(
		this, tr("Select Log File"), QDir::homePath(),
		tr("Logs") + " (*.log *.xml *.txt)" + ";;" +
		tr("All Files") + " (*)"
	);

	if (!fileName.isEmpty()) {
		bool success = _model->open(fileName);
		if (!success) {
			//Error opening file
			LoggerDebug() << "Error opening file:" << fileName;
			QMessageBox::warning(this,
				tr("Error opening file"),
				tr("The log couldn't be opened"),
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton);
		}
	}
}

void LogWindow::save() {
	QString fileName = TkFileDialog::getSaveFileName(
		this, tr("Save Log Messages"), QDir::homePath(),
		tr("Logs") + " (*.log *.xml *.txt)"
	);

	if (!fileName.isEmpty()) {
		bool success = _model->save(fileName);
		if (!success) {
			//Error opening file
			LoggerDebug() << "Error saving file:" << fileName;
			QMessageBox::warning(this,
				tr("Error saving file"),
				tr("The log couldn't be saved"),
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton);
		}
	}
}

void LogWindow::clear() {
	_model->clear();
}

void LogWindow::playPauseButtonClicked() {
	LogModel::State state = _model->state();

	switch (state) {
	case LogModel::PlayingState:
		_model->pause();
		break;
	case LogModel::PausedState:
		_model->resume();
		break;
	default:
		LoggerCritical() << "Unknown state:" << state;
	}

	//Change playPauseButton icon & text
	retranslate();
}

void LogWindow::openSourceFile(const QModelIndex & index) {
	LogMessage msg = _model->logMessage(index);
	QString file = msg.file;
	if (!file.isEmpty()) {
		bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(file));
		if (!success) {
			LoggerWarning() << "QDesktopServices::openUrl() failed with file:" << file;
		}
	} else {
		LoggerWarning() << "No source code file";
	}
}

void LogWindow::setEnabledLoggerOutput(QObject * object) {
	LoggerOutput * output = qobject_cast<LoggerOutput *>(object);
	output->setEnabled(!output->isEnabled());
}
