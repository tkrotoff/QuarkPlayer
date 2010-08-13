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

#include "LogWindow.h"

#include "MainWindowLogger.h"

#include <quarkplayer/LogMessageHandler.h>
#include <quarkplayer/LogModel.h>
#include <quarkplayer/LogMessage.h>

#include <TkUtil/TkFileDialog.h>
#include <TkUtil/TkAction.h>
#include <TkUtil/TkToolBar.h>
#include <TkUtil/LanguageChangeEventFilter.h>
#include <TkUtil/ActionCollection.h>

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

	_model = LogMessageHandler::instance().logModel();
	_playMode = true;

	populateActionCollection();

	setupUi();

	connect(ActionCollection::action("LogWindow.Open"), SIGNAL(triggered()), SLOT(open()));
	connect(ActionCollection::action("LogWindow.Save"), SIGNAL(triggered()), SLOT(save()));
	connect(ActionCollection::action("LogWindow.Clear"), SIGNAL(triggered()), SLOT(clear()));
	connect(ActionCollection::action("LogWindow.PlayPause"), SIGNAL(triggered()), SLOT(playPauseButtonClicked()));

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

	_view->scrollToBottom();

	setCentralWidget(_view);

	_toolBar = new QToolBar();
	TkToolBar::setToolButtonStyle(_toolBar);
	_toolBar->addAction(ActionCollection::action("LogWindow.Open"));
	_toolBar->addAction(ActionCollection::action("LogWindow.Save"));
	_toolBar->addAction(ActionCollection::action("LogWindow.Clear"));
	_toolBar->addAction(ActionCollection::action("LogWindow.PlayPause"));
	addToolBar(_toolBar);
}

void LogWindow::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	Q_ASSERT(app);

	ActionCollection::addAction("LogWindow.Open", new TkAction(app, QKeySequence::Open));
	ActionCollection::addAction("LogWindow.Save", new TkAction(app, QKeySequence::Save));
	ActionCollection::addAction("LogWindow.Clear", new QAction(app));
	ActionCollection::addAction("LogWindow.PlayPause", new QAction(app));
	//TODO add level action
}

void LogWindow::retranslate() {
	setWindowTitle(tr("Message Log"));

	ActionCollection::action("LogWindow.Open")->setText(tr("&Open"));
	ActionCollection::action("LogWindow.Open")->setIcon(QIcon::fromTheme("document-open"));

	ActionCollection::action("LogWindow.Save")->setText(tr("&Save"));
	ActionCollection::action("LogWindow.Save")->setIcon(QIcon::fromTheme("document-save"));

	ActionCollection::action("LogWindow.Clear")->setText(tr("&Clear"));
	ActionCollection::action("LogWindow.Clear")->setIcon(QIcon::fromTheme("edit-clear"));

	QAction * action = ActionCollection::action("LogWindow.PlayPause");
	Q_ASSERT(action);
	if (_playMode) {
		action->setText(tr("&Pause"));
		action->setIcon(QIcon::fromTheme("media-playback-pause"));
	} else {
		action->setText(tr("&Play"));
		action->setIcon(QIcon::fromTheme("media-playback-start"));
	}

	_toolBar->setWindowTitle(tr("ToolBar"));
}

void LogWindow::open() {
	QString fileName = TkFileDialog::getOpenFileName(
		this, tr("Select Log File"), QDir::homePath(),
		tr("Logs") + " (*.log *.txt)" + ";;" +
		tr("All Files") + " (*)"
	);

	if (!fileName.isEmpty()) {
		bool success = _model->open(fileName);
		if (!success) {
			//Error opening file
			MainWindowDebug() << "Error opening file:" << fileName;
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
		tr("Logs") + " (*.log *.txt)"
	);

	if (!fileName.isEmpty()) {
		bool success = _model->save(fileName);
		if (!success) {
			//Error opening file
			MainWindowDebug() << "Error saving file:" << fileName;
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
	_playMode = !_playMode;

	if (_playMode) {
		_view->setModel(_model);
	} else {
		_view->setModel(NULL);
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
			MainWindowWarning() << "QDesktopServices::openUrl() failed with file:" << file;
		}
	} else {
		MainWindowWarning() << "No source code file";
	}
}
