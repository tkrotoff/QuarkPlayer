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

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/FileExtensions.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/config/ConfigWindow.h>

#include <tkutil/TkIcon.h>
#include <tkutil/TkFileDialog.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(filebrowser, FileBrowserWidgetFactory);

PluginInterface * FileBrowserWidgetFactory::create(QuarkPlayer & quarkPlayer) const {
	_plugin = new FileBrowserWidget(quarkPlayer);
	return _plugin;
}

FileBrowserWidgetFactory::~FileBrowserWidgetFactory() {
	qDebug() << __FUNCTION__ << "Plugin unloaded";

	delete _plugin;
}

FileBrowserWidget::FileBrowserWidget(QuarkPlayer & quarkPlayer)
	: QWidget(NULL),
	PluginInterface(quarkPlayer) {

	_ui = new Ui::FileBrowserWidget();
	_ui->setupUi(this);

	layout()->setMargin(0);
	layout()->setSpacing(0);
	_ui->browseButton->setIcon(TkIcon("document-open-folder"));
	connect(_ui->browseButton, SIGNAL(clicked()), SLOT(configure()));
	_ui->clearSearchButton->setIcon(TkIcon("edit-delete"));
	_ui->clearSearchButton->setEnabled(false);
	connect(_ui->clearSearchButton, SIGNAL(clicked()), _ui->searchLineEdit, SLOT(clear()));
	connect(_ui->searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(search(const QString &)));

	QStringList nameFilters;
	/*foreach (QString ext, FileExtensions::multimedia()) {
		nameFilters << "*." + ext;
	}*/
	nameFilters << "*";
	_dirModel = new SimpleDirModel(nameFilters);
	_ui->treeView->setModel(_dirModel);
	_ui->treeView->setHeaderHidden(true);

	QHeaderView * header = _ui->treeView->header();
	header->hideSection(1);
	header->hideSection(2);
	header->hideSection(3);
	header->setResizeMode(QHeaderView::ResizeToContents);
	header->setStretchLastSection(false);
	header->setVisible(false);

	connect(&(quarkPlayer.config()), SIGNAL(valueChanged(const QString &, const QVariant &)),
		SLOT(musicDirChanged(const QString &, const QVariant &)));
	_ui->treeView->setRootIndex(_dirModel->index(quarkPlayer.config().musicDir()));
	_ui->treeView->setDragEnabled(true);
	connect(_ui->treeView, SIGNAL(doubleClicked(const QModelIndex &)),
		SLOT(doubleClicked(const QModelIndex &)));

	//Add to config window
	ConfigWindow::addConfigWidget(new FileBrowserConfigWidget());

	//Add to the main window
	_dockWidget = new QDockWidget(tr("Local Files"));
	quarkPlayer.mainWindow().addBrowserDockWidget(_dockWidget);
	_dockWidget->setWidget(this);
}

FileBrowserWidget::~FileBrowserWidget() {
	qDebug() << __FUNCTION__ << "Unloading plugin";
	quarkPlayer().mainWindow().removeBrowserDockWidget(_dockWidget);
	_dockWidget->close();
	close();
	//delete _dockWidget;
	//delete _ui;
}

void FileBrowserWidget::doubleClicked(const QModelIndex & index) {
	QFileInfo fileInfo = _dirModel->fileInfo(index);
	if (fileInfo.isFile()) {
		//FIXME sometimes, QFileInfo gives us this pattern: C://... that MPlayer does not accept
		QString slashSlashBugFix = fileInfo.absoluteFilePath().replace("//", "/");
		quarkPlayer().play(slashSlashBugFix);
	}
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
	_ui->treeView->setRootIndex(_dirModel->index(quarkPlayer().config().musicDir()));
}

void FileBrowserWidget::configure() {
	QString musicDir = TkFileDialog::getExistingDirectory(this, tr("Select a Directory"), quarkPlayer().config().musicDir());
	if (!musicDir.isEmpty()) {
		quarkPlayer().config().setValue(Config::MUSIC_DIR_KEY, musicDir);
	}
}

void FileBrowserWidget::musicDirChanged(const QString & key, const QVariant & value) {
	qDebug() << __FUNCTION__ << key << value;

	if (key == Config::MUSIC_DIR_KEY) {
		_ui->treeView->setRootIndex(_dirModel->index(quarkPlayer().config().musicDir()));
		_dirModel->refresh();
	}
}
