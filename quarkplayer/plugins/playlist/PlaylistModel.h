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

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <phonon/phononnamespace.h>
#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QtCore/QString>

class QuarkPlayer;

/**
 * Playlist model.
 *
 * @author Tanguy Krotoff
 */
class PlaylistModel : public QAbstractItemModel {
	Q_OBJECT
public:

	PlaylistModel(QObject * parent, QuarkPlayer & quarkPlayer);

	~PlaylistModel();

	void highlightItem(int row);

	void addFiles(const QStringList & files, int row = -1);

	//Inherited from QAbstractItemModel
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex & index) const;

	bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
	QMimeData * mimeData(const QModelIndexList & indexes) const;
	QStringList mimeTypes() const;
	Qt::DropActions supportedDropActions() const;

public slots:

	void clear();

	void play(const QModelIndex & index);

	void playNextTrack();

	void playPreviousTrack();

private slots:

	void metaStateChanged(Phonon::State newState, Phonon::State oldState);

	void stateChanged(Phonon::State newState, Phonon::State oldState);

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

private:

	void updateRow(int row);

	QuarkPlayer & _quarkPlayer;

	Phonon::MediaObject * _metaObjectInfoResolver;

	class Track {
	public:

		Track(const Phonon::MediaSource & mediaSource);
		Track(const Track & track);

		~Track();

		Track & operator=(const Track & right);
		int operator==(const Track & right);

		QString fileName() const;
		Phonon::MediaSource mediaSource() const;
		void setTrackNumber(const QString & trackNumber);
		QString trackNumber() const;
		void setTitle(const QString & title);
		QString title() const;
		void setArtist(const QString & artist);
		QString artist() const;
		void setAlbum(const QString & album);
		QString album() const;
		void setLength(const QString & length);
		QString length() const;

	private:

		void copy(const Track & track);

		QString convertMilliseconds(qint64 totalTime) const;

		Phonon::MediaSource _source;

		QString _filename;

		QString _trackNumber;
		QString _title;
		QString _artist;
		QString _album;
		QString _length;
	};

	QList<Track> _mediaSources;

	int _position;
};

#endif	//PLAYLISTMODEL_H
