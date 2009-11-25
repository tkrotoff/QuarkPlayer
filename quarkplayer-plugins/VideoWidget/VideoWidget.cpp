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

#include "VideoWidget.h"

#include <quarkplayer-plugins/mainwindow/MainWindow.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/ScreenSaver.h>
#include <tkutil/TkAction.h>
#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>

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

	populateActionCollection();

	setContextMenuPolicy(Qt::CustomContextMenu);
	createContextMenu();
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
		SLOT(showContextMenu(const QPoint &)));

	RETRANSLATE(this);
	retranslate();
}

VideoWidget::~VideoWidget() {
}

void VideoWidget::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	TkAction * action = new TkAction(app);
	ActionCollection::addAction("VideoWidget.AspectRatioAuto", action);

	action = new TkAction(app);
	ActionCollection::addAction("VideoWidget.AspectRatioScale", action);

	action = new TkAction(app);
	ActionCollection::addAction("VideoWidget.AspectRatio16/9", action);

	action = new TkAction(app);
	ActionCollection::addAction("VideoWidget.AspectRatio4/3", action);

	action = new TkAction(app);
	ActionCollection::addAction("VideoWidget.ScaleModeFitInView", action);

	action = new TkAction(app);
	ActionCollection::addAction("VideoWidget.ScaleModeScaleAndCrop", action);
}

void VideoWidget::retranslate() {
	ActionCollection::action("VideoWidget.AspectRatioAuto")->setText(tr("Auto"));
	ActionCollection::action("VideoWidget.AspectRatioScale")->setText(tr("Scale"));
	ActionCollection::action("VideoWidget.AspectRatio16/9")->setText(tr("16/9"));
	ActionCollection::action("VideoWidget.AspectRatio4/3")->setText(tr("4/3"));
	ActionCollection::action("VideoWidget.ScaleModeFitInView")->setText(tr("Fit in View"));
	ActionCollection::action("VideoWidget.ScaleModeScaleAndCrop")->setText(tr("Scale and Crop"));
}

void VideoWidget::createContextMenu() {
	_contextMenu = new QMenu(this);

	_contextMenu->addAction(ActionCollection::action("MainWindow.PreviousTrack"));
	_contextMenu->addAction(ActionCollection::action("MainWindow.PlayPause"));
	_contextMenu->addAction(ActionCollection::action("MainWindow.Stop"));
	_contextMenu->addAction(ActionCollection::action("MainWindow.NextTrack"));
	_contextMenu->addAction(ActionCollection::action("MainWindow.FullScreen"));

	_contextMenu->addSeparator();

	_contextMenu->addAction(ActionCollection::action("MainWindow.OpenFile"));
	_contextMenu->addAction(ActionCollection::action("MainWindow.OpenDVD"));
	_contextMenu->addAction(ActionCollection::action("MainWindow.OpenURL"));

	_contextMenu->addSeparator();

	QMenu * aspectRatioMenu = _contextMenu->addMenu(tr("&Aspect Ratio"));
	QActionGroup * aspectRatioGroup = new QActionGroup(aspectRatioMenu);
	connect(aspectRatioGroup, SIGNAL(triggered(QAction *)), SLOT(aspectRatioChanged(QAction *)));
	aspectRatioGroup->setExclusive(true);

	QAction * action = ActionCollection::action("VideoWidget.AspectRatioAuto");
	action->setCheckable(true);
	action->setChecked(true);
	aspectRatioMenu->addAction(action);
	aspectRatioGroup->addAction(action);

	action = ActionCollection::action("VideoWidget.AspectRatioScale");
	action->setCheckable(true);
	aspectRatioMenu->addAction(action);
	aspectRatioGroup->addAction(action);

	action = ActionCollection::action("VideoWidget.AspectRatio16/9");
	action->setCheckable(true);
	aspectRatioMenu->addAction(action);
	aspectRatioGroup->addAction(action);

	action = ActionCollection::action("VideoWidget.AspectRatio4/3");
	action->setCheckable(true);
	aspectRatioMenu->addAction(action);
	aspectRatioGroup->addAction(action);

	QMenu * scaleModeMenu = _contextMenu->addMenu(tr("&Scale Mode"));
	QActionGroup * scaleModeGroup = new QActionGroup(scaleModeMenu);
	connect(scaleModeGroup, SIGNAL(triggered(QAction *)), SLOT(scaleModeChanged(QAction *)));
	scaleModeGroup->setExclusive(true);

	action = ActionCollection::action("VideoWidget.ScaleModeFitInView");
	action->setCheckable(true);
	action->setChecked(true);
	scaleModeMenu->addAction(action);
	scaleModeGroup->addAction(action);

	action = ActionCollection::action("VideoWidget.ScaleModeScaleAndCrop");
	action->setCheckable(true);
	scaleModeMenu->addAction(action);
	scaleModeGroup->addAction(action);

	_contextMenu->addSeparator();

	_contextMenu->addAction(ActionCollection::action("MainWindow.Quit"));
}

void VideoWidget::showContextMenu(const QPoint & pos) {
	_contextMenu->popup(isFullScreen() ? pos : mapToGlobal(pos));
}

void VideoWidget::scaleModeChanged(QAction * action) {
	if (action == ActionCollection::action("VideoWidget.ScaleModeFitInView")) {
		setScaleMode(Phonon::VideoWidget::FitInView);
	} else if (action == ActionCollection::action("VideoWidget.ScaleModeScaleAndCrop")) {
		setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
	} else {
		qCritical() << __FUNCTION__ << "Error: unknown action:" << action->text();
	}
}

void VideoWidget::aspectRatioChanged(QAction * action) {
	if (action == ActionCollection::action("VideoWidget.AspectRatio16/9")) {
		setAspectRatio(Phonon::VideoWidget::AspectRatio16_9);
	} else if (action == ActionCollection::action("VideoWidget.AspectRatioScale")) {
		setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
	} else if (action == ActionCollection::action("VideoWidget.AspectRatio4/3")) {
		setAspectRatio(Phonon::VideoWidget::AspectRatio4_3);
	} else if (action == ActionCollection::action("VideoWidget.AspectRatioAuto")) {
		setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
	} else {
		qCritical() << __FUNCTION__ << "Error: unknown action:" << action->text();
	}
}

void VideoWidget::enterFullScreenSlot() {
	if (isFullScreen()) {
		return;
	}

	//Going fullscreen

	//Disable screensaver
	ScreenSaver::disable();

	//Bugfix: when going fullscreen, dockWidget does not have any child widget
	//and thus get closed. In order to avoid this, we set a fake child widget to dockWidget
	_dockWidget->setWidget(new QLabel("Fullscreen bugfix"));
	enterFullScreen();
	show();
	//

	//QWidget that contains PlayToolBar + StatusBar
	//Lazy initialization
	if (!_widgetOverFullScreen) {
		_widgetOverFullScreen = new QWidget(NULL);
		_widgetOverFullScreen->setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
			Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

		QVBoxLayout * layout = new QVBoxLayout();
		layout->setSpacing(0);
		layout->setMargin(0);
		layout->setContentsMargins(0, 0, 0, 0);
		_widgetOverFullScreen->setLayout(layout);
	}

	QAction * fullScreen = ActionCollection::action("MainWindow.FullScreen");
	fullScreen->setChecked(true);
}

void VideoWidget::leaveFullScreenSlot() {
	if (!isFullScreen()) {
		return;
	}

	//Leaving fullscreen

	//Restore screensaver
	ScreenSaver::restore();

	exitFullScreen();
	_dockWidget->setWidget(this);

	addPlayToolBarToMainWindow();
	_widgetOverFullScreen->hide();

	QAction * fullScreen = ActionCollection::action("MainWindow.FullScreen");
	fullScreen->setChecked(false);
}

void VideoWidget::setFullScreenSlot(bool fullScreen) {
	if (fullScreen) {
		enterFullScreenSlot();
	} else {
		leaveFullScreenSlot();
	}
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent * event) {
	Phonon::VideoWidget::mouseDoubleClickEvent(event);
	setFullScreenSlot(!isFullScreen());
}

bool VideoWidget::event(QEvent * event) {
	switch(event->type()) {
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
	int widgetOverWidth = static_cast<int>(widgetUnder->width() / 1.5);
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

	connect(ActionCollection::action("MainWindow.FullScreen"), SIGNAL(toggled(bool)),
		SLOT(setFullScreenSlot(bool)));

	connect(ActionCollection::action("MainWindow.FullScreenLeave"), SIGNAL(triggered()),
		SLOT(leaveFullScreenSlot()));
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
