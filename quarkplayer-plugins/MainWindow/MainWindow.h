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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
class MAINWINDOW_API MainWindow : public IMainWindow {
	Q_OBJECT
public:

	MainWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~MainWindow();

	void setPlayToolBar(QToolBar * playToolBar);
	QToolBar * playToolBar() const;

	QToolBar * mainToolBar() const;

	void setStatusBar(QStatusBar * statusBar);
	QStatusBar * statusBar() const;

	QMenu * menuFile() const;
	QMenu * menuPlay() const;
	QMenu * menuAudio() const;
	QMenu * menuSettings() const;
	QMenu * menuHelp() const;

	void addBrowserDockWidget(QDockWidget * dockWidget);

	void addVideoDockWidget(QDockWidget * dockWidget);

	void addPlaylistDockWidget(QDockWidget * dockWidget);


	/** HACK */
	QPair<QTabBar *, int> findDockWidgetTab(QDockWidget * dockWidget);

signals:

	/**
	 * A subtitle file has been dropped by the user inside the main window.
	 */
	void subtitleFileDropped(const QString & fileName);

	/**
	 * Several files have been opened by the user.
	 *
	 * The first file from the list is already being played.
	 */
	void filesOpened(const QStringList & files);

	/**
	 * The status bar has been added to the main window.
	 *
	 * @param statusBar main window status bar (cannot be NULL or there is a bug...)
	 * @see setStatusBar()
	 */
	void statusBarAdded(QStatusBar * statusBar);

private slots:

	void reportBug();
	void showMailingList();

	void showLog();

	void about();

	void playFile();
	void playDVD();
	void playURL();
	void playVCD();

	void updateWindowTitle();

	void retranslate();

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

	/**
	 * Volume output has been muted.
	 */
	void mutedChanged(bool muted);

	void mutedToggled(bool muted);

	/** HACK */
	void tabCloseRequested(int index);

private:

	void populateActionCollection();

	void setupUi();

	/**
	 * Play a media source using the current media object.
	 *
	 * Code factorization.
	 *
	 * @see QuarkPlayer::play()
	 */
	void play(const Phonon::MediaSource & mediaSource);

	/** Internal factorization code. */
	void addDockWidget(Qt::DockWidgetArea area, QDockWidget * lastDockWidget, QDockWidget * dockWidget);

	/** HACK get all QTabBar of the QDockWidgets and modify them. */
	void hackDockWidgetTabBar();

	/** HACK */
	QDockWidget * findDockWidget(QTabBar * tabBar, int index);

	void dragEnterEvent(QDragEnterEvent * event);

	void dropEvent(QDropEvent * event);

	void closeEvent(QCloseEvent * event);

	/**
	 * Asks for the DVDROM or CDROM path.
	 *
	 * @return DVDROM/CDROM path depending on the platform (can be a directory or a file)
	 */
	QString getDiscPath(const QString & defaultPath, const QString & windowTitle);

	QMenu * _menuFile;
	QMenu * _menuPlay;
	QMenu * _menuAudio;
	QMenu * _menuSettings;
	QMenu * _menuHelp;

	QToolBar * _mainToolBar;
	QToolBar * _playToolBar;
	QStatusBar * _statusBar;
};

#include <quarkplayer/PluginFactory.h>

/**
 * Creates MainWindow plugin.
 *
 * @author Tanguy Krotoff
 */
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
