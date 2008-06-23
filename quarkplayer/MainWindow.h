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
class PlayToolBar;

class QDockWidget;
class QTabWidget;

namespace Phonon {
	class MediaSource;
}

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

	QMenu * menuAudioChannels() const;

	QMenu * menuSubtitles() const;

	QMenu * menuTitles() const;

	QMenu * menuChapters() const;

	QMenu * menuAngles() const;

	QTabWidget * browserTabWidget() const;

	QTabWidget * playlistTabWidget() const;

	QTabWidget * videoTabWidget() const;

signals:

	void subtitleFileDropped(const QString & subtitle);

	void playToolBarAdded(QToolBar * playToolBar);

public slots:

	void play(const Phonon::MediaSource & mediaSource);

private slots:

	void about();

	void showConfigWindow();

	void playFile();
	void playDVD();
	void playURL();

	void addRecentFilesToMenu();
	void playRecentFile(int id);
	void clearRecentFiles();

	void metaDataChanged();

	void retranslate();

private:

	void populateActionCollection();

	void setupUi();

	void createDockWidgets();

	void dragEnterEvent(QDragEnterEvent * event);

	void dropEvent(QDropEvent * event);

	TkToolBar * _mainToolBar;
	QMenu * _menuRecentFiles;
	QMenu * _menuAudioChannels;
	QMenu * _menuSubtitles;
	QMenu * _menuTitles;
	QMenu * _menuChapters;
	QMenu * _menuAngles;
	QMenu * _menuFile;
	QMenu * _menuPlay;
	QMenu * _menuAudio;
	QMenu * _menuSubtitle;
	QMenu * _menuBrowse;
	QMenu * _menuSettings;
	QMenu * _menuHelp;

	QToolBar * _playToolBar;

	QTabWidget * _videoTabWidget;

	QTabWidget * _browserTabWidget;

	QTabWidget * _playlistTabWidget;
};

#endif	//MAINWINDOW_H
