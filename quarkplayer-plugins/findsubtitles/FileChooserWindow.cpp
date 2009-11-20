/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "FileChooserWindow.h"

#include "ui_FileChooserWindow.h"

#include <QtGui/QtGui>

FileChooserWindow::FileChooserWindow(QWidget * parent)
	: QDialog(parent) {

	_currentItemCheckState = Qt::Checked;

	_ui = new Ui::FileChooserWindow();
	_ui->setupUi(this);

	_ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);

	connect(_ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)),
		SLOT(itemClicked(QListWidgetItem *)));
	connect(_ui->listWidget, SIGNAL(itemPressed(QListWidgetItem *)),
		SLOT(itemPressed(QListWidgetItem *)));
}

FileChooserWindow::~FileChooserWindow() {
}

void FileChooserWindow::enableOkButton() {
	QPushButton * okButton = _ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setEnabled(true);
}

void FileChooserWindow::disableOkButton() {
	QPushButton * okButton = _ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setEnabled(false);
}

void FileChooserWindow::itemClicked(QListWidgetItem * item) {
	if (item->checkState() == _currentItemCheckState) {
		//Clicked on the list item (not checkbox)
		if (item->checkState() == Qt::Checked) {
			item->setCheckState(Qt::Unchecked);
		} else {
			item->setCheckState(Qt::Checked);
		}
	}
	//else - clicked on the checkbox itself, do nothing

	//Compute the number of uncheck files
	int nbItemsUnchecked = 0;
	for (int i = 0; i < _ui->listWidget->count(); i++) {
		QListWidgetItem * item = _ui->listWidget->item(i);
		if (item) {
			if (item->checkState() != Qt::Checked) {
				nbItemsUnchecked++;
			}
		}
	}
	int nbItems = _ui->listWidget->count();
	if (nbItemsUnchecked == nbItems) {
		disableOkButton();
	} else {
		enableOkButton();
	}
	///
}

void FileChooserWindow::itemPressed(QListWidgetItem * item) {
	_currentItemCheckState = item->checkState();
}

void FileChooserWindow::addFiles(const QStringList & files) {
	QFileIconProvider iconProvider;
	QIcon fileIcon = iconProvider.icon(QFileIconProvider::File);
	foreach (QString fileName, files) {
		QListWidgetItem * item = new QListWidgetItem(_ui->listWidget);
		item->setText(fileName);
		item->setIcon(fileIcon);
		item->setCheckState(Qt::Checked);
	}
}

QStringList FileChooserWindow::selectedFiles() const {
	QStringList files;

	for (int i = 0; i < _ui->listWidget->count(); i++) {
		QListWidgetItem * item = _ui->listWidget->item(i);
		if (item && item->checkState() == Qt::Checked) {
			files.append(item->text());
		}
	}

	return files;
}

void FileChooserWindow::setInformationText(const QString & text) {
	_ui->informationLabel->setText(text);
}

void FileChooserWindow::setOkButtonText(const QString & text) {
	QPushButton * okButton = _ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setText(text);
}
