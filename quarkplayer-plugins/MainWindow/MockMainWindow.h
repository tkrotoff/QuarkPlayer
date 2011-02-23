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

#ifndef MOCKMAINWINDOW_H
#define MOCKMAINWINDOW_H

#include <quarkplayer-plugins/MainWindow/IMainWindow.h>

#include <QtGui/QMainWindow>

class QuarkPlayer;

namespace Phonon {
	class MediaSource;
	class MediaObject;
}

class QDockWidget;

/**
 * QuarkPlayer main window.
 *
 * Facade pattern.
 *
 * @see QMainWindow
 * @see QuarkPlayer
 * @see http://en.wikipedia.org/wiki/Facade_pattern
 * @author Tanguy Krotoff
 */
class MockMainWindow : public IMainWindow {
	Q_OBJECT
public:

	MockMainWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~MockMainWindow();

	void setPlayToolBar(QToolBar * playToolBar);
	QToolBar * playToolBar() const;

	QMenu * menuFile() const;
	QMenu * menuPlay() const;
	QMenu * menuAudio() const;
	QMenu * menuSettings() const;
	QMenu * menuHelp() const;

	void addBrowserDockWidget(QDockWidget * dockWidget);
	void resetBrowserDockWidget();

	void addVideoDockWidget(QDockWidget * dockWidget);
	void resetVideoDockWidget();

	void addPlaylistDockWidget(QDockWidget * dockWidget);
	void resetPlaylistDockWidget();

signals:

	/**
	 * The play toolbar has been added to the main window.
	 *
	 * Specific to the PlayToolBar plugin.
	 *
	 * @see setPlayToolBar()
	 * @see PlayToolBar
	 * @param playToolBar main window play toolbar (cannot be NULL or there is a bug...)
	 */
	void playToolBarAdded(QToolBar * playToolBar);

	/**
	 * The status bar has been added to the main window.
	 *
	 * @param statusBar main window status bar (cannot be NULL or there is a bug...)
	 * @see setStatusBar()
	 */
	void statusBarAdded(QStatusBar * statusBar);

private slots:

	void about();

	void showLog();

	void playFile();

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

private:

	void populateActionCollection();

	void setupUi();

	/** Internal factorization code. */
	void addDockWidget(Qt::DockWidgetArea area, QDockWidget * lastDockWidget, QDockWidget * dockWidget);

	void closeEvent(QCloseEvent * event);

	QMenu * _menuFile;
	QMenu * _menuPlay;
	QMenu * _menuHelp;

	QToolBar * _playToolBar;
};

#endif	//MOCKMAINWINDOW_H
