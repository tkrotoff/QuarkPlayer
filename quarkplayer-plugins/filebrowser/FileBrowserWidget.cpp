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
#include "UuidActionCollection.h"

#include "config/FileBrowserConfigWidget.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/config/ConfigWindow.h>
#include <quarkplayer/PluginsManager.h>

#include <filetypes/FileTypes.h>

#include <tkutil/TkIcon.h>
#include <tkutil/SearchLineEdit.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

static const char * FILEBROWSER_SEARCH_HISTORY_KEY = "filebrowser_search_history";

Q_EXPORT_PLUGIN2(filebrowser, FileBrowserWidgetFactory);

PluginInterface * FileBrowserWidgetFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new FileBrowserWidget(quarkPlayer, uuid);
}

FileBrowserWidget::FileBrowserWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QWidget(quarkPlayer.mainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	_dirModel = NULL;
	_fileSearchModel = NULL;

	//Short for UuidActionCollection::setUuid()
	setUuid(uuid);
	///

	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);

	populateActionCollection();
	createToolBar();

	//TreeView
	_treeView = new FileBrowserTreeView(quarkPlayer);
	layout->addWidget(_treeView);

	//Add to the main window
	_dockWidget = new QDockWidget();
	quarkPlayer.mainWindow()->addBrowserDockWidget(_dockWidget);
	_dockWidget->setWidget(this);

	setMaximumSize(1.5 * sizeHint().width(), maximumSize().height());

	if (PluginsManager::instance().allPluginsAlreadyLoaded()) {
		loadDirModel();
	} else {
		connect(&PluginsManager::instance(), SIGNAL(allPluginsLoaded()),
			SLOT(loadDirModel()), Qt::QueuedConnection);
	}

	ConfigWindow * configWindow = quarkPlayer.mainWindow()->configWindow();
	if (configWindow) {
		configWindowCreated(configWindow);
	} else {
		connect(quarkPlayer.mainWindow(), SIGNAL(configWindowCreated(ConfigWindow *)),
			SLOT(configWindowCreated(ConfigWindow *)));
	}

	Config::instance().addKey(FILEBROWSER_SEARCH_HISTORY_KEY, QStringList());

	RETRANSLATE(this);
	retranslate();
}

FileBrowserWidget::~FileBrowserWidget() {
	quarkPlayer().mainWindow()->removeDockWidget(_dockWidget);
	quarkPlayer().mainWindow()->resetBrowserDockWidget();
}

void FileBrowserWidget::createToolBar() {
	QToolBar * toolBar = new QToolBar(NULL);
	toolBar->setIconSize(QSize(16, 16));
	layout()->addWidget(toolBar);

	//Browse button
	QToolButton * browseButton = new QToolButton();
	browseButton->setAutoRaise(true);
	browseButton->setDefaultAction(uuidAction("fileBrowserBrowse"));
	toolBar->addWidget(browseButton);
	connect(browseButton, SIGNAL(clicked()), SLOT(configure()));

	//Search line edit
	QStringList history = Config::instance().value(FILEBROWSER_SEARCH_HISTORY_KEY).toStringList();
	_searchLineEdit = new SearchLineEdit(history, toolBar);
	connect(_searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(search()));
	toolBar->addWidget(_searchLineEdit);

	//New file browser button
	QToolButton * newFileBrowserButton = new QToolButton();
	newFileBrowserButton->setAutoRaise(true);
	newFileBrowserButton->setDefaultAction(uuidAction("fileBrowserNew"));
	toolBar->addWidget(newFileBrowserButton);
	connect(newFileBrowserButton, SIGNAL(clicked()), SLOT(createNewFileBrowserWidget()));
}

void FileBrowserWidget::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	addUuidAction("fileBrowserBrowse", new QAction(app));

	addUuidAction("fileBrowserNew", new QAction(app));
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
	_treeView->setRootIndex(_dirModel->index(Config::instance().musicDir(uuid())));
	_dirModel->setRootPath(Config::instance().musicDir(uuid()));
	_treeView->setDragEnabled(true);
}

void FileBrowserWidget::search() {
	QString pattern(_searchLineEdit->text());
	if (pattern.isEmpty()) {
		setWindowTitle(QString());
		_treeView->setRootIsDecorated(true);

		_treeView->setModel(_dirModel);
		_treeView->setRootIndex(_dirModel->index(Config::instance().musicDir(uuid())));

		if (_fileSearchModel) {
			_fileSearchModel->stop();
		}

	} else {
		//Sets a busy mouse cursor since the search can take several seconds
		QApplication::restoreOverrideCursor();
		QApplication::setOverrideCursor(Qt::BusyCursor);

		setWindowTitle(tr("Searching..."));
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
		QStringList words(pattern.split(' '));
		for (int i = 0; i < words.size(); i++) {
			tmp += '(' + words[i] + ')';
			if (i < words.size() - 1) {
				tmp += ".*";
			}
		}
		qDebug() << __FUNCTION__ << tmp;

		_fileSearchModel->search(Config::instance().musicDir(uuid()),
				QRegExp(tmp, Qt::CaseInsensitive, QRegExp::RegExp2),
				extensions);
	}
}

void FileBrowserWidget::searchFinished(int timeElapsed) {
	//Restores the mouse cursor from busy to normal
	QApplication::restoreOverrideCursor();

	setWindowTitle(tr("Search finished:") + ' ' + QString::number((float) timeElapsed / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_fileSearchModel->rowCount()) + " " + tr("medias") + ")");

	//Add the word searched inside the SearchToolBar
	QString word = _searchLineEdit->text();
	if (word.size() > 2) {
		_searchLineEdit->addWord(word);
		QStringList wordList = _searchLineEdit->wordList();
		QStringList tmp;
		//Only takes the first 100 words from the list and save them
		for (int i = 0; i < wordList.size() && i < 100; i++) {
			tmp += wordList[i];
		}
		Config::instance().setValue(FILEBROWSER_SEARCH_HISTORY_KEY, tmp);
	}
}

void FileBrowserWidget::configure() {
	QString musicDir = TkFileDialog::getExistingDirectory(this, tr("Select a Directory"),
			Config::instance().musicDir(uuid()));
	musicDir = QDir::toNativeSeparators(musicDir);
	if (!musicDir.isEmpty()) {
		Config::instance().addMusicDir(musicDir, uuid());
	}
}

void FileBrowserWidget::musicDirChanged(const QString & key, const QVariant & value) {
	if (key == Config::MUSIC_DIR_KEY + uuid().toString()) {
		_treeView->setRootIndex(_dirModel->index(Config::instance().musicDir(uuid())));
		setWindowTitle(QString());
	}
}

void FileBrowserWidget::createNewFileBrowserWidget() {
	PluginData pluginData = PluginsManager::instance().pluginData(uuid());
	PluginsManager::instance().loadDisabledPlugin(pluginData);
}

void FileBrowserWidget::retranslate() {
	_searchLineEdit->clearButton()->setToolTip(tr("Clear Search"));
	_searchLineEdit->clearButton()->setIcon(TkIcon("edit-clear-locationbar-rtl"));

	_searchLineEdit->showWordListButton()->setToolTip(tr("Previous Searches"));
	_searchLineEdit->showWordListButton()->setIcon(TkIcon("go-down-search"));

	_searchLineEdit->setToolTip(tr("Search files, use whitespaces to separate words"));
	_searchLineEdit->setClickMessage(tr("Enter search terms here"));

	uuidAction("fileBrowserBrowse")->setText(tr("Change Directory"));
	uuidAction("fileBrowserBrowse")->setIcon(TkIcon("document-open-folder"));

	uuidAction("fileBrowserNew")->setText(tr("New File Browser Window"));
	uuidAction("fileBrowserNew")->setIcon(TkIcon("window-new"));

	setWindowTitle(QString());
}

void FileBrowserWidget::setWindowTitle(const QString & statusMessage) {
	if (statusMessage.isEmpty()) {
		_dockWidget->setWindowTitle(Config::instance().musicDir(uuid()));
	} else {
		_dockWidget->setWindowTitle(statusMessage);
		QStatusBar * statusBar = quarkPlayer().mainWindow()->statusBar();
		if (statusBar) {
			statusBar->showMessage(statusMessage);
		}
	}
}

void FileBrowserWidget::configWindowCreated(ConfigWindow * configWindow) {
	//Add to config window
	configWindow->addConfigWidget(new FileBrowserConfigWidget(uuid()));
}
