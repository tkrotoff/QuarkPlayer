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

#include "MyVideoWidget.h"

#include "VideoWidgetLogger.h"

#include <quarkplayer-plugins/MainWindow/MainWindow.h>

#include <TkUtil/Actions.h>
#include <TkUtil/ScreenSaver.h>
#include <TkUtil/TkAction.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QTimerEvent>

MyVideoWidget::MyVideoWidget(QDockWidget * dockWidget, IMainWindow * mainWindow)
	: Phonon::VideoWidget(NULL) {

	Q_ASSERT(dockWidget);
	_dockWidget = dockWidget;
	Q_ASSERT(mainWindow);
	_mainWindow = mainWindow;

	_playToolBar = _mainWindow->playToolBar();
	_statusBar = _mainWindow->statusBar();

	connect(_mainWindow, SIGNAL(playToolBarAdded(QToolBar *)),
		SLOT(playToolBarAdded(QToolBar *)));
	connect(_mainWindow, SIGNAL(statusBarAdded(QStatusBar *)),
		SLOT(statusBarAdded(QStatusBar *)));

	//Lazy initialization
	_widgetOverFullScreen = NULL;

	connect(Actions["Global.FullScreen"], SIGNAL(toggled(bool)),
		SLOT(setFullScreenInternal(bool)));

	//We have to add the QAction to the widget otherwise it won't work
	//From Qt doc:
	//Note that an action must be added to a widget before it can be used;
	//this is also true when the shortcut should be global
	//(i.e., Qt::ApplicationShortcut as Qt::ShortcutContext).
	addAction(Actions["Global.FullScreenExit"]);

	connect(Actions["Global.FullScreenExit"], SIGNAL(triggered()),
		SLOT(triggerFullScreenExitAction()));

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

MyVideoWidget::~MyVideoWidget() {
}

void MyVideoWidget::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	Q_ASSERT(app);

	_actions.add("VideoWidget.AspectRatioAuto", new QAction(app));
	_actions.add("VideoWidget.AspectRatioScale", new QAction(app));
	_actions.add("VideoWidget.AspectRatio16/9", new QAction(app));
	_actions.add("VideoWidget.AspectRatio4/3", new QAction(app));
	_actions.add("VideoWidget.ScaleModeFitInView", new QAction(app));
	_actions.add("VideoWidget.ScaleModeScaleAndCrop", new QAction(app));
}

void MyVideoWidget::retranslate() {
	_actions["VideoWidget.AspectRatioAuto"]->setText(tr("Auto"));
	_actions["VideoWidget.AspectRatioScale"]->setText(tr("Scale"));
	_actions["VideoWidget.AspectRatio16/9"]->setText(tr("16/9"));
	_actions["VideoWidget.AspectRatio4/3"]->setText(tr("4/3"));
	_actions["VideoWidget.ScaleModeFitInView"]->setText(tr("Fit in View"));
	_actions["VideoWidget.ScaleModeScaleAndCrop"]->setText(tr("Scale and Crop"));
}

void MyVideoWidget::createContextMenu() {
	_contextMenu = new QMenu(this);

	_contextMenu->addAction(Actions["Global.PreviousTrack"]);
	_contextMenu->addAction(Actions["Global.PlayPause"]);
	_contextMenu->addAction(Actions["Global.Stop"]);
	_contextMenu->addAction(Actions["Global.NextTrack"]);
	_contextMenu->addAction(Actions["Global.FullScreen"]);

	_contextMenu->addSeparator();

	_contextMenu->addAction(Actions["Global.OpenFile"]);
	_contextMenu->addAction(Actions["Global.OpenURL"]);
	_contextMenu->addAction(Actions["Global.OpenDVD"]);

	_contextMenu->addSeparator();

	QMenu * aspectRatioMenu = _contextMenu->addMenu(tr("&Aspect Ratio"));
	QActionGroup * aspectRatioGroup = new QActionGroup(aspectRatioMenu);
	connect(aspectRatioGroup, SIGNAL(triggered(QAction *)), SLOT(aspectRatioChanged(QAction *)));
	aspectRatioGroup->setExclusive(true);

	QAction * action = _actions["VideoWidget.AspectRatioAuto"];
	action->setCheckable(true);
	action->setChecked(true);
	aspectRatioMenu->addAction(action);
	aspectRatioGroup->addAction(action);

	action = _actions["VideoWidget.AspectRatioScale"];
	action->setCheckable(true);
	aspectRatioMenu->addAction(action);
	aspectRatioGroup->addAction(action);

	action = _actions["VideoWidget.AspectRatio16/9"];
	action->setCheckable(true);
	aspectRatioMenu->addAction(action);
	aspectRatioGroup->addAction(action);

	action = _actions["VideoWidget.AspectRatio4/3"];
	action->setCheckable(true);
	aspectRatioMenu->addAction(action);
	aspectRatioGroup->addAction(action);

	QMenu * scaleModeMenu = _contextMenu->addMenu(tr("&Scale Mode"));
	QActionGroup * scaleModeGroup = new QActionGroup(scaleModeMenu);
	connect(scaleModeGroup, SIGNAL(triggered(QAction *)), SLOT(scaleModeChanged(QAction *)));
	scaleModeGroup->setExclusive(true);

	action = _actions["VideoWidget.ScaleModeFitInView"];
	action->setCheckable(true);
	action->setChecked(true);
	scaleModeMenu->addAction(action);
	scaleModeGroup->addAction(action);

	action = _actions["VideoWidget.ScaleModeScaleAndCrop"];
	action->setCheckable(true);
	scaleModeMenu->addAction(action);
	scaleModeGroup->addAction(action);

	_contextMenu->addSeparator();

	_contextMenu->addAction(Actions["Global.Quit"]);
}

void MyVideoWidget::showContextMenu(const QPoint & pos) {
	_contextMenu->popup(isFullScreen() ? pos : mapToGlobal(pos));
}

void MyVideoWidget::scaleModeChanged(QAction * action) {
	if (action == _actions["VideoWidget.ScaleModeFitInView"]) {
		setScaleMode(Phonon::VideoWidget::FitInView);
	} else if (action == _actions["VideoWidget.ScaleModeScaleAndCrop"]) {
		setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
	} else {
		VideoWidgetCritical() << "Unknown action:" << action->text();
	}
}

void MyVideoWidget::aspectRatioChanged(QAction * action) {
	if (action == _actions["VideoWidget.AspectRatio16/9"]) {
		setAspectRatio(Phonon::VideoWidget::AspectRatio16_9);
	} else if (action == _actions["VideoWidget.AspectRatioScale"]) {
		setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
	} else if (action == _actions["VideoWidget.AspectRatio4/3"]) {
		setAspectRatio(Phonon::VideoWidget::AspectRatio4_3);
	} else if (action == _actions["VideoWidget.AspectRatioAuto"]) {
		setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
	} else {
		VideoWidgetCritical() << "Unknown action:" << action->text();
	}
}

void MyVideoWidget::triggerFullScreenExitAction() {
	Actions["Global.FullScreen"]->setChecked(false);
}

void MyVideoWidget::enterFullScreenInternal() {
	if (isFullScreen()) {
		return;
	}

	//Going fullscreen

	VideoWidgetDebug() << "Enter fullscreen";

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
		VideoWidgetDebug() << "_widgetOverFullScreen:" << _widgetOverFullScreen;

		_widgetOverFullScreen = new QWidget(NULL);
		_widgetOverFullScreen->setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
			Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

		QVBoxLayout * layout = new QVBoxLayout();
		layout->setSpacing(0);
		layout->setMargin(0);
		layout->setContentsMargins(0, 0, 0, 0);
		_widgetOverFullScreen->setLayout(layout);
	}
}

void MyVideoWidget::exitFullScreenInternal() {
	if (!isFullScreen()) {
		return;
	}

	VideoWidgetDebug() << "Exit fullscreen";

	//Restore screensaver
	ScreenSaver::restore();

	exitFullScreen();
	_dockWidget->setWidget(this);

	//Re-initializes the toolbars
	playToolBarAdded(_playToolBar);
	statusBarAdded(_statusBar);
	///

	_widgetOverFullScreen->hide();
}

void MyVideoWidget::setFullScreenInternal(bool fullScreen) {
	if (fullScreen) {
		enterFullScreenInternal();
	} else {
		exitFullScreenInternal();
	}
}

void MyVideoWidget::mouseDoubleClickEvent(QMouseEvent * event) {
	if (event->button() == Qt::LeftButton) {
		event->accept();
		Actions["Global.FullScreen"]->toggle();
	} else {
		event->ignore();
	}

	Phonon::VideoWidget::mouseDoubleClickEvent(event);
}

void MyVideoWidget::mouseMoveEvent(QMouseEvent * event) {
	event->accept();
	if (isFullScreen()) {
		checkMousePos();
		_timer.start(1000, this);
	}
	unsetCursor();

	Phonon::VideoWidget::mouseMoveEvent(event);
}

bool MyVideoWidget::event(QEvent * event) {
	//QEvent::WindowStateChange cannot be in its own event method
	//like mouseMoveEvent() and mouseDoubleClickEvent()
	//I would rather prefer to have a method named windowStateChangeEvent()
	if (event->type() == QEvent::WindowStateChange) {
		event->accept();
		if (isFullScreen()) {
			_timer.start(1000, this);
		} else {
			_timer.stop();
			unsetCursor();
		}
	}

	return Phonon::VideoWidget::event(event);
}

void MyVideoWidget::timerEvent(QTimerEvent * event) {
	if (event->timerId() == _timer.timerId()) {
		event->accept();
		//Let's store the cursor shape
		setCursor(Qt::BlankCursor);
	}

	Phonon::VideoWidget::timerEvent(event);
}

void MyVideoWidget::showWidgetOver(QWidget * widgetOver, QWidget * widgetUnder) {
	//Width divided by 1.5, I think it's better
	//otherwise the widget (playToolBar + statusBar) is too big
	int widgetOverWidth = static_cast<int>(widgetUnder->width() / 1.5);
	widgetOver->resize(widgetOverWidth, widgetOver->height());

	//Center of the screen
	int x = (widgetUnder->width() - widgetOverWidth) / 2;
	//Bottom of the screen
	int y = widgetUnder->height() - widgetOver->height();

	widgetOver->move(x, y);

	widgetOver->show();
}

void MyVideoWidget::checkMousePos() {
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

void MyVideoWidget::playToolBarAdded(QToolBar * playToolBar) {
	_playToolBar = playToolBar;
	if (_playToolBar) {
		_mainWindow->addToolBar(Qt::BottomToolBarArea, _playToolBar);
	}
}

void MyVideoWidget::statusBarAdded(QStatusBar * statusBar) {
	_statusBar = statusBar;
	if (_statusBar) {
		_mainWindow->setStatusBar(_statusBar);
	}
}
