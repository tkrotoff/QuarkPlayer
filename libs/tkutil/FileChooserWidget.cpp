/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "FileChooserWidget.h"

#include "TkFileDialog.h"

#include <QtGui/QtGui>

#include <QtCore/QDebug>

FileChooserWidget::FileChooserWidget(QWidget * parent)
	: QWidget(parent) {

	QHBoxLayout * layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);

	//Path line edit
	_pathLineEdit = new QLineEdit(this);
	layout->addWidget(_pathLineEdit);
	QCompleter * completer = new QCompleter(this);
	completer->setModel(new QDirModel(completer));
	_pathLineEdit->setCompleter(completer);
	connect(_pathLineEdit, SIGNAL(returnPressed()),
		SLOT(returnPressed()));
	connect(_pathLineEdit, SIGNAL(textChanged(const QString &)),
		SLOT(textChanged(const QString &)));

	//Search button
	_searchButton = new QToolButton();
	layout->addWidget(_searchButton);
	_searchButton->setAutoRaise(true);
	connect(_searchButton, SIGNAL(clicked()), SLOT(search()));
}

FileChooserWidget::~FileChooserWidget() {
}

void FileChooserWidget::setDialogType(DialogType dialogType) {
	_dialogType = dialogType;
}

void FileChooserWidget::setFilter(const QString & filter) {
	_filter = filter;
}

void FileChooserWidget::search() {
	QString tmp;

	switch (_dialogType) {
	case DialogTypeFile:
		tmp = TkFileDialog::getOpenFileName(this, tr("Select a File"), _path, _filter);
		break;
	case DialogTypeDir:
		tmp = TkFileDialog::getExistingDirectory(this, tr("Select a Directory"), _path);
		break;
	default:
		qCritical() << __FUNCTION__ << "Error: unknown DialogType:" << _dialogType;
	}

	if (!tmp.isEmpty()) {
		_path = tmp;
		_pathLineEdit->setText(_path);
		emit pathChanged(_path);
	}
}

void FileChooserWidget::setSearchButtonIcon(const QIcon & icon) {
	_searchButton->setIcon(icon);
}

QString FileChooserWidget::path() const {
	return _pathLineEdit->text();
}

void FileChooserWidget::setPath(const QString & path) {
	_path = path;
	_pathLineEdit->setText(_path);
}

QLineEdit * FileChooserWidget::lineEdit() const {
	return _pathLineEdit;
}

void FileChooserWidget::returnPressed() {
	emit pathChanged(_path);
}

void FileChooserWidget::textChanged(const QString & text) {
	_path = text;
}
