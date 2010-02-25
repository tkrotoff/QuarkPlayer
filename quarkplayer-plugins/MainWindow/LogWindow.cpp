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

#include <TkUtil/TkFileDialog.h>
#include <TkUtil/TkAction.h>
#include <TkUtil/TkToolBar.h>
#include <TkUtil/LanguageChangeEventFilter.h>
#include <TkUtil/ActionCollection.h>

#include <QtGui/QtGui>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

LogWindow::LogWindow(QWidget * parent)
	: QMainWindow(parent) {

	populateActionCollection();

	setupUi();

	connect(ActionCollection::action("LogWindow.Save"), SIGNAL(triggered()), SLOT(saveText()));

	RETRANSLATE(this);
	retranslate();
}

LogWindow::~LogWindow() {
}

void LogWindow::setupUi() {
	resize(648, 482);

	_textEdit = new QTextEdit();
	setCentralWidget(_textEdit);

	_toolBar = new QToolBar();
	TkToolBar::setToolButtonStyle(_toolBar);
	_toolBar->addAction(ActionCollection::action("LogWindow.Save"));
	_toolBar->addAction(ActionCollection::action("LogWindow.Clear"));
	_toolBar->addAction(ActionCollection::action("LogWindow.Pause"));
	addToolBar(_toolBar);
}

void LogWindow::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("LogWindow.Save", new TkAction(app, QKeySequence::Save));
	ActionCollection::addAction("LogWindow.Clear", new QAction(app));
	ActionCollection::addAction("LogWindow.Pause", new QAction(app));
	//FIXME add level action
}

void LogWindow::retranslate() {
	setWindowTitle(tr("Message Log"));

	ActionCollection::action("LogWindow.Save")->setText(tr("&Save"));
	ActionCollection::action("LogWindow.Save")->setIcon(QIcon::fromTheme("document-save"));

	ActionCollection::action("LogWindow.Clear")->setText(tr("&Clear"));
	ActionCollection::action("LogWindow.Clear")->setIcon(QIcon::fromTheme("edit-clear"));

	ActionCollection::action("LogWindow.Pause")->setText(tr("&Pause"));
	ActionCollection::action("LogWindow.Pause")->setIcon(QIcon::fromTheme("media-playback-pause"));

	_toolBar->setWindowTitle(tr("toolBar"));
}

void LogWindow::setText(const QString & text) {
	_textEdit->setPlainText(text);
}

void LogWindow::appendText(const QString & text) {
	_textEdit->moveCursor(QTextCursor::End);
	_textEdit->insertPlainText(text);
}

void LogWindow::saveText() {
	QString fileName = TkFileDialog::getSaveFileName(
		this, tr("Choose a filename to save under"),
		"", tr("Logs") +" (*.log *.txt)");

	if (!fileName.isEmpty()) {
		if (QFileInfo(fileName).exists()) {
			int res = QMessageBox::question(this,
				tr("Confirm overwrite?"),
				tr("The file already exists.\n"
				"Do you want to overwrite?"),
				QMessageBox::Yes,
				QMessageBox::No,
				QMessageBox::NoButton);
			if (res == QMessageBox::No) {
				return;
			}
		}

		QFile file(fileName);
		if (file.open(QIODevice::WriteOnly)) {
			QTextStream stream(&file);
			stream << _textEdit->toPlainText();
			file.close();
		} else {
			//Error opening file
			qDebug() << __FUNCTION__ << "Error saving file:" << fileName;
			QMessageBox::warning(this,
				tr("Error saving file"),
				tr("The log couldn't be saved"),
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton);
		}
	}
}
