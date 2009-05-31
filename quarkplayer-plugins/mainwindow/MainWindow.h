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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <quarkplayer-plugins/mainwindow/mainwindow_export.h>

#include <quarkplayer/PluginInterface.h>

#include <tkutil/TkMainWindow.h>
#include <tkutil/TkToolBar.h>

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
class MAINWINDOW_API MainWindow : public TkMainWindow, public PluginInterface {
	Q_OBJECT
public:

	MainWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~MainWindow();

	void setPlayToolBar(QToolBar * playToolBar);
	QToolBar * playToolBar() const;

	void setStatusBar(QStatusBar * statusBar);
	QStatusBar * statusBar() const;

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
	 * Signal emitted when files should be added to the current playlist.
	 *
	 * This signal should be catched by a playlist plugin if any exist.
	 *
	 * @param files list of files to be added
	 * @see playFile()
	 */
	void addFilesToCurrentPlaylist(const QStringList & files);

	/**
	 * A subtitle file has been dropped inside the main window.
	 */
	void subtitleFileDropped(const QString & subtitle);

	/**
	 * The play toolbar has been added to the main window.
	 *
	 * Specific to the PlayToolBar plugin.
	 *
	 * @see setPlayToolBar()
	 * @see PlayToolBar
	 * @param main window play toolbar
	 */
	void playToolBarAdded(QToolBar * playToolBar);

	/**
	 * The status bar has been added to the main window.
	 *
	 * @param main window status bar (cannot be NULL or there is a bug...)
	 * @see setStatusBar()
	 */
	void statusBarAdded(QStatusBar * statusBar);

public slots:

	/**
	 * Play a media source using the current media object.
	 *
	 * @see QuarkPlayer::play()
	 */
	void play(const Phonon::MediaSource & mediaSource);

	/** Adds a file to the list of recent files that have been played. */
	void addFileToRecentFilesMenu(const Phonon::MediaSource & mediaSource);

private slots:

	void reportBug();
	void showMailingList();

	void about();

	void playFile();
	void playDVD();
	void playURL();
	void playVCD();

	void addRecentFilesToMenu();
	void playRecentFile(int id);
	void clearRecentFiles();

	void updateWindowTitle();

	void retranslate();

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

	/**
	 * Volume output has been muted.
	 */
	void mutedChanged(bool muted);

	void mutedToggled(bool muted);

private:

	void populateActionCollection();

	void setupUi();

	/** Internal factorization code. */
	void addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockWidget, QDockWidget * lastDockWidget);

	void dragEnterEvent(QDragEnterEvent * event);

	void dropEvent(QDropEvent * event);

	void closeEvent(QCloseEvent * event);

	TkToolBar * _mainToolBar;
	QMenu * _menuRecentFiles;
	QMenu * _menuFile;
	QMenu * _menuPlay;
	QMenu * _menuAudio;
	QMenu * _menuSettings;
	QMenu * _menuHelp;

	QToolBar * _playToolBar;
	QStatusBar * _statusBar;
};

#include <quarkplayer/PluginFactory.h>

class MAINWINDOW_API MainWindowFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	static const char * PLUGIN_NAME;

	QString name() const { return PLUGIN_NAME; }
	QStringList dependencies() const;
	QString description() const { return tr("Main window"); }
	QString version() const { return "0.0.1"; }
	QString url() const { return "http://quarkplayer.googlecode.com/"; }
	QString vendor() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;

	static MainWindow * mainWindow();
};

#endif	//MAINWINDOW_H
