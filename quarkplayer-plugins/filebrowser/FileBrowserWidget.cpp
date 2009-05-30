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

#include "FileBrowserWidget.h"

#include "FileBrowserTreeView.h"
#include "FileSearchModel.h"

#include "config/FileBrowserConfigWidget.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/config/ConfigWindow.h>
#include <quarkplayer/PluginsManager.h>

#include <quarkplayer-plugins/mainwindow/MainWindow.h>

#include <filetypes/FileTypes.h>

#include <tkutil/TkIcon.h>
#include <tkutil/SearchLineEdit.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <modeltest/modeltest.h>

#include <QtGui/QVBoxLayout>
#include <QtGui/QDockWidget>
#include <QtGui/QToolButton>
#include <QtGui/QApplication>
#include <QtGui/QAction>
#include <QtGui/QStatusBar>
#include <QtGui/QHeaderView>

#include <QtCore/QtPlugin>
#include <QtCore/QDebug>

static const char * FILEBROWSER_SEARCH_HISTORY_KEY = "filebrowser_search_history";

Q_EXPORT_PLUGIN2(filebrowser, FileBrowserWidgetFactory);

QString FileBrowserWidgetFactory::pluginName() const {
	return "filebrowser";
}

QStringList FileBrowserWidgetFactory::dependencies() const {
	QStringList tmp;
	tmp += "mainwindow";
	return tmp;
}

PluginInterface * FileBrowserWidgetFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new FileBrowserWidget(quarkPlayer, uuid);
}

static MainWindow * getMainWindow() {
	return dynamic_cast<MainWindow *>(PluginsManager::instance().pluginInterface("mainwindow"));
}

FileBrowserWidget::FileBrowserWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QWidget(getMainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	_fileSearchModel = NULL;

	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);

	populateActionCollection();
	createToolBar();

	//TreeView
	_treeView = new FileBrowserTreeView(this);
	layout->addWidget(_treeView);

	//Add to the main window
	_dockWidget = new QDockWidget();
	getMainWindow()->addBrowserDockWidget(_dockWidget);
	_dockWidget->setWidget(this);

	setMaximumSize(static_cast<int>(1.5 * sizeHint().width()), maximumSize().height());

	if (PluginsManager::instance().allPluginsAlreadyLoaded()) {
		loadDirModel();
	} else {
		connect(&PluginsManager::instance(), SIGNAL(allPluginsLoaded()),
			SLOT(loadDirModel()), Qt::QueuedConnection);
	}

	ConfigWindow * configWindow = getMainWindow()->configWindow();
	if (configWindow) {
		configWindowCreated(configWindow);
	} else {
		connect(getMainWindow(), SIGNAL(configWindowCreated(ConfigWindow *)),
			SLOT(configWindowCreated(ConfigWindow *)));
	}

	if (!Config::instance().contains(FILEBROWSER_SEARCH_HISTORY_KEY)) {
		Config::instance().addKey(FILEBROWSER_SEARCH_HISTORY_KEY, QStringList());
	}

	RETRANSLATE(this);
	retranslate();
}

FileBrowserWidget::~FileBrowserWidget() {
	getMainWindow()->removeDockWidget(_dockWidget);
	getMainWindow()->resetBrowserDockWidget();
}

void FileBrowserWidget::createToolBar() {
	QToolBar * toolBar = new QToolBar(NULL);
	toolBar->setIconSize(QSize(16, 16));
	layout()->addWidget(toolBar);

	//Browse button
	QToolButton * browseButton = new QToolButton();
	browseButton->setAutoRaise(true);
	browseButton->setDefaultAction(uuidAction("FileBrowser.Browse"));
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
	newFileBrowserButton->setDefaultAction(uuidAction("FileBrowser.New"));
	toolBar->addWidget(newFileBrowserButton);
	connect(newFileBrowserButton, SIGNAL(clicked()), SLOT(createNewFileBrowserWidget()));
}

void FileBrowserWidget::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	addUuidAction("FileBrowser.Browse", new QAction(app));

	addUuidAction("FileBrowser.New", new QAction(app));
}

void FileBrowserWidget::loadDirModel() {
	_fileSearchModel = new FileSearchModel(this);
	//new ModelTest(_fileSearchModel, this);

	//Shows only tooltips for files that have a "multimedia" extension (i.e .mp3, .avi, .flac...)
	QStringList extensions;
	extensions << FileTypes::extensions(FileType::Video);
	extensions << FileTypes::extensions(FileType::Audio);
	_fileSearchModel->setToolTipExtensions(extensions);

	//Shows only files that can be handled by QuarkPlayer (i.e .mp3, .avi, .flac...)
	extensions.clear();
	extensions << FileTypes::extensions(FileType::Video);
	extensions << FileTypes::extensions(FileType::Audio);
	extensions << FileTypes::extensions(FileType::Playlist);
	extensions << FileTypes::extensions(FileType::Subtitle);
	_fileSearchModel->setSearchExtensions(extensions);

	//_dirModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
	//_dirModel->setSorting(QDir::Name | QDir::DirsFirst);
	//_dirModel->setReadOnly(true);
	//_dirModel->setNameFilterDisables(false);

	_treeView->setHeaderHidden(true);

	_treeView->setModel(_fileSearchModel);

	//QHeaderView * header = _treeView->header();
	//header->hideSection(1);
	//header->hideSection(2);
	//header->hideSection(3);
	//header->setResizeMode(QHeaderView::ResizeToContents);
	//header->setStretchLastSection(false);
	//header->setVisible(false);

	connect(&Config::instance(), SIGNAL(valueChanged(const QString &, const QVariant &)),
		SLOT(musicDirChanged(const QString &, const QVariant &)));
	//_treeView->setRootIndex(_dirModel->index(Config::instance().musicDir(uuid())));
	//_dirModel->setRootPath(Config::instance().musicDir(uuid()));
	_treeView->setDragEnabled(true);
	_treeView->setRootIsDecorated(true);

	//Ok show the content of the directory
	search();
}

void FileBrowserWidget::search() {
	//Resets the model and the view first before to perform a new search
	_fileSearchModel->reset();

	disconnect(_fileSearchModel, SIGNAL(searchFinished(int)),
		this, SLOT(searchFinished(int)));

	QString rootSearchPath(Config::instance().musicDir(uuid()));
	_fileSearchModel->setRootSearchPath(rootSearchPath);

	QString pattern(_searchLineEdit->text());
	if (pattern.isEmpty()) {
		setWindowTitle(QString());

		_fileSearchModel->search(rootSearchPath, QRegExp(QString(), Qt::CaseInsensitive, QRegExp::RegExp2), INT_MAX, false);
	} else {
		//Sets a busy mouse cursor since the search can take several seconds
		unsetCursor();
		setCursor(Qt::BusyCursor);

		setWindowTitle(tr("Searching..."));

		connect(_fileSearchModel, SIGNAL(searchFinished(int)),
			SLOT(searchFinished(int)));

		QString tmp;
		QStringList words(pattern.split(' '));
		for (int i = 0; i < words.size(); i++) {
			tmp += '(' + words[i] + ')';
			if (i < words.size() - 1) {
				tmp += ".*";
			}
		}
		qDebug() << __FUNCTION__ << tmp;

		_fileSearchModel->search(rootSearchPath, QRegExp(tmp, Qt::CaseInsensitive, QRegExp::RegExp2), 1, true);
	}
}

void FileBrowserWidget::searchFinished(int timeElapsed) {
	//Restores the mouse cursor from busy to normal
	unsetCursor();

	setWindowTitle(tr("Search finished:") + ' ' + QString::number((float) timeElapsed / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_fileSearchModel->rowCount()) + ' ' + tr("medias") + ')');

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
	Q_UNUSED(value);

	if (key == Config::MUSIC_DIR_KEY + uuid().toString()) {
		search();
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

	uuidAction("FileBrowser.Browse")->setText(tr("Change Directory"));
	uuidAction("FileBrowser.Browse")->setIcon(TkIcon("document-open-folder"));

	uuidAction("FileBrowser.New")->setText(tr("New File Browser Window"));
	uuidAction("FileBrowser.New")->setIcon(TkIcon("window-new"));

	setWindowTitle(QString());
}

void FileBrowserWidget::setWindowTitle(const QString & statusMessage) {
	if (statusMessage.isEmpty()) {
		_dockWidget->setWindowTitle(Config::instance().musicDir(uuid()));
	} else {
		_dockWidget->setWindowTitle(statusMessage);
		QStatusBar * statusBar = getMainWindow()->statusBar();
		if (statusBar) {
			statusBar->showMessage(statusMessage);
		}
	}
}

void FileBrowserWidget::configWindowCreated(ConfigWindow * configWindow) {
	//Add to config window
	configWindow->addConfigWidget(new FileBrowserConfigWidget(uuid()));
}

//FIXME should be factorized
#include <tkutil/ActionCollection.h>
QAction * FileBrowserWidget::uuidAction(const QString & name) {
	if (uuid().isNull()) {
		qCritical() << __FUNCTION__ << "Error: UUID is null";
	}
	return ActionCollection::action(name + '_' + uuid().toString());
}
void FileBrowserWidget::addUuidAction(const QString & name, QAction * action) {
	if (uuid().isNull()) {
		qCritical() << __FUNCTION__ << "Error: UUID is null";
	}
	return ActionCollection::addAction(name + '_' + uuid().toString(), action);
}
///
