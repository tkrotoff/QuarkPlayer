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

#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <quarkplayer/PluginInterface.h>

#include <QtGui/QWidget>

#include <QtCore/QList>

#include <phonon/phononnamespace.h>

class PlaylistModel;

class QuarkPlayer;

namespace Phonon {
	class MediaObject;
	class MediaSource;
}

class QToolBar;
class QTreeView;
class QDockWidget;

/**
 * Playlist.
 *
 * @author Tanguy Krotoff
 */
class PlaylistWidget : public QWidget, public PluginInterface {
	Q_OBJECT
public:

	PlaylistWidget(QuarkPlayer & quarkPlayer);

	~PlaylistWidget();

private slots:

	void addDir();

	void addFiles();

	void addURL();

	void openPlaylist();

	void savePlaylist();

	void clearSelection();

	void retranslate();

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

	/** Create a new playlist widget that will add itself to the dockwidget. */
	void createNewPlaylistWidget();

	void parserFilesFound(const QStringList & files);

private:

	void populateActionCollection();

	void createPlaylistToolBar();

	PlaylistModel * _playlistModel;

	QTreeView * _treeView;

	QToolBar * _playlistToolBar;

	QDockWidget * _dockWidget;
};

#include <quarkplayer/PluginFactory.h>

class PlaylistWidgetFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	PluginInterface * create(QuarkPlayer & quarkPlayer) const;
};

#endif	//PLAYLISTWIDGET_H
