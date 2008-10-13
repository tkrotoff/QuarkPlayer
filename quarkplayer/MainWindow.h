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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <quarkplayer/quarkplayer_export.h>
#include <quarkplayer/PluginInterface.h>

#include <tkutil/TkMainWindow.h>
#include <tkutil/TkToolBar.h>

#include <QtGui/QMainWindow>

class QuarkPlayer;
class ConfigWindow;

namespace Phonon {
	class MediaSource;
	class MediaObject;
}

class QDockWidget;

/**
 * Main interface, the main window: QMainWindow.
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API MainWindow : public TkMainWindow, public PluginInterface {
	Q_OBJECT
public:

	MainWindow(QuarkPlayer & quarkPlayer, QWidget * parent);

	~MainWindow();

	void setPlayToolBar(QToolBar * playToolBar);
	QToolBar * playToolBar() const;

	void setStatusBar(QStatusBar * statusBar);
	QStatusBar * statusBar() const;

	QMenu * menuFile() const;
	QMenu * menuPlay() const;
	QMenu * menuSettings() const;
	QMenu * menuHelp() const;

	void addBrowserDockWidget(QDockWidget * widget);
	void removeBrowserDockWidget(QDockWidget * widget);

	void addVideoDockWidget(QDockWidget * widget);

	void addPlaylistDockWidget(QDockWidget * widget);


	QString name() const { return tr("Main window"); }
	QString description() const { return tr("Main window"); }
	QString version() const { return "0.0.1"; }
	QString webpage() const { return "http://quarkplayer.googlecode.com/"; }
	QString email() const { return "quarkplayer@googlegroups.com"; }
	QString authors() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3"; }
	QString copyright() const { return "Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>"; }

signals:

	void subtitleFileDropped(const QString & subtitle);

	void playToolBarAdded(QToolBar * playToolBar);

	void statusBarAdded(QStatusBar * statusBar);

	/**
	 * The ConfigWindow has been created, meaning user asked for the configuration window to be showed.
	 *
	 * This usefull for plugins with ConfigWidget inside the ConfigWindow.
	 * This signal is only sent once (configuration window is constructed only once and then
	 * show and hide each time the user wants it).
	 *
	 * Example:
	 * <pre>
	 * #include <quarkplayer/config/ConfigWindow.h>
	 * #include <quarkplayer/MainWindow.h>
	 *
	 * TotoPlugin::TotoPlugin(QuarkPlayer & quarkPlayer) {
	 * 	connect(&quarkPlayer.mainWindow(), SIGNAL(configWindowCreated(ConfigWindow *)),
	 * 		SLOT(configWindowCreated(ConfigWindow *)));
	 * }
	 *
	 * TotoPlugin::configWindowCreated(ConfigWindow * configWindow) {
	 * 	configWindow->addConfigWidget(new TotoPluginConfigWidget());
	 * }
	 * </pre>
	 *
	 * @param configWindow the configuration window that has been created
	 */
	void configWindowCreated(ConfigWindow * configWindow);

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

	void about();

	void showConfigWindow();

	void playFile();
	void playDVD();
	void playURL();

	void addRecentFilesToMenu();
	void playRecentFile(int id);
	void clearRecentFiles();

	void updateWindowTitle();

	void retranslate();

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

private:

	void populateActionCollection();

	void setupUi();

	void createDockWidgets();

	void dragEnterEvent(QDragEnterEvent * event);

	void dropEvent(QDropEvent * event);

	void closeEvent(QCloseEvent * event);

	TkToolBar * _mainToolBar;
	QMenu * _menuRecentFiles;
	QMenu * _menuFile;
	QMenu * _menuPlay;
	QMenu * _menuSettings;
	QMenu * _menuHelp;

	QToolBar * _playToolBar;
	QStatusBar * _statusBar;

	QDockWidget * _videoDockWidget;

	QDockWidget * _browserDockWidget;

	QDockWidget * _playlistDockWidget;
};

#endif	//MAINWINDOW_H
