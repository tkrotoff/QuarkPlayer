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

#include <tkutil/TkMainWindow.h>
#include <tkutil/TkToolBar.h>

#include <phonon/phononnamespace.h>
#include <phonon/path.h>

#include <QtGui/QMainWindow>

class VideoWidget;
class PlayToolBar;

namespace Phonon {
	class MediaObject;
	class MediaController;
	class AudioOutput;
	class MediaSource;
}

class QStackedWidget;

/**
 * Main interface, the main window: QMainWindow.
 *
 * @author Tanguy Krotoff
 */
class MainWindow : public TkMainWindow {
	Q_OBJECT
public:

	MainWindow(QWidget * parent);

	~MainWindow();

	PlayToolBar * playToolBar() const;

	VideoWidget * videoWidget() const;

	QStackedWidget * stackedWidget() const;

	QMenu * menuAudioChannels() const;

	QMenu * menuSubtitles() const;

	QMenu * menuTitles() const;

	QMenu * menuChapters() const;

	QMenu * menuAngles() const;

private slots:

	void about();

	void showConfigWindow();
	void showQuickSettingsWindow();

	void playFile();
	void playDVD();
	void playURL();

	void addRecentFilesToMenu();
	void playRecentFile(int id);
	void clearRecentFiles();

	void play(const Phonon::MediaSource & mediaSource);

	void aboutToFinish();

	void sourceChanged(const Phonon::MediaSource & source);
	void metaDataChanged();
	void stateChanged(Phonon::State newState, Phonon::State oldState);
	void hasVideoChanged(bool hasVideo);

	void retranslate();

private:

	void populateActionCollection();

	void setupUi();

	void closeEvent(QCloseEvent * event);

	Phonon::MediaObject * _mediaObject;

	/** Widget containing the video. */
	VideoWidget * _videoWidget;

	/** Widget containing the logo. */
	QWidget * _backgroundLogoWidget;

	/** Widget containing the media data. */
	QWidget * _mediaDataWidget;

	QStackedWidget * _stackedWidget;

	TkToolBar * _mainToolBar;
	QMenu * _menuRecentFiles;
	QMenu * _menuAudioChannels;
	QMenu * _menuSubtitles;
	QMenu * _menuTitles;
	QMenu * _menuChapters;
	QMenu * _menuAngles;
	QMenu * _menuFile;
	QMenu * _menuAudio;
	QMenu * _menuSubtitle;
	QMenu * _menuBrowse;
	QMenu * _menuSettings;
	QMenu * _menuHelp;

	Phonon::AudioOutput * _audioOutput;
	Phonon::Path _audioOutputPath;
	Phonon::MediaSource * _mediaSource;

	PlayToolBar * _playToolBar;
};

#endif	//MAINWINDOW_H
