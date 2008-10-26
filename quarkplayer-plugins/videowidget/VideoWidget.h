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

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <phonon/videowidget.h>
#include <phonon/phononnamespace.h>

#include <QtGui/QAction>

#include <QtCore/QBasicTimer>

class MainWindow;

namespace Phonon {
	class MediaObject;
}

class QDockWidget;
class QStatusBar;
class QToolBar;

/**
 * Where the video lies.
 *
 * Inspired from class MediaVideoWidget from mediaplayer example from Trolltech.
 *
 * @author Tanguy Krotoff
 */
class VideoWidget : public Phonon::VideoWidget {
	Q_OBJECT
public:

	VideoWidget(QDockWidget * dockWidget, MainWindow * mainWindow);

	~VideoWidget();

private slots:

	void setFullScreenSlot(bool fullScreen);

	void playToolBarAdded(QToolBar * playToolBar);
	void statusBarAdded(QStatusBar * statusBar);

private:

	void mouseDoubleClickEvent(QMouseEvent * event);

	void keyPressEvent(QKeyEvent * event);

	bool event(QEvent * event);

	void timerEvent(QTimerEvent * event);

	void checkMousePos();

	static void showWidgetOver(QWidget * widgetOver, QWidget * widgetUnder);

	void addPlayToolBarToMainWindow();

	QDockWidget * _dockWidget;

	MainWindow * _mainWindow;

	QToolBar * _playToolBar;

	QStatusBar * _statusBar;

	QBasicTimer _timer;

	/** QWidget that contains PlayToolBar + StatusBar. */
	QWidget * _widgetOverFullScreen;
};

#endif	//VIDEOWIDGET_H
