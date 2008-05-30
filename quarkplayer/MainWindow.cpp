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

#include "MainWindow.h"

#include "ui_MainWindow.h"

#include "PlayToolBar.h"
#include "VideoWidget.h"
#include "StatusBar.h"
#include "MediaController.h"
#include "config/Config.h"
#include "config/ConfigWindow.h"

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>

#include <QtCore/QSignalMapper>

#include <QtGui/QtGui>

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent) {

	_ui = new Ui::MainWindow();
	_ui->setupUi(this);

	addRecentFilesToMenu();

	_mediaObject = new Phonon::MediaObject(this);
	_mediaObject->setTickInterval(1000);
	connect(_mediaObject, SIGNAL(metaDataChanged()),
		SLOT(metaDataChanged()));
	connect(_mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
		SLOT(sourceChanged(const Phonon::MediaSource &)));
	connect(_mediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));

	MediaController * mediaController = new MediaController(_ui, _mediaObject, this);

	//audioOutput
	_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	Phonon::createPath(_mediaObject, _audioOutput);

	//toolBar
	_playToolBar = new PlayToolBar(_mediaObject, _audioOutput);

	//videoWidget
	_videoWidget = new VideoWidget(this, _mediaObject);
	Phonon::createPath(_mediaObject, _videoWidget);
	_ui->videoLayout->addWidget(_videoWidget);

	//statusBar
	setStatusBar(new StatusBar(_mediaObject));

	connect(_ui->actionPlayDVD, SIGNAL(triggered()), SLOT(playDVD()));
	connect(_ui->actionPlayURL, SIGNAL(triggered()), SLOT(playURL()));
	connect(_ui->actionAddFiles, SIGNAL(triggered()), SLOT(addFiles()));
	connect(_ui->actionOpenSubtitleFile, SIGNAL(triggered()), SLOT(openSubtitleFile()));
	connect(_ui->actionPreferences, SIGNAL(triggered()), SLOT(showConfigWindow()));
	connect(_ui->actionExit, SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(_ui->actionExit, SIGNAL(triggered()), SLOT(close()));
	connect(_ui->actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(_ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

MainWindow::~MainWindow() {
	delete _ui;
}

PlayToolBar * MainWindow::playToolBar() const {
	return _playToolBar;
}

VideoWidget * MainWindow::videoWidget() const {
	return _videoWidget;
}

void MainWindow::addRecentFilesToMenu() {
	connect(_ui->actionClearRecentFiles, SIGNAL(triggered()), SLOT(clearRecentFiles()));

	QStringList recentFiles = Config::instance().recentFiles();
	if (!recentFiles.isEmpty()) {

		_ui->menuRecentFiles->clear();

		QSignalMapper * signalMapper = new QSignalMapper(this);

		for (int i = 0; i < recentFiles.size(); i++) {
			QAction * action = _ui->menuRecentFiles->addAction(recentFiles[i], signalMapper, SLOT(map()));
			signalMapper->setMapping(action, i);
		}

		_ui->menuRecentFiles->addSeparator();
		_ui->menuRecentFiles->addAction(_ui->actionClearRecentFiles);

		connect(signalMapper, SIGNAL(mapped(int)),
			SLOT(playRecentFile(int)));
	}
}

void MainWindow::playRecentFile(int id) {
	QStringList recentFiles = Config::instance().recentFiles();
	play(recentFiles[id]);
}

void MainWindow::clearRecentFiles() {
	//Clear recent files menu
	_ui->menuRecentFiles->clear();
	_ui->menuRecentFiles->addSeparator();
	_ui->menuRecentFiles->addAction(_ui->actionClearRecentFiles);

	//Clear recent files configuration
	Config::instance().setValue(Config::RECENT_FILES_KEY, QStringList());
}

void MainWindow::addFiles() {
	static const int MAX_RECENT_FILES = 10;

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio/Video File"));

	if (fileName.isEmpty()) {
		return;
	}

	QStringList recentFiles = Config::instance().recentFiles();
	bool prepend = false;
	if (recentFiles.isEmpty()) {
		prepend = true;
	} else if (fileName != recentFiles.first()) {
		prepend = true;
	}
	if (prepend) {
		recentFiles.prepend(fileName);
		if (recentFiles.size() > MAX_RECENT_FILES) {
			recentFiles.removeLast();
		}
	}

	Config::instance().setValue(Config::RECENT_FILES_KEY, recentFiles);

	addRecentFilesToMenu();

	play(fileName);
}

void MainWindow::playDVD() {
	play(Phonon::MediaSource(Phonon::Dvd, "D:"));
}

void MainWindow::playURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));
	if (!url.isEmpty()) {
		play(url);
	}
}

void MainWindow::play(const Phonon::MediaSource & mediaSource) {
	_mediaObject->setCurrentSource(mediaSource);
	_mediaObject->play();
}

void MainWindow::sourceChanged(const Phonon::MediaSource & source) {
	//currentTimeLcdNumber->display("00:00:00");
}

void MainWindow::metaDataChanged() {
	qDebug() << __FUNCTION__;

	QMultiMap<QString, QString> metaData = _mediaObject->metaData();

	QString title = metaData.value("TITLE");
	if (title.isEmpty()) {
		title = _mediaObject->currentSource().fileName();
	}

	/*titleLabel->setText(title);
	artistLabel->setText(metaData.value("ARTIST"));
	albumLabel->setText(metaData.value("ALBUM"));
	yearLabel->setText(metaData.value("DATE"));*/

	setWindowTitle("QuarkPlayer - " + title);
}

void MainWindow::aboutToFinish() {
}

void MainWindow::closeEvent(QCloseEvent * event) {
	_mediaObject->stop();
	event->accept();
}

void MainWindow::showConfigWindow() {
	static ConfigWindow * configWindow = new ConfigWindow(this);
	configWindow->show();
}

void MainWindow::about() {
	QMessageBox::information(this, tr("About QuarkPlayer"),
			tr(
			"QuarkPlayer, a Phonon media player\n" \
			"Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>\n\n" \
			"This program is free software: you can redistribute it and/or modify\n" \
			"it under the terms of the GNU General Public License as published by\n" \
			"the Free Software Foundation, either version 3 of the License, or\n" \
			"(at your option) any later version."
			));
}
