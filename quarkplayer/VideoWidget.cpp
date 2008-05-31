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

#include "VideoWidget.h"

#include "MainWindow.h"
#include "PlayToolBar.h"

#include <phonon/mediaobject.h>

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QLabel>
#include <QtGui/QtGui>
#include <QtCore/QTimerEvent>

VideoWidget::VideoWidget(MainWindow * mainWindow, Phonon::MediaObject * mediaObject)
	: Phonon::VideoWidget(NULL),
	_mainWindow(mainWindow) {

	setAcceptDrops(true);

	connect(_mainWindow->playToolBar(), SIGNAL(fullScreenButtonClicked(bool)),
		SLOT(setFullScreenSlot(bool)));

	//By default not in fullscreen mode
	setFullScreenSlot(false);
}

VideoWidget::~VideoWidget() {
}

void VideoWidget::setFullScreenSlot(bool fullScreen) {
	if (fullScreen) {
		//FIXME QStackedWidget and fullscreen mode are not well together
		//I hope to remove this code in the future...
		//There should be no reason why a widget inside a QStackedWidget could
		//not be fullscreen
		_mainWindow->stackedWidget()->removeWidget(this);
		setFullScreen(fullScreen);
	} else {
		setFullScreen(fullScreen);
		_mainWindow->stackedWidget()->addWidget(this);
		_mainWindow->stackedWidget()->setCurrentWidget(this);

		//Leaving fullscreen
		addPlayToolBarToMainWindow();
	}
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent * event) {
	Phonon::VideoWidget::mouseDoubleClickEvent(event);
	_mainWindow->playToolBar()->setCheckedFullScreenButton(!isFullScreen());
}

void VideoWidget::keyPressEvent(QKeyEvent * event) {
	if (event->key() == Qt::Key_Escape && !event->modifiers()) {
		setFullScreenSlot(false);
		event->accept();
		return;
	}

	Phonon::VideoWidget::keyPressEvent(event);
}

bool VideoWidget::event(QEvent * event) {
	switch(event->type()) {
	case QEvent::Close:
		//We just ignore the close event on the video widget
		//this prevents ALT+F4 from having an effect in fullscreen mode
		event->ignore();
		return true;

	case QEvent::MouseMove:
		if (isFullScreen()) {
			checkMousePos();
			_timer.start(1000, this);
		}
		unsetCursor();
		break;

	case QEvent::WindowStateChange:
		if (isFullScreen()) {
			_timer.start(1000, this);
		} else {
			_timer.stop();
			unsetCursor();
		}
		break;

	default:
		break;
	}

	return Phonon::VideoWidget::event(event);
}

void VideoWidget::timerEvent(QTimerEvent * event) {
	if (event->timerId() == _timer.timerId()) {
		//Let's store the cursor shape
		setCursor(Qt::BlankCursor);
	}
	Phonon::VideoWidget::timerEvent(event);
}

void VideoWidget::dropEvent(QDropEvent * event) {
	//_mainWindow->handleDrop(event);
}

void VideoWidget::dragEnterEvent(QDragEnterEvent * event) {
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void VideoWidget::checkMousePos() {
	static const int MARGIN = 70;
	static bool nearBottom = false;

	if (!isFullScreen()) {
		return;
	}

	PlayToolBar * playToolBar = _mainWindow->playToolBar();

	QPoint pos = QCursor::pos();

	if (pos.y() > height() - MARGIN) {
		if (!nearBottom) {
			qDebug() << __FUNCTION__;
			nearBottom = true;

			_mainWindow->removeToolBar(playToolBar);
			playToolBar->setParent(NULL);
			playToolBar->setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
					Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

			playToolBar->showOver(this);
		}
	} else {
		if (nearBottom) {
			nearBottom = false;
			addPlayToolBarToMainWindow();
		}
	}
}

void VideoWidget::addPlayToolBarToMainWindow() {
	qDebug() << __FUNCTION__;
	_mainWindow->addToolBar(Qt::BottomToolBarArea, _mainWindow->playToolBar());
}
