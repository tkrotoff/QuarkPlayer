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

#include "FileBrowserTreeView.h"
#include "FileSearchModel.h"

#include "config/FileBrowserConfigWidget.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/config/ConfigWindow.h>
#include <quarkplayer/PluginManager.h>

#include <filetypes/FileTypes.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(filebrowser, FileBrowserWidgetFactory);

PluginInterface * FileBrowserWidgetFactory::create(QuarkPlayer & quarkPlayer) const {
	return new FileBrowserWidget(quarkPlayer);
}

FileBrowserWidget::FileBrowserWidget(QuarkPlayer & quarkPlayer)
	: QWidget(NULL),
	PluginInterface(quarkPlayer) {

	_dirModel = NULL;
	_fileSearchModel = NULL;

	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);

	populateActionCollection();
	createToolBar();

	_treeView = new FileBrowserTreeView(quarkPlayer);
	layout->addWidget(_treeView);

	connect(&PluginManager::instance(), SIGNAL(allPluginsLoaded()),
		SLOT(loadDirModel()), Qt::QueuedConnection);

	connect(&quarkPlayer.mainWindow(), SIGNAL(configWindowCreated(ConfigWindow *)),
		SLOT(configWindowCreated(ConfigWindow *)));

	//Add to the main window
	_dockWidget = new QDockWidget();
	quarkPlayer.mainWindow().addBrowserDockWidget(_dockWidget);
	_dockWidget->setWidget(this);

	setMaximumSize(1.5 * sizeHint().width(), maximumSize().height());

	RETRANSLATE(this);
	retranslate();

	_clearSearchButton->setEnabled(false);
}

FileBrowserWidget::~FileBrowserWidget() {
}

void FileBrowserWidget::createToolBar() {
	_toolBar = new QToolBar(NULL);
	_toolBar->setIconSize(QSize(16, 16));
	layout()->addWidget(_toolBar);

	//Browse button
	QToolButton * browseButton = new QToolButton();
	browseButton->setAutoRaise(true);
	browseButton->setDefaultAction(ActionCollection::action("fileBrowserBrowse"));
	_toolBar->addWidget(browseButton);
	connect(browseButton, SIGNAL(clicked()), SLOT(configure()));

	//Search toolbar
	//Copy-paste from PlaylistWidget.cpp
	_searchLineEdit = new QLineEdit();
	_toolBar->addWidget(_searchLineEdit);
	_searchTimer = new QTimer(this);
	_searchTimer->setSingleShot(true);
	_searchTimer->setInterval(700);
	connect(_searchTimer, SIGNAL(timeout()), SLOT(search()));
	connect(_searchLineEdit, SIGNAL(returnPressed()), SLOT(search()));
	connect(_searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(searchChanged()));
	_clearSearchButton = new QToolButton();
	_clearSearchButton->setAutoRaise(true);
	_clearSearchButton->setDefaultAction(ActionCollection::action("fileBrowserClearSearch"));
	_toolBar->addWidget(_clearSearchButton);
	connect(_clearSearchButton, SIGNAL(clicked()), _searchLineEdit, SLOT(clear()));

	//New file browser button
	QToolButton * newFileBrowserButton = new QToolButton();
	newFileBrowserButton->setAutoRaise(true);
	newFileBrowserButton->setDefaultAction(ActionCollection::action("fileBrowserNew"));
	_toolBar->addWidget(newFileBrowserButton);
	connect(newFileBrowserButton, SIGNAL(clicked()), SLOT(createNewFileBrowserWidget()));
}

void FileBrowserWidget::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("fileBrowserClearSearch", new QAction(app));

	ActionCollection::addAction("fileBrowserBrowse", new QAction(app));

	ActionCollection::addAction("fileBrowserNew", new QAction(app));
}

QStringList FileBrowserWidget::nameFilters() const {
	static QStringList tmp;
	if (tmp.isEmpty()) {
		QStringList extensions(FileTypes::extensions(FileType::Video, FileType::Audio));
		extensions << FileTypes::extensions(FileType::Playlist);
		foreach (QString ext, extensions) {
			tmp << "*." + ext;
		}
	}
	return tmp;
}

void FileBrowserWidget::loadDirModel() {
	_dirModel = new QFileSystemModel(this);
	_dirModel->setNameFilters(nameFilters());
	_dirModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
	//_dirModel->setSorting(QDir::Name | QDir::DirsFirst);
	_dirModel->setReadOnly(true);
	_dirModel->setNameFilterDisables(false);

	_treeView->setHeaderHidden(true);

	_treeView->setModel(_dirModel);

	QHeaderView * header = _treeView->header();
	header->hideSection(1);
	header->hideSection(2);
	header->hideSection(3);
	header->setResizeMode(QHeaderView::ResizeToContents);
	header->setStretchLastSection(false);
	header->setVisible(false);

	connect(&Config::instance(), SIGNAL(valueChanged(const QString &, const QVariant &)),
		SLOT(musicDirChanged(const QString &, const QVariant &)));
	_treeView->setRootIndex(_dirModel->index(Config::instance().musicDir()));
	_dirModel->setRootPath(Config::instance().musicDir());
	_treeView->setDragEnabled(true);
}

void FileBrowserWidget::searchChanged() {
	_searchTimer->stop();
	_searchTimer->start();
}

void FileBrowserWidget::search() {
	_searchTimer->stop();
	QString pattern(_searchLineEdit->text().trimmed());
	_clearSearchButton->setEnabled(!pattern.isEmpty());
	if (pattern.isEmpty()) {
		_treeView->setRootIsDecorated(true);

		_treeView->setModel(_dirModel);
		_treeView->setRootIndex(_dirModel->index(Config::instance().musicDir()));

		if (_fileSearchModel) {
			_fileSearchModel->stop();
		}

	} else {
		QStatusBar * statusBar = quarkPlayer().mainWindow().statusBar();
		if (statusBar) {
			statusBar->showMessage(tr("Searching..."));
		}
		_treeView->setRootIsDecorated(false);

		if (!_fileSearchModel) {
			_fileSearchModel = new FileSearchModel(this);
			connect(_fileSearchModel, SIGNAL(searchFinished(int)),
				SLOT(searchFinished(int)));
		}

		_fileSearchModel->setIconProvider(_dirModel->iconProvider());
		_treeView->setModel(_fileSearchModel);

		QStringList extensions = FileTypes::extensions(FileType::Video, FileType::Audio);
		extensions << FileTypes::extensions(FileType::Playlist);

		QString tmp;
		foreach (QString word, pattern.split(' ')) {
			tmp += word + '|';
		}
		tmp.remove(tmp.lastIndexOf('|'), 1);
		tmp.prepend("(");
		tmp.append(")");
		qDebug() << __FUNCTION__ << tmp;

		_fileSearchModel->search(Config::instance().musicDir(),
				QRegExp(tmp, Qt::CaseInsensitive, QRegExp::RegExp2),
				extensions);
	}
}

void FileBrowserWidget::searchFinished(int timeElapsed) {
	QStatusBar * statusBar = quarkPlayer().mainWindow().statusBar();
	if (statusBar) {
		statusBar->showMessage(tr("Search finished:") + ' ' + QString::number((float) timeElapsed / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_fileSearchModel->rowCount()) + " " + tr("medias") + ")");
	}
}

void FileBrowserWidget::configure() {
	QString musicDir = TkFileDialog::getExistingDirectory(this, tr("Select a Directory"), Config::instance().musicDir());
	if (!musicDir.isEmpty()) {
		Config::instance().setValue(Config::MUSIC_DIR_KEY, musicDir);
	}
}

void FileBrowserWidget::musicDirChanged(const QString & key, const QVariant & value) {
	if (key == Config::MUSIC_DIR_KEY) {
		_treeView->setRootIndex(_dirModel->index(Config::instance().musicDir()));
	}
}

void FileBrowserWidget::createNewFileBrowserWidget() {
	new FileBrowserWidget(quarkPlayer());
}

void FileBrowserWidget::retranslate() {
	ActionCollection::action("fileBrowserClearSearch")->setText(tr("Clear Search"));
	ActionCollection::action("fileBrowserClearSearch")->setIcon(TkIcon("edit-delete"));

	_searchLineEdit->setToolTip(tr("Search files, use whitespaces to separate words"));
	QString pattern(_searchLineEdit->text().trimmed());
	_clearSearchButton->setEnabled(!pattern.isEmpty());

	ActionCollection::action("fileBrowserBrowse")->setText(tr("Change Directory"));
	ActionCollection::action("fileBrowserBrowse")->setIcon(TkIcon("document-open-folder"));

	ActionCollection::action("fileBrowserNew")->setText(tr("New File Browser Window"));
	ActionCollection::action("fileBrowserNew")->setIcon(TkIcon("window-new"));

	_toolBar->setMinimumSize(_toolBar->sizeHint());

	_dockWidget->setWindowTitle(tr("File Browser"));
}

void FileBrowserWidget::configWindowCreated(ConfigWindow * configWindow) {
	//Add to config window
	configWindow->addConfigWidget(new FileBrowserConfigWidget());
}
