/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "SimpleMPlayer.h"

#include "ui_SimpleMPlayer.h"

#include <phonon-mplayer/libmplayer/MPlayerLoader.h>
#include <phonon-mplayer/libmplayer/MPlayerProcess.h>
#include <phonon-mplayer/libmplayer/MPlayerVideoWidget.h>

#include <QtGui/QtGui>

using namespace Phonon::MPlayer;

SimpleMPlayer::SimpleMPlayer(QWidget * parent)
	: QMainWindow(parent) {

	_ui = new Ui::SimpleMPlayer();
	_ui->setupUi(this);

	//Run an empty MPlayerProcess in order to get the MPlayer version number
	//Using the MPlayer version number, we performs some tests (see MPlayerLoader.cpp)
	//and thus we need to know the MPlayer version number as soon as possible
	if (MPlayerProcess::getMPlayerVersion() == MPlayerProcess::MPLAYER_VERSION_NOTFOUND) {
		MPlayerLoader::startMPlayerVersion(this);
	}
	///

	//QWidget * videoWidget = new QWidget();
	//centralWidget()->layout()->addWidget(videoWidget);

	_videoWidget = new MPlayerVideoWidget(_ui->videoWidget);

	//QVBoxLayout * layout = new QVBoxLayout();
	//layout->addWidget(_videoWidget);
	//centralWidget()->setLayout(layout);

	_process = MPlayerLoader::createNewMPlayerProcess(this);
	connect(_process, SIGNAL(videoWidgetSizeChanged(int, int)),
		SLOT(videoWidgetSizeChanged(int, int)));

	connect(_ui->actionPlay, SIGNAL(triggered()), SLOT(playFile()));
}

SimpleMPlayer::~SimpleMPlayer() {
}

void SimpleMPlayer::playFile() {
	QString fileName = QFileDialog::getOpenFileName(this, "Open File");
	if (fileName.isEmpty()) {
		return;
	}

	MPlayerLoader::start(_process, fileName, _videoWidget->winId());
}

void SimpleMPlayer::videoWidgetSizeChanged(int width, int height) {
	LibMPlayerDebug() << "Video width:" << width << "height:" << height;

	//I spent 2 full days for these few fucking lines of code!
	//It resizes dynamically the widget + the main window
	//I didn't find another way
	//Each line is very important!
	//If someone finds a better solution, please tell me!

	QSize videoSize(width, height);
	videoSize.boundedTo(QApplication::desktop()->availableGeometry().size());

	_videoWidget->hide();
	_videoWidget->setVideoSize(videoSize);

#ifdef Q_WS_WIN
	QWidget * parent = qobject_cast<QWidget *>(this->parent());

	QSize previousSize;
	if (parent) {
		previousSize = parent->minimumSize();
		parent->setMinimumSize(videoSize);
	}
#endif	//Q_WS_WIN

	_videoWidget->show();

#ifdef Q_WS_WIN
	if (parent) {
		parent->setMinimumSize(previousSize);
	}
#endif	//Q_WS_WIN
	///
}
