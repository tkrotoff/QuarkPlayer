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
#include <quarkplayer/config/Config.h>
#include <quarkplayer/PluginManager.h>

#include <mediainfowindow/MediaInfoFetcher.h>

#include <filetypes/FileTypes.h>

#include <tkutil/FindFiles.h>
#include <tkutil/TkFile.h>
#include <tkutil/Random.h>

#include <playlistparser/PlaylistParser.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

const int PlaylistModel::COLUMN_TRACK = 0;
const int PlaylistModel::COLUMN_TITLE = 1;
const int PlaylistModel::COLUMN_ARTIST = 2;
const int PlaylistModel::COLUMN_ALBUM = 3;
const int PlaylistModel::COLUMN_LENGTH = 4;
const int PlaylistModel::COLUMN_FIRST = COLUMN_TRACK;
const int PlaylistModel::COLUMN_LAST = COLUMN_LENGTH;

static const int COLUMN_COUNT = 5;

static const QString CURRENT_PLAYLIST = "/current_playlist.m3u";

static const int POSITION_INVALID = -1;

PlaylistModel::PlaylistModel(QObject * parent, QuarkPlayer & quarkPlayer)
	: QAbstractItemModel(parent),
	_quarkPlayer(quarkPlayer) {

	_position = POSITION_INVALID;
	_mediaInfoFetcherLaunched = false;
	_rowWhereToInsertFiles = -1;

	//Info fetcher
	_mediaInfoFetcher = new MediaInfoFetcher(this);
	connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(updateMediaInfo()));

	//Optimization: loads the playlist only when all plugins have been loaded
	connect(&PluginManager::instance(), SIGNAL(allPluginsLoaded()),
		SLOT(loadCurrentPlaylist()), Qt::QueuedConnection);
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

		if (!track.mediaDataResolved()) {
			_filesInfoResolver = track.fileName();

			//Resolve meta data file one by one
			if (!_mediaInfoFetcherLaunched) {
				_mediaInfoFetcherLaunched = true;
				_mediaInfoFetcher->start(_filesInfoResolver);
			}
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
	qDebug() << __FUNCTION__ << "data:" << data << "action:" << action;
	qDebug() << __FUNCTION__ << "row:" << row << "column:" << column << "parent:" << parent;

	QStringList files;

	FindFiles findFiles;
	_rowWhereToInsertFiles = row;
	connect(&findFiles, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)));
	connect(&findFiles, SIGNAL(finished()), SLOT(saveCurrentPlaylist()));

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

				bool isMultimediaFile = FileTypes::extensions(FileType::Video, FileType::Audio).contains(fileInfo.suffix(), Qt::CaseInsensitive);
				if (isMultimediaFile) {
					files << filename;
				} else if (fileInfo.isDir()) {
					findFiles.setSearchPath(filename);
					findFiles.findAllFiles();
				}
			}
		}
	} else {
		return false;
	}

	addFiles(files, row);
	return true;
}

void PlaylistModel::filesFound(const QStringList & files) {
	addFiles(files, _rowWhereToInsertFiles);
}

void PlaylistModel::addFiles(const QStringList & files, int row) {
	QStringList filenameList;
	foreach (QString filename, files) {
		bool isMultimediaFile = FileTypes::extensions(FileType::Video, FileType::Audio).contains(TkFile::fileExtension(filename), Qt::CaseInsensitive);
		if (isMultimediaFile) {
			filenameList << filename;
		}
	}
	if (filenameList.isEmpty()) {
		return;
	}

	int first = 0;
	if (row == -1) {
		//row == -1 means append the files
		first = _mediaSources.size();
	} else {
		//row != -1 means we have a specific row location where to add the files
		first = row;
	}
	int last = first + filenameList.size() - 1;
	int currentRow = first;

	beginInsertRows(QModelIndex(), first, last);
	foreach (QString filename, filenameList) {
		_mediaSources.insert(currentRow, Track(filename));
		currentRow++;
	}
	endInsertRows();

	QCoreApplication::processEvents();
}

bool PlaylistModel::removeRows(int row, int count, const QModelIndex & parent) {
	qDebug() << __FUNCTION__ << "position:" << _position;
	if (row == _position) {
		_position = (-1) * count;
	}
	qDebug() << __FUNCTION__ << "position:" << _position;

	beginRemoveRows(QModelIndex(), row, row + count - 1);
	for (int i = 0; i < count; i++) {
		qDebug() << __FUNCTION__ << "Remove row:" << row;

		if (row < _position) {
			_position--;
		}

		//Updates the list of MediaSource
		_mediaSources.removeAt(row);
	}
	endRemoveRows();

	//Save current playlist each time we remove files from it
	saveCurrentPlaylist();

	return true;
}

void PlaylistModel::loadCurrentPlaylist() {
	//Restore last current playlist
	Config & config = Config::instance();
	QString path(config.configDir());
	PlaylistParser parser(path + CURRENT_PLAYLIST);
	connect(&parser, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)));
	parser.load();
}

void PlaylistModel::saveCurrentPlaylist() const {
	Config & config = Config::instance();
	QString path(config.configDir());
	PlaylistParser parser(path + CURRENT_PLAYLIST);
	parser.save(this->files());
}

QStringList PlaylistModel::files() const {
	QStringList files;
	foreach (Track track, _mediaSources) {
		files << track.fileName();
	}
	return files;
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

void PlaylistModel::updateMediaInfo() {
	Track track(_mediaInfoFetcher->filename());

	//Finds all the matching MediaSource
	for (int row = 0; ; row++) {
		row = _mediaSources.indexOf(track, row);
		if (row == -1) {
			break;
		}

		//Display track numbers like Winamp
		//track.setTrackNumber(QString::number(row));

		track.setTrackNumber(_mediaInfoFetcher->trackNumber());
		track.setTitle(_mediaInfoFetcher->title());
		track.setArtist(_mediaInfoFetcher->artist());
		track.setAlbum(_mediaInfoFetcher->album());
		track.setLength(_mediaInfoFetcher->length());
		track.setMediaDataResolved(true);

		_mediaSources[row] = track;

		//Update the row since the matching MediaSource has been modified
		emit dataChanged(index(row, COLUMN_FIRST), index(row, COLUMN_LAST));
	}

	_mediaInfoFetcherLaunched = false;
}

MediaInfoFetcher & PlaylistModel::mediaInfoFetcher() const {
	return *_mediaInfoFetcher;
}

Phonon::MediaSource PlaylistModel::mediaSource(const QModelIndex & index) const {
	if (index.isValid()) {
		int row = index.row();
		Track track(_mediaSources[row]);
		return track.mediaSource();
	}
	return Phonon::MediaSource();
}

void PlaylistModel::clear() {
	_mediaSources.clear();
	_filesInfoResolver.clear();
	_position = POSITION_INVALID;
	reset();

	//Save current playlist each time we remove files from it
	saveCurrentPlaylist();
}

void PlaylistModel::play() {
	if (_position != POSITION_INVALID) {
		_quarkPlayer.play(_mediaSources[_position].mediaSource());
	} else {
		qCritical() << __FUNCTION__ << "Error: the position is invalid";
	}
}

void PlaylistModel::enqueue() {
	if (_position != POSITION_INVALID) {
		_quarkPlayer.currentMediaObject()->enqueue(_mediaSources[_position].mediaSource());
	} else {
		qCritical() << __FUNCTION__ << "Error: the position is invalid";
	}
}

void PlaylistModel::setPosition(int position) {
	_position = position;
}

int PlaylistModel::position() const {
	return _position;
}
