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

#include <quarkplayer/MainWindow.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/ScreenSaver.h>

#include <QtGui/QtGui>

#include <QtCore/QTimerEvent>

VideoWidget::VideoWidget(QDockWidget * dockWidget, MainWindow * mainWindow)
	: Phonon::VideoWidget(NULL) {

	_dockWidget = dockWidget;
	_mainWindow = mainWindow;

	_playToolBar = _mainWindow->playToolBar();
	_statusBar = _mainWindow->statusBar();

	connect(_mainWindow, SIGNAL(playToolBarAdded(QToolBar *)),
		SLOT(playToolBarAdded(QToolBar *)));
	connect(_mainWindow, SIGNAL(statusBarAdded(QStatusBar *)),
		SLOT(statusBarAdded(QStatusBar *)));

	//Lazy initialization
	_widgetOverFullScreen = NULL;

	if (_playToolBar) {
		playToolBarAdded(_playToolBar);
	}
}

VideoWidget::~VideoWidget() {
}

void VideoWidget::setFullScreenSlot(bool fullScreen) {
	if (fullScreen) {
		//Going fullscreen

		//Disable screensaver
		ScreenSaver::disable();

		//Bugfix: when going fullscreen, dockWidget does not have any child widget
		//and thus get closed. In order to avoid this, we set a fake child widget to dockWidget
		_dockWidget->setWidget(new QLabel("Fullscreen bugfix"));
		setFullScreen(fullScreen);
		show();
		//

		//QWidget that contains PlayToolBar + StatusBar
		//Lazy initialization
		if (!_widgetOverFullScreen) {
			_widgetOverFullScreen = new QWidget(NULL);
			_widgetOverFullScreen->setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
				Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

			QVBoxLayout * layout = new QVBoxLayout();
			layout->setContentsMargins(0, 0, 0, 0);
			_widgetOverFullScreen->setLayout(layout);
		}
	} else {
		//Leaving fullscreen

		//Restore screensaver
		ScreenSaver::restore();

		setFullScreen(fullScreen);
		_dockWidget->setWidget(this);

		addPlayToolBarToMainWindow();
		_widgetOverFullScreen->hide();
	}
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent * event) {
	Phonon::VideoWidget::mouseDoubleClickEvent(event);
	QAction * fullScreen = ActionCollection::action("fullScreen");
	if (fullScreen) {
		fullScreen->setChecked(!isFullScreen());
	} else {
		setFullScreenSlot(!isFullScreen());
	}
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

void VideoWidget::showWidgetOver(QWidget * widgetOver, QWidget * widgetUnder) {
	//Width divided by 1.5, I think it's better
	//otherwise the widget (playToolBar + statusBar) is too big
	//So same as VLC...
	int widgetOverWidth = widgetUnder->width() / 1.5;
	widgetOver->resize(widgetOverWidth, widgetOver->height());

	//Center of the screen
	int x = (widgetUnder->width() - widgetOverWidth) / 2;
	//Bottom of the screen
	int y = widgetUnder->height() - widgetOver->height();

	widgetOver->move(x, y);

	widgetOver->show();
}

void VideoWidget::checkMousePos() {
	static bool bottomCursor = false;

	if (!isFullScreen()) {
		return;
	}

	//playToolBar can be NULL
	int playToolBarHeight = 0;
	if (_playToolBar) {
		playToolBarHeight = _playToolBar->height();
	}

	//statusBar can be NULL
	int statusBarHeight = 0;
	if (_statusBar) {
		statusBarHeight = _statusBar->height();
	}

	//Computing size of the _widgetOverFullScreen
	int widgetOverFullScreenHeight = playToolBarHeight + statusBarHeight;
	_widgetOverFullScreen->setMaximumHeight(widgetOverFullScreenHeight);

	QPoint pos = QCursor::pos();

	if (pos.y() > height() - widgetOverFullScreenHeight) {
		//Going "near bottom" area

		if (!bottomCursor) {
			qDebug() << __FUNCTION__;
			bottomCursor = true;

			//PlayToolBar
			if (_playToolBar) {
				_widgetOverFullScreen->layout()->removeWidget(_playToolBar);
				_widgetOverFullScreen->layout()->addWidget(_playToolBar);
			}

			//StatusBar
			if (_statusBar) {
				_widgetOverFullScreen->layout()->removeWidget(_statusBar);
				_widgetOverFullScreen->layout()->addWidget(_statusBar);
			}

			showWidgetOver(_widgetOverFullScreen, this);
		}
	} else {
		//Leaving "near bottom" area

		if (bottomCursor) {
			bottomCursor = false;

			_widgetOverFullScreen->hide();
		}
	}
}

void VideoWidget::playToolBarAdded(QToolBar * playToolBar) {
	qDebug() << __FUNCTION__;

	_playToolBar = playToolBar;

	//We do this in order to add the play toolbar to the mainwindow
	//when starting
	addPlayToolBarToMainWindow();

	connect(ActionCollection::action("fullScreen"), SIGNAL(toggled(bool)),
		SLOT(setFullScreenSlot(bool)));
}

void VideoWidget::statusBarAdded(QStatusBar * statusBar) {
	_statusBar = statusBar;
}

void VideoWidget::addPlayToolBarToMainWindow() {
	if (_playToolBar) {
		_mainWindow->addToolBar(Qt::BottomToolBarArea, _playToolBar);
	}

	if (_statusBar) {
		_mainWindow->setStatusBar(_statusBar);
	}
}
