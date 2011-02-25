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

#ifndef IMAINWINDOW_H
#define IMAINWINDOW_H

#include <quarkplayer-plugins/MainWindow/MainWindowExport.h>

#include <quarkplayer/PluginInterface.h>

#include <TkUtil/TkMainWindow.h>

class QDockWidget;

/**
 * Interface for MainWindowQuarkPlayer main window.
 *
 * Facade pattern.
 *
 * @see QMainWindow
 * @author Tanguy Krotoff
 */
class MAINWINDOW_API IMainWindow : public TkMainWindow, public PluginInterface {
	Q_OBJECT
public:

	IMainWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	virtual ~IMainWindow();

	virtual void setPlayToolBar(QToolBar * playToolBar) = 0;
	virtual QToolBar * playToolBar() const = 0;

	virtual QMenu * menuFile() const = 0;
	virtual QMenu * menuPlay() const = 0;
	virtual QMenu * menuAudio() const = 0;
	virtual QMenu * menuSettings() const = 0;
	virtual QMenu * menuHelp() const = 0;

	virtual void addBrowserDockWidget(QDockWidget * dockWidget) = 0;

	virtual void addVideoDockWidget(QDockWidget * dockWidget) = 0;

	virtual void addPlaylistDockWidget(QDockWidget * dockWidget) = 0;

	/** HACK */
	virtual QPair<QTabBar *, int> findDockWidgetTab(QDockWidget * dockWidget) {
		Q_UNUSED(dockWidget);
		QPair<QTabBar *, int> result;
		return result;
	}

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
};

#endif	//IMAINWINDOW_H
