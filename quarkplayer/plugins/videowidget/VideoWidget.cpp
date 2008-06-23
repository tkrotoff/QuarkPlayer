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

#include <QtGui/QtGui>

#include <QtCore/QTimerEvent>

VideoWidget::VideoWidget(QStackedWidget * stackedWidget, MainWindow * mainWindow)
	: Phonon::VideoWidget(NULL) {

	_mainWindow = mainWindow;
	_stackedWidget = stackedWidget;

	_statusBar = NULL;

	connect(_mainWindow, SIGNAL(playToolBarAdded(QToolBar *)),
		SLOT(playToolBarAdded(QToolBar *)));

	//Lazy initialization
	_widgetOverFullScreen = NULL;
}

VideoWidget::~VideoWidget() {
}

void VideoWidget::setFullScreenSlot(bool fullScreen) {
	if (fullScreen) {
		//Going fullscreen

		//FIXME QStackedWidget and fullscreen mode are not well together
		//I hope to remove this code in the future...
		//There should be no reason why a widget inside a QStackedWidget could
		//not be fullscreen
		_stackedWidget->removeWidget(this);

		setFullScreen(fullScreen);

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

		setFullScreen(fullScreen);
		_stackedWidget->addWidget(this);
		_stackedWidget->setCurrentWidget(this);

		addPlayToolBarToMainWindow();
		_widgetOverFullScreen->hide();
	}
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent * event) {
	Phonon::VideoWidget::mouseDoubleClickEvent(event);
	ActionCollection::action("fullScreen")->setChecked(!isFullScreen());
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
	widgetOver->resize(widgetUnder->width(), widgetOver->height());

	int x = 0;
	int y = widgetUnder->height() - widgetOver->height();

	widgetOver->move(x, y);
	widgetOver->show();
}

void VideoWidget::checkMousePos() {
	static const int MARGIN = 70;
	static bool bottomCursor = false;

	if (!isFullScreen()) {
		return;
	}

	QToolBar * playToolBar = _mainWindow->playToolBar();
	_statusBar = _mainWindow->statusBar();
	//Computing size of the widgetOverFullScreen
	int widgetOverFullScreenHeight = playToolBar->height() + _statusBar->height();
	_widgetOverFullScreen->setMaximumHeight(widgetOverFullScreenHeight);

	QPoint pos = QCursor::pos();

	if (pos.y() > height() - widgetOverFullScreenHeight) {
		//Going near bottom

		if (!bottomCursor) {
			qDebug() << __FUNCTION__;
			bottomCursor = true;

			//PlayToolBar
			_widgetOverFullScreen->layout()->removeWidget(playToolBar);
			_widgetOverFullScreen->layout()->addWidget(playToolBar);

			//StatusBar
			_widgetOverFullScreen->layout()->removeWidget(_statusBar);
			_widgetOverFullScreen->layout()->addWidget(_statusBar);

			showWidgetOver(_widgetOverFullScreen, this);
		}
	} else {
		//Leaving near bottom

		if (bottomCursor) {
			bottomCursor = false;

			_widgetOverFullScreen->hide();
		}
	}
}

void VideoWidget::playToolBarAdded(QToolBar * playToolBar) {
	//We do this in order to add the play toolbar to the mainwindow
	//when starting
	addPlayToolBarToMainWindow();

	connect(ActionCollection::action("fullScreen"), SIGNAL(toggled(bool)),
		SLOT(setFullScreenSlot(bool)));
}

void VideoWidget::addPlayToolBarToMainWindow() {
	if (_mainWindow->playToolBar()) {
		_mainWindow->addToolBar(Qt::BottomToolBarArea, _mainWindow->playToolBar());
	}

	if (_statusBar) {
		_mainWindow->setStatusBar(_statusBar);
	}
}
