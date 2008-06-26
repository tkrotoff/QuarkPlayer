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

#include "FileBrowserConfigWidget.h"

#include "ui_FileBrowserConfigWidget.h"

#include <quarkplayer/config/Config.h>

#include <tkutil/TkIcon.h>
#include <tkutil/TkFileDialog.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

FileBrowserConfigWidget::FileBrowserConfigWidget() {
	_ui = new Ui::FileBrowserConfigWidget();
	_ui->setupUi(this);

	QCompleter * completer = new QCompleter(this);
	completer->setModel(new QDirModel(completer));
	_ui->musicDirLineEdit->setCompleter(completer);
	_ui->browseButton->setIcon(TkIcon("document-open-folder"));
	connect(_ui->browseButton, SIGNAL(clicked()),
		SLOT(browseDir()));
}

FileBrowserConfigWidget::~FileBrowserConfigWidget() {
	delete _ui;
}

QString FileBrowserConfigWidget::name() const {
	return tr("File Browser Plugin");
}

QString FileBrowserConfigWidget::iconName() const {
	return "preferences-plugin";
}

void FileBrowserConfigWidget::saveConfig() {
	Config & config = Config::instance();

	//musicDir
	QString musicDir = _ui->musicDirLineEdit->text();
	config.setValue(Config::MUSIC_DIR_KEY, musicDir);
}

void FileBrowserConfigWidget::readConfig() {
	Config & config = Config::instance();

	//musicDir
	_ui->musicDirLineEdit->setText(config.musicDir());
}

void FileBrowserConfigWidget::retranslate() {
	_ui->retranslateUi(this);
}

void FileBrowserConfigWidget::browseDir() {
	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select a Directory"), Config::instance().musicDir());
	_ui->musicDirLineEdit->setText(dir);
}
