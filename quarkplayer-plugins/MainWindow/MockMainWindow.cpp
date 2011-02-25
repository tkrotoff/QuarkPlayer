/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MockMainWindow.h"

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

#include <FileTypes/FileTypes.h>

#include <QtSingleApplication>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#include <phonon/audiooutput.h>

#include <QtGui/QtGui>

MockMainWindow::MockMainWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: IMainWindow(quarkPlayer, uuid) {

	new CommonActions(quarkPlayer, this);

	setupUi();

	//DockWidgets tabs are vertical like in Amarok
	setDockOptions(QMainWindow::VerticalTabs);

	_playToolBar = NULL;

	connect(Actions["Common.OpenFile"], SIGNAL(triggered()), SLOT(playFile()));
	connect(Actions["Common.Quit"], SIGNAL(triggered()), SLOT(close()));
	connect(Actions["Common.ShowLog"], SIGNAL(triggered()), SLOT(showLog()));
	connect(Actions["Common.About"], SIGNAL(triggered()), SLOT(about()));

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	show();
}

MockMainWindow::~MockMainWindow() {
	//If the MainWindow is destroyed, it's reasonable to say
	//that we can quit the entire application
	QCoreApplication::quit();
}

void MockMainWindow::setPlayToolBar(QToolBar * playToolBar) {
	_playToolBar = playToolBar;
	addToolBar(Qt::BottomToolBarArea, playToolBar);
	emit playToolBarAdded(_playToolBar);
}

QToolBar * MockMainWindow::playToolBar() const {
	return _playToolBar;
}

QMenu * MockMainWindow::menuFile() const {
	return _menuFile;
}

QMenu * MockMainWindow::menuPlay() const {
	return _menuPlay;
}

QMenu * MockMainWindow::menuAudio() const {
	return NULL;
}

QMenu * MockMainWindow::menuSettings() const {
	return _menuSettings;
}

QMenu * MockMainWindow::menuHelp() const {
	return _menuHelp;
}

void MockMainWindow::playFile() {
	QString fileName = TkFileDialog::getOpenFileName(
		this, tr("Select Audio/Video File"), Config::instance().lastDirOpened(),
		tr("Multimedia") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video, FileType::Audio)) + ";;" +
		tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video)) +";;" +
		tr("Audio") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Audio)) +";;" +
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*)"
	);

	if (!fileName.isEmpty()) {
		quarkPlayer().play(fileName);
	}
}

void MockMainWindow::showLog() {
	static LogWindow * logWindow = new LogWindow(this);
	logWindow->show();
}

void MockMainWindow::about() {
	static AboutWindow * aboutWindow = new AboutWindow(this);
	aboutWindow->exec();
}

void MockMainWindow::setupUi() {
	_menuFile = new QMenu();
	_menuFile->setTitle("&File");
	menuBar()->addMenu(_menuFile);
	_menuFile->addAction(Actions["Common.OpenFile"]);
	_menuFile->addSeparator();
	_menuFile->addAction(Actions["Common.Quit"]);

	_menuPlay = new QMenu();
	_menuPlay->setTitle("&Play");
	menuBar()->addMenu(_menuPlay);
	_menuPlay->addAction(Actions["Common.PlayPause"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["Common.FullScreen"]);
	//No menu entry for FullScreenExit, see MyVideoWidget.cpp

	_menuSettings = new QMenu();
	_menuSettings->setTitle("&Settings");
	menuBar()->addMenu(_menuSettings);
	_menuSettings->addAction(Actions["Common.Equalizer"]);
	_menuSettings->addAction(Actions["Common.Configure"]);

	_menuHelp = new QMenu();
	_menuHelp->setTitle("&Help");
	menuBar()->addMenu(_menuHelp);
	_menuHelp->addAction(Actions["Common.ShowLog"]);
	_menuHelp->addAction(Actions["Common.About"]);
}

void MockMainWindow::closeEvent(QCloseEvent * event) {
	TkMainWindow::closeEvent(event);

	event->accept();

	//Quits the application
	//QCoreApplication::quit();

	//FIXME we should only use QCoreApplication::quit()
	//exit(EXIT_SUCCESS);
}

void MockMainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * lastDockWidget, QDockWidget * dockWidget) {
	if (dockWidget) {
		//dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
		//dockWidget->setFloating(false);

		//To hide the title bar completely
		//we must replace the default widget with a generic one
		dockWidget->setTitleBarWidget(new QWidget(this));

		QMainWindow::addDockWidget(area, dockWidget);
		if (lastDockWidget) {
			tabifyDockWidget(lastDockWidget, dockWidget);
		}
	}
}

void MockMainWindow::addBrowserDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::LeftDockWidgetArea, lastDockWidget, dockWidget);
	lastDockWidget = dockWidget;
}

void MockMainWindow::addVideoDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, lastDockWidget, dockWidget);
	lastDockWidget = dockWidget;
}

void MockMainWindow::addPlaylistDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, lastDockWidget, dockWidget);
	lastDockWidget = dockWidget;
}

void MockMainWindow::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	disconnect(Actions["Common.Quit"], SIGNAL(triggered()), mediaObject, SLOT(stop()));
	connect(Actions["Common.Quit"], SIGNAL(triggered()), mediaObject, SLOT(stop()));
}
