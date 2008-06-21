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

#include "FileBrowserWidget.h"

#include "ui_FileBrowserWidget.h"

#include "SimpleDirModel.h"
#include "config/FileBrowserConfigWidget.h"

#include <quarkplayer/MainWindow.h>
#include <quarkplayer/FileExtensions.h>
#include <quarkplayer/config/Config.h>

#include <tkutil/TkIcon.h>
#include <tkutil/TkFileDialog.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

FileBrowserWidget::FileBrowserWidget(MainWindow * mainWindow)
	: QWidget(NULL) {

	_ui = new Ui::FileBrowserWidget();
	_ui->setupUi(this);

	Config & config = Config::instance();

	layout()->setMargin(0);
	layout()->setSpacing(0);
	_ui->browseButton->setIcon(TkIcon("document-open-folder"));
	connect(_ui->browseButton, SIGNAL(clicked()), SLOT(configure()));
	_ui->clearSearchButton->setIcon(TkIcon("edit-delete"));
	_ui->clearSearchButton->setEnabled(false);
	connect(_ui->clearSearchButton, SIGNAL(clicked()), _ui->searchLineEdit, SLOT(clear()));
	connect(_ui->searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(search(const QString &)));

	_mainWindow = mainWindow;

	QStringList nameFilters;
	foreach (QString ext, FileExtensions::multimedia()) {
		nameFilters << "*." + ext;
	}
	nameFilters << "*";
	_dirModel = new SimpleDirModel(nameFilters);

	_ui->treeView->setHeaderHidden(true);
	_ui->treeView->setModel(_dirModel);
	connect(&config, SIGNAL(valueChanged(const QString &, const QVariant &)),
		SLOT(musicDirChanged(const QString &, const QVariant &)));
	_ui->treeView->setRootIndex(_dirModel->index(config.musicDir()));
	_ui->treeView->setDragEnabled(true);
	connect(_ui->treeView, SIGNAL(doubleClicked(const QModelIndex &)),
		SLOT(doubleClicked(const QModelIndex &)));

	//Add to the main window
	QTabWidget * browserTabWidget = _mainWindow->browserTabWidget();
	browserTabWidget->addTab(this, tr("Files"));
}

FileBrowserWidget::~FileBrowserWidget() {
}

void FileBrowserWidget::doubleClicked(const QModelIndex & index) {
	QFileInfo fileInfo = _dirModel->fileInfo(index);
	//FIXME sometimes, QFileInfo gives us this pattern: C://... that MPlayer does not accept
	QString slashSlashBugFix = fileInfo.absoluteFilePath().replace("//", "/");
	_mainWindow->play(slashSlashBugFix);
}

void FileBrowserWidget::search(const QString & pattern) {
	qDebug() << __FUNCTION__ << pattern;

	QStringList nameFilters;
	if (pattern.isEmpty()) {
		nameFilters << "*";
		_ui->clearSearchButton->setEnabled(false);
	} else {
		nameFilters << "*" + pattern + "*";
		_ui->clearSearchButton->setEnabled(true);
	}

	_dirModel->setNameFilters(nameFilters);
	_ui->treeView->setRootIndex(_dirModel->index(Config::instance().musicDir()));
}

void FileBrowserWidget::configure() {
	Config & config = Config::instance();
	QString musicDir = TkFileDialog::getExistingDirectory(this, tr("Select a Directory"), config.musicDir());
	if (!musicDir.isEmpty()) {
		config.setValue(Config::MUSIC_DIR_KEY, musicDir);
	}
}

void FileBrowserWidget::musicDirChanged(const QString & key, const QVariant & value) {
	qDebug() << __FUNCTION__ << key << value;

	if (key == Config::MUSIC_DIR_KEY) {
		_ui->treeView->setRootIndex(_dirModel->index(Config::instance().musicDir()));
		_dirModel->refresh();
	}
}
