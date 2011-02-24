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

#ifndef MYVIDEOWIDGET_H
#define MYVIDEOWIDGET_H

#include <TkUtil/ActionCollection.h>

#include <phonon/videowidget.h>
#include <phonon/phononnamespace.h>

#include <QtGui/QAction>

#include <QtCore/QBasicTimer>

class IMainWindow;

namespace Phonon {
	class MediaObject;
}

class QDockWidget;
class QStatusBar;
class QToolBar;

/**
 * Widget where the video lies.
 *
 * Inspired from class MediaVideoWidget from Trolltech' mediaplayer example.
 *
 * Cannot be simply named VideoWidget otherwise Doxygen confuses it with Phonon::VideoWidget,
 * thus was renamed MyVideoWidget.
 *
 * @author Tanguy Krotoff
 */
class MyVideoWidget : public Phonon::VideoWidget {
	Q_OBJECT
public:

	MyVideoWidget(QDockWidget * dockWidget, IMainWindow * mainWindow);

	~MyVideoWidget();

public slots:

	/**
	 * Triggers action FullScreenExit.
	 *
	 * Simulates user clicking on the exit full screen button.
	 * Call by VideoWidgetPlugin when the current playing media object finishes.
	 */
	void triggerFullScreenExitAction();

private slots:

	/**
	 * Enters or exits full screen mode.
	 *
	 * @param fullScreen if true then call enterFullScreenInternal(); if false call exitFullScreenInternal()
	 */
	void setFullScreenInternal(bool fullScreen);

	void playToolBarAdded(QToolBar * playToolBar);
	void statusBarAdded(QStatusBar * statusBar);

	/**
	 * Shows the context menu on the video when user clicks
	 * using right button of the mouse.
	 *
	 * @param pos current position of the mouse cursor
	 */
	void showContextMenu(const QPoint & pos);

	void scaleModeChanged(QAction * action);
	void aspectRatioChanged(QAction * action);

	void retranslate();

private:

	/**
	 * Enters full screen mode.
	 */
	void enterFullScreenInternal();

	/**
	 * Exits full screen mode.
	 */
	void exitFullScreenInternal();

	/** Catches QEvent::WindowStateChange. */
	bool event(QEvent * event);

	/** When user double click on the video it gets fullscreen. */
	void mouseDoubleClickEvent(QMouseEvent * event);

	/** When user moves the mouse then the mouse cursor should appear. */
	void mouseMoveEvent(QMouseEvent * event);

	void timerEvent(QTimerEvent * event);

	/**
	 * Checks current mouse cursor position.
	 */
	void checkMousePos();

	static void showWidgetOver(QWidget * widgetOver, QWidget * widgetUnder);

	/**
	 * Creates the context menu of the video.
	 *
	 * This is the context menu that appears when user clicks
	 * using right button of the mouse.
	 */
	void createContextMenu();

	void populateActionCollection();

	QDockWidget * _dockWidget;

	IMainWindow * _mainWindow;

	QToolBar * _playToolBar;

	QStatusBar * _statusBar;

	QBasicTimer _timer;

	/** QWidget that contains PlayToolBar + StatusBar. */
	QWidget * _widgetOverFullScreen;

	/**
	 * Context menu of the video.
	 *
	 * This is the context menu that appears when user clicks
	 * using right button of the mouse.
	 */
	QMenu * _contextMenu;

	ActionCollection _actions;
};

#endif	//MYVIDEOWIDGET_H
