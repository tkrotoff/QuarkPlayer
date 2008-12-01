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
#include <quarkplayer/config/PlaylistConfig.h>
#include <quarkplayer/PluginsManager.h>

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

const int PlaylistModel::COLUMN_TRACK = 0;
const int PlaylistModel::COLUMN_TITLE = 1;
const int PlaylistModel::COLUMN_ARTIST = 2;
const int PlaylistModel::COLUMN_ALBUM = 3;
const int PlaylistModel::COLUMN_LENGTH = 4;
const int PlaylistModel::COLUMN_FIRST = COLUMN_TRACK;
const int PlaylistModel::COLUMN_LAST = COLUMN_LENGTH;

static const int COLUMN_COUNT = PlaylistModel::COLUMN_LAST + 1;

static const int POSITION_INVALID = -1;
const int PlaylistModel::APPEND_FILES = -1;

static const char * PLAYLIST_TRACK_DISPLAY_MODE_KEY = "playlist_track_display_mode";

PlaylistModel::PlaylistModel(QObject * parent, QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QAbstractItemModel(parent),
	_quarkPlayer(quarkPlayer) {

	_uuid = uuid;

	clearInternal();

	connect(&quarkPlayer, SIGNAL(addFilesToCurrentPlaylist(const QStringList &)),
		SLOT(addFilesToCurrentPlaylist(const QStringList &)));

	//Info fetcher
	_mediaInfoFetcher = new MediaInfoFetcher(this);
	connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(updateMediaInfo()));

	if (PluginsManager::instance().allPluginsAlreadyLoaded()) {
		//If all the plugins are already loaded...
		loadCurrentPlaylist();
	} else {
		//Optimization: loads the playlist only when all plugins have been loaded
		connect(&PluginsManager::instance(), SIGNAL(allPluginsLoaded()),
			SLOT(loadCurrentPlaylist()), Qt::QueuedConnection);
	}

	Config::instance().addKey(PLAYLIST_TRACK_DISPLAY_MODE_KEY, TrackDisplayModeNormal);
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

	if (!index.isValid() || index.model() != this) {
		return tmp;
	}

	int row = index.row();
	int column = index.column();
	if (role == Qt::DisplayRole) {
		Track track = _filenames[row];
		QString filename(track.fileName());
		if (track.mediaDataResolved()) {
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

		else {
			switch (column) {
			case COLUMN_TITLE:
				if (!QUrl(filename).host().isEmpty()) {
					//A filename that contains a host/server name is a remote/network media
					//So let's keep the complete filename
					tmp = filename;
				} else {
					tmp = TkFile::dir(filename) + "/" +
						TkFile::removeFileExtension(TkFile::fileName(filename));

					//Resolve meta data file one by one
					//Cannot do that for remote/network media, i.e URLs
					if (_mediaInfoFetcherRow == POSITION_INVALID) {
						_mediaInfoFetcherRow = row;
						_mediaInfoFetcher->start(filename);
					}
				}
				break;
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

	return _filenames.size();
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
				files << filename;
			}
		}
	} else {
		return false;
	}

	addFiles(files, row);
	saveCurrentPlaylist();

	return true;
}

void PlaylistModel::filesFound(const QStringList & files) {
	addFiles(files, _rowWhereToInsertFiles);
}

void PlaylistModel::addFiles(const QStringList & files, int row) {
	_rowWhereToInsertFiles = row;
	QStringList filenameList;
	foreach (QString filename, files) {
		bool isMultimediaFile = FileTypes::extensions(FileType::Video, FileType::Audio).contains(
					TkFile::fileExtension(filename), Qt::CaseInsensitive);
		if (isMultimediaFile) {
			filenameList << filename;
		} else if (TkFile::isDir(filename)) {
			_nbFindFiles++;
			FindFiles * findFiles = new FindFiles(this);
			connect(findFiles, SIGNAL(filesFound(const QStringList &)),
				SLOT(filesFound(const QStringList &)));
			connect(findFiles, SIGNAL(finished(int)),
				SLOT(searchfinished(int)));
			findFiles->setSearchPath(filename);
			findFiles->setFilesFoundLimit(500);
			findFiles->setFindDirs(false);
			findFiles->start();
		} else if (!QUrl(filename).host().isEmpty()) {
			//A filename that contains a host/server name is a remote/network media
			//So it should be added to the playlist
			filenameList << filename;
		}
	}

	if (!filenameList.isEmpty()) {
		int first = 0;
		if (row == APPEND_FILES) {
			//row == APPEND_FILES means append the files
			first = _filenames.size();
		} else {
			//row != APPEND_FILES means we have a specific row location where to add the files
			first = row;
		}
		int last = first + filenameList.size() - 1;
		int currentRow = first;

		beginInsertRows(QModelIndex(), first, last);
		foreach (QString filename, filenameList) {
			_filenames.insert(currentRow, Track(filename));
			currentRow++;
		}
		endInsertRows();

		//Change current playing position
		bool positionAlreadyChanged = false;
		foreach(int dragAndDropRow, _dragAndDropRows) {
			if (_position == dragAndDropRow) {
				//This means the current playing media is getting drag and droped
				int firstRow = _dragAndDropRows.first();
				int offset = dragAndDropRow - firstRow;
				int newPosition = first + offset;
				setPosition(newPosition);
				positionAlreadyChanged = true;
				break;
			}
		}
		if (!positionAlreadyChanged) {
			int count = filenameList.size();
			if (_position != POSITION_INVALID && row != APPEND_FILES) {
				if (_position >= row) {
					_position += count;
				}
			}
		}
	}
}

void PlaylistModel::addFilesToCurrentPlaylist(const QStringList & files) {
	if (_uuid == PlaylistConfig::instance().activePlaylist()) {
		addFiles(files);
		saveCurrentPlaylist();
	}
}

void PlaylistModel::searchfinished(int timeElapsed) {
	_nbFindFiles--;
	if (_nbFindFiles <= 0) {
		saveCurrentPlaylist();
	}
}

bool PlaylistModel::removeRows(int row, int count, const QModelIndex & parent) {
	bool success = false;
	beginRemoveRows(parent, row, row + count - 1);
	for (int i = 0; i < count; i++) {
		//Remove from the list of filenames
		_filenames.removeAt(row);
		success = true;
	}
	endRemoveRows();

	//Save current playlist each time we remove files from it
	saveCurrentPlaylist();

	//Change current playing position
	if (_position >= row) {
		if (_position <= row + count - 1) {
			setPosition(POSITION_INVALID);
		} else {
			_position -= count;
		}
	}

	return success;
}

QString PlaylistModel::currentPlaylist() const {
	return "/playlist_" + _uuid.toString() + ".m3u";
}

void PlaylistModel::loadCurrentPlaylist() {
	//Restore last current playlist
	QString path(Config::instance().configDir());
	PlaylistParser * parser = new PlaylistParser(path + currentPlaylist(), this);
	connect(parser, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)));
	connect(parser, SIGNAL(finished(int)),
		SIGNAL(playlistLoaded(int)));
	parser->load();
}

void PlaylistModel::saveCurrentPlaylist() {
	//Optimization:
	//saves the playlist only if no playlist change for at least 2 seconds (2000 ms)
	static QTimer * saveCurrentPlaylistTimer = NULL;
	if (!saveCurrentPlaylistTimer) {
		//Lazy initialization
		saveCurrentPlaylistTimer = new QTimer(this);
		saveCurrentPlaylistTimer->setSingleShot(true);
		saveCurrentPlaylistTimer->setInterval(2000);
		connect(saveCurrentPlaylistTimer, SIGNAL(timeout()), SLOT(saveCurrentPlaylist()));
	}

	static bool timerAlreadyStarted = false;
	if (!timerAlreadyStarted) {
		timerAlreadyStarted = true;
		saveCurrentPlaylistTimer->stop();
		saveCurrentPlaylistTimer->start();
	}

	else if (!saveCurrentPlaylistTimer->isActive()) {
		timerAlreadyStarted = false;

		//Optimization:
		//saves the playlist only if the playlist has been changed
		static QStringList lastPlaylistSaved("this string should be unique");
		QStringList newPlaylist = fileNames();
		if (newPlaylist != lastPlaylistSaved) {
			lastPlaylistSaved = newPlaylist;
			QString path(Config::instance().configDir());
			PlaylistParser * parser = new PlaylistParser(path + currentPlaylist(), this);
			connect(parser, SIGNAL(finished(int)),
				SIGNAL(playlistSaved(int)));
			parser->save(newPlaylist);
		}
	}
}

QStringList PlaylistModel::fileNames() const {
	QStringList files;
	foreach (Track track, _filenames) {
		files << track.fileName();
	}
	return files;
}

QMimeData * PlaylistModel::mimeData(const QModelIndexList & indexes) const {
	_dragAndDropRows.clear();
	QMimeData * mimeData = new QMimeData();
	QStringList files;
	foreach (QModelIndex index, indexes) {
		//Number of index is the number of columns
		//Here we have 5 columns, we only want to select 1 column per row
		if (index.column() == COLUMN_TITLE) {
			int dragAndDropRow = index.row();
			QString filename(_filenames[dragAndDropRow].fileName());
			if (!filename.isEmpty()) {
				files << filename;
				_dragAndDropRows += dragAndDropRow;
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
	if (_mediaInfoFetcherRow == POSITION_INVALID) {
		qCritical() << __FUNCTION__ << "Error: _mediaInfoFetcherRow invalid";
	} else {
		if (_mediaInfoFetcherRow < _filenames.size()) {
			Track track = _filenames[_mediaInfoFetcherRow];

			if (track.fileName() == _mediaInfoFetcher->fileName() &&
				!track.mediaDataResolved()) {

				TrackDisplayMode trackDisplayMode = static_cast<TrackDisplayMode>(Config::instance().value(PLAYLIST_TRACK_DISPLAY_MODE_KEY).toInt());
				switch (trackDisplayMode) {
				case TrackDisplayModeNormal:
					track.setTrackNumber(_mediaInfoFetcher->trackNumber());
					break;
				case TrackDisplayModeWinamp:
					//Display track numbers like Winamp
					track.setTrackNumber(QString::number(_mediaInfoFetcherRow));
					break;
				default:
					qCritical() << __FUNCTION__ << "Error: unknown TrackDisplayMode:" << trackDisplayMode;
				}

				track.setTitle(_mediaInfoFetcher->title());
				track.setArtist(_mediaInfoFetcher->artist());
				track.setAlbum(_mediaInfoFetcher->album());
				track.setLength(_mediaInfoFetcher->length());
				track.setMediaDataResolved(true);

				_filenames[_mediaInfoFetcherRow] = track;

				//Update the row since the matching MediaSource has been modified
				emit dataChanged(index(_mediaInfoFetcherRow, COLUMN_FIRST), index(_mediaInfoFetcherRow, COLUMN_LAST));
			}
		}
	}
	_mediaInfoFetcherRow = POSITION_INVALID;
}

QString PlaylistModel::fileName(const QModelIndex & index) const {
	QString tmp;
	if (index.isValid()) {
		int row = index.row();
		tmp = _filenames[row].fileName();
	}
	return tmp;
}

void PlaylistModel::clearInternal() {
	_filenames.clear();
	_position = POSITION_INVALID;
	_mediaInfoFetcherRow = POSITION_INVALID;
	_rowWhereToInsertFiles = APPEND_FILES;
	_nbFindFiles = 0;
}

void PlaylistModel::clear() {
	clearInternal();
	reset();

	//Save current playlist
	saveCurrentPlaylist();
}

void PlaylistModel::play(int position) {
	setPosition(position);
	if (_position != POSITION_INVALID) {
		QString fileName(_filenames[position].fileName());
		qDebug() << __FUNCTION__ << "Play file:" << fileName;
		_quarkPlayer.play(fileName);
	} else {
		qCritical() << __FUNCTION__ << "Error: invalid position";
	}
}

void PlaylistModel::enqueue(int position) {
	if (position != POSITION_INVALID) {
		//Important to clear the queue: otherwise we can get some strange behaviors
		//One never knows what is inside the queue of the backend,
		//better to erase it and be sure
		_quarkPlayer.currentMediaObject()->clearQueue();

		QString fileName(_filenames[position].fileName());
		qDebug() << __FUNCTION__ << "Enqueue file:" << fileName;
		_quarkPlayer.currentMediaObject()->enqueue(fileName);
	} else {
		qCritical() << __FUNCTION__ << "Error: invalid position";
	}
}

void PlaylistModel::setPosition(int position) {
	static int lastValidPosition = POSITION_INVALID;

	_position = position;

	if (_position != POSITION_INVALID) {
		lastValidPosition = _position;
	}

	//Update graphically the last valid position
	emit dataChanged(this->index(lastValidPosition, PlaylistModel::COLUMN_FIRST),
		this->index(lastValidPosition, PlaylistModel::COLUMN_LAST));
}

int PlaylistModel::position() const {
	return _position;
}
