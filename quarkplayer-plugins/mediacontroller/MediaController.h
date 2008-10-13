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

#ifndef MEDIACONTROLLER_H
#define MEDIACONTROLLER_H

#include <quarkplayer/PluginInterface.h>

#include <QtGui/QWidget>

class MediaControllerToolBar;

class QuarkPlayer;
class MainWindow;

namespace Phonon {
	class MediaController;
	class MediaObject;
}

class QMenu;

/**
 * Handles Phonon::MediaController.
 *
 * Handles subtitles, audio channels, angles...
 *
 * Inherits from QWidget instead of QObject, otherwise retranslate() does not work.
 *
 * @see Phonon::MediaController
 * @author Tanguy Krotoff
 */
class MediaController : public QWidget, public PluginInterface {
	Q_OBJECT
public:

	MediaController(QuarkPlayer & quarkPlayer);

	~MediaController();

	QString name() const { return tr("Media controller"); }
	QString description() const { return tr("DVD chapters and titles handling"); }
	QString version() const { return "0.0.1"; }
	QString webpage() const { return "http://quarkplayer.googlecode.com/"; }
	QString email() const { return "quarkplayer@googlegroups.com"; }
	QString authors() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

private slots:

	void openSubtitleFile();
	void openSubtitleFile(const QString & subtitleFile);

	void availableAudioChannelsChanged();
	void actionAudioChannelTriggered(int id);

	void availableSubtitlesChanged();
	void actionSubtitleTriggered(int id);

	void availableTitlesChanged();
	void actionTitleTriggered(int id);

	void availableChaptersChanged();
	void actionChapterTriggered(int id);

	void availableAnglesChanged();
	void actionAngleTriggered(int id);

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

	void retranslate();

private:

	void populateActionCollection();

	void addMenusToMainWindow();

	/** Code factorization. */
	static void removeAllAction(QObject * object);

	MainWindow * _mainWindow;

	MediaControllerToolBar * _toolBar;

	Phonon::MediaController * _mediaController;

	QMenu * _menuAudioChannels;
	QMenu * _menuSubtitles;
	QMenu * _menuTitles;
	QMenu * _menuChapters;
	QMenu * _menuAngles;
	QMenu * _menuAudio;
	QMenu * _menuSubtitle;
	QMenu * _menuBrowse;
};

#include <quarkplayer/PluginFactory.h>

class MediaControllerFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	PluginInterface * create(QuarkPlayer & quarkPlayer) const;
};

#endif	//MEDIACONTROLLER_H
