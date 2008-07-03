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

#include "PlaylistModel.h"

#include "Track.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/FileExtensions.h>

#include <tkutil/FindFiles.h>
#include <tkutil/Random.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

static const int COLUMN_TRACK = 0;
static const int COLUMN_TITLE = 1;
static const int COLUMN_ARTIST = 2;
static const int COLUMN_ALBUM = 3;
static const int COLUMN_LENGTH = 4;

static const int COLUMN_COUNT = 5;

PlaylistModel::PlaylistModel(QObject * parent, QuarkPlayer & quarkPlayer)
	: QAbstractItemModel(parent),
	_quarkPlayer(quarkPlayer) {

	_shuffle = false;
	_repeat = false;
	clear();

	//Info resolver
	_metaObjectInfoResolver = new Phonon::MediaObject(this);
	connect(_metaObjectInfoResolver, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(metaStateChanged(Phonon::State, Phonon::State)));

	connect(&_quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));
}

PlaylistModel::~PlaylistModel() {
}

int PlaylistModel::columnCount(const QModelIndex & parent) const {
	return COLUMN_COUNT;
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const {
	QVariant tmp;

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case COLUMN_TRACK:
			tmp = tr("#");
			break;
		case COLUMN_TITLE:
			tmp = tr("Title");
			break;
		case COLUMN_ARTIST:
			tmp = tr("Artist");
			break;
		case COLUMN_ALBUM:
			tmp = tr("Album");
			break;
		case COLUMN_LENGTH:
			tmp = tr("Length");
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unknown column:" << section;
		}
	}

	return tmp;
}

QVariant PlaylistModel::data(const QModelIndex & index, int role) const {
	QVariant tmp;

	if (!index.isValid()) {
		return tmp;
	}

	int row = index.row();
	int column = index.column();
	Track track(_mediaSources[row]);
	if (role == Qt::DisplayRole) {
		switch (column) {
		case COLUMN_TRACK:
			tmp = track.trackNumber();
			break;
		case COLUMN_TITLE:
			tmp = track.title();
			break;
		case COLUMN_ARTIST:
			tmp = track.artist();
			break;
		case COLUMN_ALBUM:
			tmp = track.album();
			break;
		case COLUMN_LENGTH:
			tmp = track.length();
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unknown column:" << column;
		}
	}

	if (row == _position) {
		//Change the font and color of the current played media

		if (role == Qt::FontRole) {
			QFont font;
			font.setBold(true);
			tmp = font;
		}

		if (role == Qt::BackgroundRole) {
			tmp = QApplication::palette().alternateBase();
		}
	}

	return tmp;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex & parent) const {
	if (parent.isValid()) {
		return QModelIndex();
	}

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	return createIndex(row, column);
}

QModelIndex PlaylistModel::parent(const QModelIndex & index) const {
	return QModelIndex();
}

int PlaylistModel::rowCount(const QModelIndex & parent) const {
	if (parent.isValid()) {
		return 0;
	}

	return _mediaSources.size();
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex & index) const {
	if (!index.isValid()) {
		return Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

bool PlaylistModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) {
	QStringList files;

	//Add urls to a list
	if (data->hasUrls()) {
		QList<QUrl> urlList = data->urls();
		QString filename;
		foreach (QUrl url, urlList) {
			if (url.isValid()) {
				qDebug() << __FUNCTION__ << "File scheme:" << url.scheme();
				if (url.scheme() == "file") {
					filename = url.toLocalFile();
				} else {
					filename = url.toString();
				}

				QFileInfo fileInfo(filename);

				bool isMultimediaFile = FileExtensions::multimedia().contains(fileInfo.suffix(), Qt::CaseInsensitive);
				if (isMultimediaFile) {
					files << filename;
				} else if (fileInfo.isDir()) {
					QStringList tmp(FindFiles::findAllFiles(filename));
					foreach (QString filename2, tmp) {
						QFileInfo fileInfo2(filename2);
						bool isMultimediaFile2 = FileExtensions::multimedia().contains(fileInfo2.suffix(), Qt::CaseInsensitive);
						if (isMultimediaFile2) {
							files << filename2;
						}
					}
				}
			}
		}
	} else {
		return false;
	}

	addFiles(files, row);
	return true;
}

void PlaylistModel::addFiles(const QStringList & files, int row) {
	_filesInfoResolver << files;

	int first = row;
	if (row == -1) {
		first = _mediaSources.size();
	}
	int last = first + _filesInfoResolver.size() - 1;
	int position = first;

	beginInsertRows(QModelIndex(), first, last);
	foreach (QString file, _filesInfoResolver) {
		_mediaSources.insert(position++, Track(file));
	}
	endInsertRows();

	//Resolve first meta data file
	//The other ones an queued
	if (!_filesInfoResolver.isEmpty()) {
		_metaObjectInfoResolver->setCurrentSource(_filesInfoResolver.first());
	}
}

QMimeData * PlaylistModel::mimeData(const QModelIndexList & indexes) const {
	QMimeData * mimeData = new QMimeData();
	QStringList files;
	foreach (QModelIndex index, indexes) {
		//Number of index is the number of columns
		//Here we have 5 columns, we only want to select 1 column per row
		if (index.column() == COLUMN_TITLE) {
			QString filename(_mediaSources[index.row()].fileName());
			if (!filename.isEmpty()) {
				files << filename;
			}
		}
	}

	mimeData->setData("text/uri-list", files.join("\n").toUtf8());

	return mimeData;
}

QStringList PlaylistModel::mimeTypes() const {
	QStringList types;
	types << "text/uri-list";
	return types;
}

Qt::DropActions PlaylistModel::supportedDropActions() const {
	return Qt::CopyAction | Qt::MoveAction;
}

void PlaylistModel::metaStateChanged(Phonon::State newState, Phonon::State oldState) {
	Phonon::MediaSource source = _metaObjectInfoResolver->currentSource();
	QMap<QString, QString> metaData = _metaObjectInfoResolver->metaData();

	if (newState == Phonon::ErrorState) {
		return;
	}

	if (newState != Phonon::StoppedState && newState != Phonon::PausedState) {
		return;
	}

	if (source.type() == Phonon::MediaSource::Invalid) {
		return;
	}

	//Finds the matching MediaSource
	int row = 0;
	foreach (Track track, _mediaSources) {
		if (track == Track(source)) {
			//We found the right MediaSource
			track.setTrackNumber(metaData.value("TRACKNUMBER"));
			track.setTitle(metaData.value("TITLE"));
			track.setArtist(metaData.value("ARTIST"));
			track.setAlbum(metaData.value("ALBUM"));
			track.setLength(metaData.value("LENGTH"));
			_mediaSources[row] = track;
			updateRow(row);
		}
		row++;
	}

	_filesInfoResolver.takeFirst();
	if (!_filesInfoResolver.isEmpty()) {
		_metaObjectInfoResolver->setCurrentSource(_filesInfoResolver.first());
	}
}

void PlaylistModel::clear() {
	_mediaSources.clear();
	_position = -1;
	reset();
}

void PlaylistModel::highlightItem(int row) {
	static int previousPosition = 0;

	if (row >= 0 && _mediaSources.size() > row) {
		_position = row;
		updateRow(_position);
		updateRow(previousPosition);
		previousPosition = _position;
	}
}

void PlaylistModel::updateRow(int row) {
	qDebug() << __FUNCTION__ << row;
	emit dataChanged(index(row, 0), index(row, COLUMN_COUNT));
}

void PlaylistModel::playNextTrack() {
	if (_shuffle) {
		_position = Random::randomInt(0, _mediaSources.size());
	} else {
		_position++;
	}

	if (_position < 0 || _position >= _mediaSources.size()) {
		//Back to the top of the playlist
		_position = 0;
	}

	_quarkPlayer.play(_mediaSources[_position].mediaSource());
}

void PlaylistModel::playPreviousTrack() {
	if (_shuffle) {
		_position = Random::randomInt(0, _mediaSources.size());
	} else {
		_position--;
	}

	if (_position < 0 || _position >= _mediaSources.size()) {
		//Back to the bottom of the playlist
		_position = _mediaSources.size() - 1;
	}

	_quarkPlayer.play(_mediaSources[_position].mediaSource());
}

void PlaylistModel::play(const QModelIndex & index) {
	if (!index.isValid()) {
		return;
	}

	_position = index.row();
	_quarkPlayer.play(_mediaSources[_position].mediaSource());
}

void PlaylistModel::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, _quarkPlayer.mediaObjectList()) {
		tmp->disconnect(this);
	}

	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State, Phonon::State)));

	//aboutToFinish -> let's play the next track
	connect(mediaObject, SIGNAL(aboutToFinish()), SLOT(enqueueNextTrack()));
}

void PlaylistModel::stateChanged(Phonon::State newState, Phonon::State oldState) {
	if (newState == Phonon::PlayingState) {
		Track track(_quarkPlayer.currentMediaObject()->currentSource());
		if (_position >= 0 && _mediaSources[_position] == track) {
			//No need to change _position: we have already the right _position
		} else {
			_position = _mediaSources.indexOf(track);
		}
		highlightItem(_position);
	}
}

void PlaylistModel::enqueueNextTrack() {
	if (_shuffle) {
		_position = Random::randomInt(0, _mediaSources.size());
	} else {
		_position++;
	}

	if (_position < 0 || _position >= _mediaSources.size()) {
		if (_repeat) {
			//Back to the top of the playlist
			_position = 0;
		}
	}

	if (_position >= 0 && _position < _mediaSources.size()) {
		_quarkPlayer.currentMediaObject()->enqueue(_mediaSources[_position].mediaSource());

		//enqueue does not send a stateChanged event :/
		//let's highlight ourself the new current item
		highlightItem(_position);
	}
}

void PlaylistModel::setShuffle(bool shuffle) {
	_shuffle = shuffle;
}

void PlaylistModel::setRepeat(bool repeat) {
	_repeat = repeat;
}
