/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"

#include "CommonActions.h"
#include "AboutWindow.h"
#include "MainWindowLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/version.h>

#include <Logger/LogWindow.h>

#include <TkUtil/ActionCollection.h>
#include <TkUtil/TkFileDialog.h>
#include <TkUtil/TkToolBar.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <FileTypes/FileTypes.h>

#include <QtSingleApplication>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#include <phonon/audiooutput.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(MainWindow, MainWindowFactory);

const char * MainWindowFactory::PLUGIN_NAME = "MainWindow";

QStringList MainWindowFactory::dependencies() const {
	QStringList tmp;
	return tmp;
}

PluginInterface * MainWindowFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new MainWindow(quarkPlayer, uuid);
}

MainWindow * MainWindowFactory::mainWindow() {
	MainWindow * mainWindow = dynamic_cast<MainWindow *>(PluginManager::instance().pluginInterface(PLUGIN_NAME));
	Q_ASSERT(mainWindow);
	return mainWindow;
}

MainWindow::MainWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: IMainWindow(quarkPlayer, uuid) {

	new CommonActions(quarkPlayer, this);

	QtSingleApplication * app = qobject_cast<QtSingleApplication *>(QCoreApplication::instance());
	if (app) {
		app->setActivationWindow(this);
		app->activateWindow();
	}

	populateActionCollection();

	setupUi();

	//Accepts Drag&Drop
	setAcceptDrops(true);

	//DockWidgets tabs are vertical like in Amarok
	setDockOptions(QMainWindow::VerticalTabs);

	_playToolBar = NULL;
	_statusBar = NULL;

	connect(Actions["Common.OpenFile"], SIGNAL(triggered()), SLOT(playFile()));
	connect(Actions["Common.OpenDVD"], SIGNAL(triggered()), SLOT(playDVD()));
	connect(Actions["Common.OpenURL"], SIGNAL(triggered()), SLOT(playURL()));
	connect(Actions["Common.OpenVCD"], SIGNAL(triggered()), SLOT(playVCD()));
	connect(Actions["Common.NewMediaObject"], SIGNAL(triggered()), &quarkPlayer, SLOT(createNewMediaObject()));
	connect(Actions["Common.Quit"], SIGNAL(triggered()), SLOT(close()));
	connect(Actions["Common.ReportBug"], SIGNAL(triggered()), SLOT(reportBug()));
	connect(Actions["Common.ShowMailingList"], SIGNAL(triggered()), SLOT(showMailingList()));
	connect(Actions["Common.ShowLog"], SIGNAL(triggered()), SLOT(showLog()));
	connect(Actions["Common.About"], SIGNAL(triggered()), SLOT(about()));
	connect(Actions["Common.AboutQt"], SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(Actions["Common.VolumeMute"], SIGNAL(toggled(bool)), SLOT(mutedToggled(bool)));

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	bool ok = restoreGeometry(Config::instance().mainWindowGeometry());
	if (!ok) {
		MainWindowCritical() << "Couldn't restore main window geometry";
	}

	RETRANSLATE(this);
	retranslate();

	show();
}

MainWindow::~MainWindow() {
	//If the MainWindow is destroyed, it's reasonable to say
	//that we can quit the entire application
	QCoreApplication::quit();
}

void MainWindow::setPlayToolBar(QToolBar * playToolBar) {
	_playToolBar = playToolBar;

#ifdef Q_WS_MAC
	_playToolBar->setFloatable(false);
	_playToolBar->setMovable(false);
#endif	//!Q_WS_MAC

	addToolBar(Qt::BottomToolBarArea, playToolBar);

	emit playToolBarAdded(_playToolBar);
}

QToolBar * MainWindow::playToolBar() const {
	return _playToolBar;
}

QToolBar * MainWindow::mainToolBar() const {
	return _mainToolBar;
}

void MainWindow::setStatusBar(QStatusBar * statusBar) {
	_statusBar = statusBar;
	if (_statusBar) {
		//If statusBar == NULL then QMainWindow::setStatusBar(NULL)
		//will call statusBar destructor
		//This can lead to an infinite loop since the statusbar plugin
		//does a MainWindow::setStatusBar(NULL) inside its destructor
		QMainWindow::setStatusBar(statusBar);
		emit statusBarAdded(_statusBar);
	}
}

QStatusBar * MainWindow::statusBar() const {
	return _statusBar;
}

void MainWindow::playFile() {
	QStringList fileNames = TkFileDialog::getOpenFileNames(
		this, tr("Select Audio/Video File"), Config::instance().lastDirOpened(),
		tr("Multimedia") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video, FileType::Audio)) + ";;" +
		tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video)) +";;" +
		tr("Audio") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Audio)) +";;" +
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*)"
	);

	if (!fileNames.isEmpty()) {
		QString file(fileNames[0]);
		Config::instance().setValue(Config::LAST_DIR_OPENED_KEY, QFileInfo(file).absolutePath());

		play(file);

		emit filesOpened(fileNames);
	}
}

QString MainWindow::getDiscPath(const QString & defaultPath, const QString & windowTitle) {
	QString path;

#ifdef Q_WS_WIN
	path = TkFileDialog::getExistingDirectory(this, windowTitle, defaultPath);
#else
	//Under Linux
	//FIXME and other OS like Mac OS X??
	path = TkFileDialog::getOpenFileName(this, windowTitle, defaultPath);
#endif	//Q_WS_WIN

	return path;
}

void MainWindow::playDVD() {
	QString dvd = getDiscPath(Config::instance().dvdPath(), tr("Select DVD Device"));
	if (!dvd.isEmpty()) {
		Config::instance().setValue(Config::DVD_PATH_KEY, dvd);
		play(Phonon::MediaSource(Phonon::Dvd, dvd));
	}
}

void MainWindow::playVCD() {
	QString cdrom = getDiscPath(Config::instance().cdromPath(), tr("Select SVCD/VCD Device"));
	if (!cdrom.isEmpty()) {
		Config::instance().setValue(Config::CDROM_PATH_KEY, cdrom);
		play(Phonon::MediaSource(Phonon::Vcd, cdrom));
	}
}

void MainWindow::playURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));

	if (!url.isEmpty()) {
		play(url);
	}
}

void MainWindow::play(const Phonon::MediaSource & mediaSource) {
	quarkPlayer().play(mediaSource);
	//_statusBar->showMessage(tr("Processing") + " " + fileName + "...");
}

void MainWindow::updateWindowTitle() {
	QString title = quarkPlayer().currentMediaObjectTitle();

	if (title.isEmpty()) {
		setWindowTitle(QCoreApplication::applicationName() + " " + QString(QUARKPLAYER_VERSION));
	} else {
		setWindowTitle(title);
	}
}

void MainWindow::reportBug() {
	QDesktopServices::openUrl(QUrl("http://code.google.com/p/quarkplayer/issues/list"));
}

void MainWindow::showMailingList() {
	QDesktopServices::openUrl(QUrl("http://groups.google.com/group/quarkplayer"));
}

void MainWindow::showLog() {
	static LogWindow * logWindow = new LogWindow(this);
	logWindow->show();
}

void MainWindow::about() {
	static AboutWindow * aboutWindow = new AboutWindow(this);
	aboutWindow->exec();
}

void MainWindow::populateActionCollection() {
}

void MainWindow::setupUi() {
	//Fake a central widget, Qt requires it
	setCentralWidget(new QWidget());
	centralWidget()->hide();
	///

	_menuFile = new QMenu();
	menuBar()->addMenu(_menuFile);
	_menuFile->addAction(Actions["Common.OpenFile"]);
	_menuFile->addAction(Actions["Common.OpenURL"]);
	_menuFile->addAction(Actions["Common.OpenDVD"]);
	_menuFile->addAction(Actions["Common.OpenVCD"]);
	_menuFile->addSeparator();
	_menuFile->addAction(Actions["Common.Quit"]);

	_menuPlay = new QMenu();
	menuBar()->addMenu(_menuPlay);
	_menuPlay->addAction(Actions["Common.PreviousTrack"]);
	_menuPlay->addAction(Actions["Common.PlayPause"]);
	_menuPlay->addAction(Actions["Common.Stop"]);
	_menuPlay->addAction(Actions["Common.NextTrack"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["Common.JumpBackward10s"]);
	_menuPlay->addAction(Actions["Common.JumpBackward1min"]);
	_menuPlay->addAction(Actions["Common.JumpBackward10min"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["Common.JumpForward10s"]);
	_menuPlay->addAction(Actions["Common.JumpForward1min"]);
	_menuPlay->addAction(Actions["Common.JumpForward10min"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["Common.SpeedDecrease10%"]);
	_menuPlay->addAction(Actions["Common.SpeedIncrease10%"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["Common.FullScreen"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["Common.NewMediaObject"]);

	_menuAudio = new QMenu();
	menuBar()->addMenu(_menuAudio);
	_menuAudio->addAction(Actions["Common.VolumeMute"]);
	_menuAudio->addAction(Actions["Common.VolumeIncrease10%"]);
	_menuAudio->addAction(Actions["Common.VolumeDecrease10%"]);

	_menuSettings = new QMenu();
	menuBar()->addMenu(_menuSettings);
	_menuSettings->addAction(Actions["Common.Equalizer"]);
	_menuSettings->addAction(Actions["Common.Configure"]);

	_menuHelp = new QMenu();
	menuBar()->addMenu(_menuHelp);
	_menuHelp->addAction(Actions["Common.ShowMailingList"]);
	_menuHelp->addAction(Actions["Common.ReportBug"]);
	_menuHelp->addAction(Actions["Common.ShowLog"]);
	_menuHelp->addSeparator();
	_menuHelp->addAction(Actions["Common.About"]);
	_menuHelp->addAction(Actions["Common.AboutQt"]);

	//Main ToolBar
	_mainToolBar = new TkToolBar(this);
	TkToolBar::setToolButtonStyle(_mainToolBar);
	_mainToolBar->addAction(Actions["Common.OpenFile"]);
	_mainToolBar->addAction(Actions["Common.OpenDVD"]);
	//_mainToolBar->addAction(Actions["Common.OpenURL"]);
	//_mainToolBar->addSeparator();
	//_mainToolBar->addAction(Actions["Common.Equalizer"]);
	//_mainToolBar->addAction(Actions["Common.Configure"]);
	addToolBar(_mainToolBar);

	//Main toolbar accessible but disabled by default
	_mainToolBar->hide();
}

void MainWindow::retranslate() {
	updateWindowTitle();
	setWindowIcon(QIcon(":/icons/quarkplayer-16x16.png"));

	//Main toolbar accessible but disabled by default
	_mainToolBar->setWindowTitle(tr("Main ToolBar"));
	_mainToolBar->setMinimumSize(_mainToolBar->sizeHint());

	_menuFile->setTitle(tr("&File"));
	_menuPlay->setTitle(tr("&Play"));
	_menuAudio->setTitle(tr("&Audio"));
	_menuSettings->setTitle(tr("&Settings"));
	_menuHelp->setTitle(tr("&Help"));
}

QMenu * MainWindow::menuFile() const {
	return _menuFile;
}

QMenu * MainWindow::menuPlay() const {
	return _menuPlay;
}

QMenu * MainWindow::menuAudio() const {
	return _menuAudio;
}

QMenu * MainWindow::menuSettings() const {
	return _menuSettings;
}

QMenu * MainWindow::menuHelp() const {
	return _menuHelp;
}

void MainWindow::dragEnterEvent(QDragEnterEvent * event) {
	MainWindowDebug() << event->mimeData()->formats();

	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent * event) {
	QStringList files;

	//Add urls to a list
	if (event->mimeData()->hasUrls()) {
		QList<QUrl> urlList = event->mimeData()->urls();
		QString fileName;
		foreach (QUrl url, urlList) {
			if (url.isValid()) {
				MainWindowDebug() << "File scheme:" << url.scheme();
				if (url.scheme() == "file") {
					fileName = url.toLocalFile();
				} else {
					fileName = url.toString();
				}
				MainWindowDebug() << "File dropped:" << fileName;
				files << fileName;
			}
		}
	}

	if (files.count() > 0) {
		if (files.count() == 1) {
			//1 file
			QString fileName = files[0];

			bool isSubtitle = FileTypes::fileExtensionMatches(fileName, FileTypes::extensions(FileType::Subtitle));
			if (isSubtitle) {
				MainWindowDebug() << "Loading subtitle:" << fileName;
				emit subtitleFileDropped(fileName);
			} else if (QFileInfo(fileName).isDir()) {
				//TODO open directory
			} else {
				//TODO add to playlist if 'auto-add-playlist' option
				play(fileName);
			}
		} else {
			//Several files

			//TODO add files to playlist
			//and play the first one in the list
		}
	}
}

void MainWindow::closeEvent(QCloseEvent * event) {
	//Saves window geometry
	Config::instance().setValue(Config::MAINWINDOW_GEOMETRY_KEY, saveGeometry());

	TkMainWindow::closeEvent(event);

	event->accept();

	//Quits the application
	//QCoreApplication::quit();

	//FIXME we should only use QCoreApplication::quit()
	//exit(EXIT_SUCCESS);
}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * lastDockWidget, QDockWidget * dockWidget) {
	if (dockWidget) {

		//Disable QDockWidget features
		//The dock widget cannot be closed, moved, or floated
		//dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);

		//Hide the title bar
		//dockWidget->setTitleBarWidget(new QWidget(this));

		QMainWindow::addDockWidget(area, dockWidget);
		if (lastDockWidget) {
			tabifyDockWidget(lastDockWidget, dockWidget);
		}

		//Others use a QTimer otherwise not all tabs inside QTabBar are detected
		//I prefer to run processEvents()
		//See http://developer.qt.nokia.com/faq/answer/how_can_i_check_which_tab_is_the_current_one_in_a_tabbed_qdockwidget
		//See http://ariya.blogspot.com/2007/04/tab-bar-with-roundednorth-for-tabbed.html
		QApplication::instance()->processEvents();
		hackDockWidgetTabBar();
	}
}

void MainWindow::hackDockWidgetTabBar() {
	QList<QTabBar *> tabBars = findChildren<QTabBar *>();
	foreach (QTabBar * tabBar, tabBars) {

		//Check the QTabBar is for a QDockWidget: the parent should be the main window
		//This test is definitely not enough :/
		if (tabBar->parentWidget() == this) {

			//Do this first otherwise tabBar->count() is fucked
			tabBar->setTabsClosable(true);

			disconnect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
			connect(tabBar, SIGNAL(tabCloseRequested(int)), SLOT(tabCloseRequested(int)));
		}
	}
}

QDockWidget * MainWindow::findDockWidget(QTabBar * tabBar, int index) {
	Q_ASSERT(tabBar);

	//Internal Qt implementation already use tabData with QVariant being
	//a quintptr which is in fact a QWidget pointer!
	//See http://qt.gitorious.org/qt/qt/blobs/4.7/src/gui/widgets/qdockarealayout.cpp
	//This is just perfect for us!
	//Assuming qulonglong (QVariant::toULongLong()) and quintptr are equivalent
	QVariant tmp = tabBar->tabData(index);
	QDockWidget * dockWidget = reinterpret_cast<QDockWidget *>(tmp.toULongLong());
	Q_ASSERT(dockWidget);
	///

	return dockWidget;
}

QPair<QTabBar *, int> MainWindow::findDockWidgetTab(QDockWidget * dockWidget) {
	QPair<QTabBar *, int> result;

	QList<QTabBar *> tabBars = findChildren<QTabBar *>();
	foreach (QTabBar * tabBar, tabBars) {

		//Check the QTabBar is for a QDockWidget: the parent should be the main window
		//This test is definitely not enough :/
		if (tabBar->parentWidget() == this) {
			for (int i = 0; i < tabBar->count(); i++) {
				QDockWidget * tmp = findDockWidget(tabBar, i);
				if (dockWidget == tmp) {

					//Found the right QDockWidget
					result.first = tabBar;
					result.second = i;
					return result;
				}
			}
		}
	}

	//Cannot be in this state
	Q_ASSERT(false);

	return result;
}

void MainWindow::tabCloseRequested(int index) {
	QTabBar * tabBar = qobject_cast<QTabBar *>(sender());
	QDockWidget * dockWidget = findDockWidget(tabBar, index);
	removeDockWidget(dockWidget);
	//FIXME needed? dockWidget->close();
}

void MainWindow::addBrowserDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::LeftDockWidgetArea, lastDockWidget, dockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::addVideoDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, lastDockWidget, dockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::addPlaylistDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, lastDockWidget, dockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	//Resets the window title when needed
	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(updateWindowTitle()));

	disconnect(Actions["Common.Quit"], SIGNAL(triggered()), mediaObject, SLOT(stop()));
	connect(Actions["Common.Quit"], SIGNAL(triggered()), mediaObject, SLOT(stop()));

	Phonon::AudioOutput * audioOutput = quarkPlayer().currentAudioOutput();
	if (audioOutput) {
		//Avoid a crash inside Phonon if the backend couldn't be loaded
		Actions["Common.VolumeMute"]->setChecked(audioOutput->isMuted());
		disconnect(audioOutput, SIGNAL(mutedChanged(bool)), this, SLOT(mutedChanged(bool)));
		connect(audioOutput, SIGNAL(mutedChanged(bool)), SLOT(mutedChanged(bool)));
	}
}

void MainWindow::mutedChanged(bool muted) {
	Actions["Common.VolumeMute"]->setChecked(muted);
}

void MainWindow::mutedToggled(bool muted) {
	Phonon::AudioOutput * audioOutput = quarkPlayer().currentAudioOutput();
	Q_ASSERT(audioOutput);
	audioOutput->setMuted(muted);
}
