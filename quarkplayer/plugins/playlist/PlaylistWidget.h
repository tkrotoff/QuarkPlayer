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

#include <QtGui/QWidget>

#include <QtCore/QList>

#include <phonon/phononnamespace.h>

namespace Ui { class PlaylistWidget; }

class MainWindow;

namespace Phonon {
	class MediaObject;
	class MediaSource;
}

class QToolBar;

/**
 * Playlist.
 *
 * @author Tanguy Krotoff
 */
class PlaylistWidget : public QWidget {
	Q_OBJECT
public:

	PlaylistWidget(MainWindow * mainWindow, Phonon::MediaObject * mediaObject);

	~PlaylistWidget();

private slots:

	void addFiles();

	void aboutToFinish();

	void metaStateChanged(Phonon::State newState, Phonon::State oldState);

	void tableDoubleClicked(int row, int column);

	void retranslate();

private:

	void populateActionCollection();

	void createPlaylistToolBar();

	void addFiles(const QStringList & files);

	QString convertMilliseconds(qint64 totalTime);

	void dragEnterEvent(QDragEnterEvent * event);

	void dropEvent(QDropEvent * event);

	Phonon::MediaObject * _mediaObject;

	Phonon::MediaObject * _metaObjectInfoResolver;

	MainWindow * _mainWindow;

	QList<Phonon::MediaSource> _mediaSources;

	Ui::PlaylistWidget * _ui;

	QToolBar * _playlistToolBar;
};

#endif	//PLAYLISTWIDGET_H
