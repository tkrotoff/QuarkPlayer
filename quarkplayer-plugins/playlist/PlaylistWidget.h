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

#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <quarkplayer/PluginInterface.h>

#include <QtGui/QWidget>

#include <QtCore/QList>

#include <phonon/phononnamespace.h>

class PlaylistModel;
class PlaylistFilter;
class DragAndDropTreeView;
class SearchLineEdit;

class QuarkPlayer;

namespace Phonon {
	class MediaObject;
	class MediaSource;
}

class QDockWidget;

/**
 * Playlist.
 *
 * A lot of code (search toolbar) is a copy-paste from FileBrowserWidget.
 *
 * @see FileBrowserWidget
 * @author Tanguy Krotoff
 */
class PlaylistWidget : public QWidget, public PluginInterface {
	Q_OBJECT
public:

	PlaylistWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~PlaylistWidget();

	QString name() const { return tr("Playlist"); }
	QString description() const { return tr("Playlist view"); }
	QString version() const { return "0.0.1"; }
	QString webpage() const { return "http://quarkplayer.googlecode.com/"; }
	QString email() const { return "quarkplayer@googlegroups.com"; }
	QString authors() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

private slots:

	void addDir();

	void addFiles();

	void addURL();

	void openPlaylist();

	void savePlaylist();

	void retranslate();

	void updateWindowTitle(const QString & statusMessage = QString());

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

	/**
	 * Create a new playlist widget that will add itself to the dockwidget.
	 *
	 * Loads a new playlist plugin
	 */
	void createNewPlaylistWidget();

	void parserFilesFound(const QStringList & files);

	void playlistLoaded(int timeElapsed);

	void playlistSaved(int timeElapsed);

	/** Jumps to current playing media. */
	void jumpToCurrent();

	void search();

	void addWordToWordList();

	void currentSourceChanged(const Phonon::MediaSource & source);

	void stateChanged(Phonon::State newState);

	void dockWidgetVisibilityChanged(bool visible);

	void activePlaylistChanged(const QUuid & uuid);

private:

	void populateActionCollection();

	void createToolBar();

	void connectToMediaObject(Phonon::MediaObject * mediaObject);

	void disconnectFromMediaObjectList();

	bool event(QEvent * event);

	PlaylistModel * _playlistModel;

	PlaylistFilter * _playlistFilter;

	DragAndDropTreeView * _treeView;

	SearchLineEdit * _searchLineEdit;

	QDockWidget * _dockWidget;

	/** Needed by addWordToWordList(). */
	QTimer * _searchTimer;
};

#include <quarkplayer/PluginFactory.h>

class PlaylistWidgetFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;
};

#endif	//PLAYLISTWIDGET_H
