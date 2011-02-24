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

#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <quarkplayer-plugins/Playlist/PlaylistExport.h>

#include <quarkplayer/PluginInterface.h>

#include <PlaylistParser/PlaylistParser.h>

#include <TkUtil/Actions.h>

#include <QtGui/QWidget>

#include <QtCore/QList>

#include <phonon/phononnamespace.h>

class PlaylistModel;
class PlaylistFilter;
class DragAndDropTreeView;
class SearchLineEdit;
class QuarkPlayer;
class IMainWindow;

namespace Phonon {
	class MediaObject;
	class MediaSource;
}

class QDockWidget;

/**
 * Playlist widget.
 *
 * A lot of code (search toolbar) is a copy-paste from FileBrowserWidget.
 *
 * Seems like Playlist is a better name than PlayList:
 * Playlist (Java): 10,000 answers
 * http://www.google.com/codesearch?as_q=Playlist&btnG=Search+Code&hl=en&as_lang=java&as_case=y
 * PlayList (Java): 6,000 answers
 * http://www.google.com/codesearch?as_q=PlayList&btnG=Search+Code&hl=en&as_lang=java&as_case=y
 *
 * @see FileBrowserWidget
 * @author Tanguy Krotoff
 */
class PLAYLIST_API PlaylistWidget : public QWidget, public PluginInterface {
	Q_OBJECT
public:

	PlaylistWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid, IMainWindow * mainWindow);

	~PlaylistWidget();

	PlaylistModel * playlistModel() const;

	PlaylistFilter * playlistFilter() const;

public slots:

	/**
	 * Adds a list of files to the current playlist.
	 *
	 * @param files files to add to the current playlist
	 */
	void addFilesToCurrentPlaylist(const QStringList & files);

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

	void playlistLoaded(PlaylistParser::Error, int timeElapsed);

	void playlistSaved(PlaylistParser::Error, int timeElapsed);

	/** Jumps to current playing media. */
	void jumpToCurrent();

	void search();

	void addWordToWordList();

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

	DragAndDropTreeView * _view;

	SearchLineEdit * _searchLineEdit;

	QDockWidget * _dockWidget;

	/** Needed by addWordToWordList(). */
	QTimer * _searchTimer;

	IMainWindow * _mainWindow;

	ActionCollection _actions;
};

#include <quarkplayer/PluginFactory.h>

/**
 * Creates PlaylistWidget plugin.
 *
 * @author Tanguy Krotoff
 */
class PLAYLIST_API PlaylistWidgetFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	static const char * PLUGIN_NAME;

	QString name() const { return PLUGIN_NAME; }
	QStringList dependencies() const;
	QString description() const { return tr("Playlist view"); }
	QString version() const { return "0.0.1"; }
	QString url() const { return "http://quarkplayer.googlecode.com/"; }
	QString vendor() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;

	static PlaylistWidget * playlistWidget();
};

#endif	//PLAYLISTWIDGET_H
