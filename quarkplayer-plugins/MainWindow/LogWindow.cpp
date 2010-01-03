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

#include "ui_LogWindow.h"

#include <TkUtil/TkFileDialog.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <QtGui/QTextEdit>
#include <QtGui/QMessageBox>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

LogWindow::LogWindow(QWidget * parent)
	: QDialog(parent) {

	_ui = new Ui::LogWindow();
	_ui->setupUi(this);

	connect(_ui->buttonBox, SIGNAL(accepted()), SLOT(saveText()));

	RETRANSLATE(this);
	retranslate();
}

LogWindow::~LogWindow() {
}

void LogWindow::retranslate() {
	_ui->retranslateUi(this);
}

void LogWindow::setText(const QString & text) {
	_ui->textEdit->setPlainText(text);
}

void LogWindow::appendText(const QString & text) {
	_ui->textEdit->moveCursor(QTextCursor::End);
	_ui->textEdit->insertPlainText(text);
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
			stream << _ui->textEdit->toPlainText();
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
