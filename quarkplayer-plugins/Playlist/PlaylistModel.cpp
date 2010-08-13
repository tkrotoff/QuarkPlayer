/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "PlaylistCommandLineParser.h"
#include "PlaylistFilter.h"
#include "PlaylistLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/ConfigWindow/PlaylistConfig.h>

#include <MediaInfoFetcher/MediaInfoFetcher.h>

#include <FileTypes/FileTypes.h>

#include <TkUtil/FindFiles.h>
#include <TkUtil/Random.h>

#include <PlaylistParser/PlaylistParser.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

const int PlaylistModel::COLUMN_INFO = 0;
const int PlaylistModel::COLUMN_TRACK = 1;
const int PlaylistModel::COLUMN_TITLE = 2;
const int PlaylistModel::COLUMN_ARTIST = 3;
const int PlaylistModel::COLUMN_ALBUM = 4;
const int PlaylistModel::COLUMN_LENGTH = 5;
const int PlaylistModel::COLUMN_FIRST = COLUMN_INFO;
const int PlaylistModel::COLUMN_LAST = COLUMN_LENGTH;
static const int COLUMN_COUNT = PlaylistModel::COLUMN_LAST + 1;

static const int POSITION_INVALID = -1;
const int PlaylistModel::APPEND_FILES = -1;

static const char * PLAYLIST_TRACK_DISPLAY_MODE_KEY = "playlist_track_display_mode";
static const char * PLAYLIST_DEFAULT_FORMAT_KEY = "playlist_default_format";

//Key for PLAY_MEDIA_ERROR and PLAY_MEDIA_NOERROR
static const char * PLAY_MEDIA_STATE = "PLAY_MEDIA_STATE";
//String to indicates that an error occured while try to play the media
static const char * PLAY_MEDIA_ERROR = "PLAY_MEDIA_ERROR";
//String to indicates that no error occured while playing the media
static const char * PLAY_MEDIA_NOERROR = "PLAY_MEDIA_NOERROR";

PlaylistModel::PlaylistModel(QObject * parent, QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QAbstractListModel(parent),
	_quarkPlayer(quarkPlayer) {

	_commandLineParser = NULL;
	_uuid = uuid;
	_playlistFilter = NULL;

	clearInternal();

	connect(&_quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	//Info fetcher
	_mediaInfoFetcher = new MediaInfoFetcher(this);
	connect(_mediaInfoFetcher, SIGNAL(finished(const MediaInfo &)),
		SLOT(updateMediaInfo(const MediaInfo &)));

	if (PluginManager::instance().allPluginsAlreadyLoaded()) {
		//If all the plugins are already loaded...
		allPluginsLoaded();
	} else {
		//Optimization: loads the playlist only when all plugins have been loaded
		connect(&PluginManager::instance(), SIGNAL(allPluginsLoaded()),
			SLOT(allPluginsLoaded()), Qt::QueuedConnection);
	}

	Config::instance().addKey(PLAYLIST_TRACK_DISPLAY_MODE_KEY, TrackDisplayModeNormal);
	Config::instance().addKey(PLAYLIST_DEFAULT_FORMAT_KEY, "xspf");
}

PlaylistModel::~PlaylistModel() {
	delete _commandLineParser;
}

void PlaylistModel::setPlaylistFilter(PlaylistFilter * playlistFilter) {
	_playlistFilter = playlistFilter;
}

int PlaylistModel::columnCount(const QModelIndex & parent) const {
	Q_UNUSED(parent);
	return COLUMN_COUNT;
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const {
	QVariant tmp;

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case COLUMN_INFO:
			//Nothing to show in the header
			break;
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
			PlaylistCritical() << "Unknown column:" << section;
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
	MediaInfo mediaInfo(_fileNames[row]);

	if (role == Qt::DisplayRole) {
		QString fileName(mediaInfo.fileName());

		switch (column) {
		case COLUMN_TRACK: {
			TrackDisplayMode trackDisplayMode = static_cast<TrackDisplayMode>(
				Config::instance().value(PLAYLIST_TRACK_DISPLAY_MODE_KEY).toInt()
			);
			switch (trackDisplayMode) {
			case TrackDisplayModeNormal: {
				int trackNumber = mediaInfo.metaDataValue(MediaInfo::TrackNumber).toInt();
				if (trackNumber > 0) {
					tmp = trackNumber;
				}
				break;
			}
			case TrackDisplayModeWinamp:
				//Display track numbers like Winamp
				tmp = QString::number(row);
				break;
			default:
				PlaylistCritical() << "Unknown TrackDisplayMode:" << trackDisplayMode;
			}
			break;
		}
		case COLUMN_TITLE: {
			QString title(mediaInfo.metaDataValue(MediaInfo::Title).toString());
			if (title.isEmpty()) {
				if (mediaInfo.fetched()) {
					if (MediaInfo::isUrl(fileName)) {
						title = fileName;
					} else {
						//Not the fullpath, only the fileName
						title = QFileInfo(fileName).fileName();
					}
				} else {
					if (MediaInfo::isUrl(fileName)) {
						//So let's keep the complete filename
						title = fileName;
					} else {
						//filename + parent directory name, e.g:
						// /home/tanguy/Music/DJ Vadim/Bluebird.mp3
						// --> DJ Vadim/Bluebird.mp3
						//This allow to search within file and directory names without
						//resolving yet all the metadatas
						fileName = QDir::toNativeSeparators(fileName);
						int lastSlashPos = fileName.lastIndexOf(QDir::separator()) - 1;
						title = fileName.mid(fileName.lastIndexOf(QDir::separator(), lastSlashPos) + 1);
					}
				}
			}
			tmp = title;
			break;
		}
		case COLUMN_ARTIST:
			tmp = mediaInfo.metaDataValue(MediaInfo::Artist).toString();
			break;
		case COLUMN_ALBUM:
			tmp = mediaInfo.metaDataValue(MediaInfo::Album).toString();
			break;
		case COLUMN_LENGTH:
			tmp = mediaInfo.durationFormatted();
			break;
		}

		if (!mediaInfo.fetched()
			&& !MediaInfo::isUrl(fileName)
			&& (mediaInfo.cueStartIndex() == MediaInfo::CUE_INDEX_INVALID)) {

			//Resolve meta data file one by one
			//Cannot do that for remote/network media, i.e URLs
			if (_mediaInfoFetcherRow == POSITION_INVALID) {
				_mediaInfoFetcherRow = row;
				_mediaInfoFetcher->start(mediaInfo);
			}
		}
	}

	if (mediaInfo.extendedMetaData(PLAY_MEDIA_STATE).toString() == PLAY_MEDIA_ERROR) {
		//Probably file not found error
		//Show a special icon to indicate that there is an error

		if (role == Qt::DecorationRole) {
			switch (column) {
			case COLUMN_INFO:
				tmp = QIcon::fromTheme("dialog-warning");
			}
		}
		else if (role == Qt::ToolTipRole) {
			switch (column) {
			case COLUMN_INFO:
				tmp = tr("File couldn't be read");
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

		else if (role == Qt::BackgroundRole) {
			tmp = QApplication::palette().alternateBase();
		}

		//Add an icon to show that the file is playing
		else if (role == Qt::DecorationRole) {
			if (column == COLUMN_INFO) {
				tmp = QIcon::fromTheme("go-jump");
			}
		}
	}

	return tmp;
}

int PlaylistModel::rowCount(const QModelIndex & parent) const {
	if (parent.isValid()) {
		return 0;
	}

	return _fileNames.size();
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex & index) const {
	if (!index.isValid()) {
		return Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

bool PlaylistModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) {
	Q_UNUSED(action);
	Q_UNUSED(column);
	Q_UNUSED(parent);

	QStringList files;

	//Add urls to a list
	if (data->hasUrls()) {
		QList<QUrl> urlList = data->urls();
		QString fileName;
		foreach (QUrl url, urlList) {
			if (url.isValid()) {
				PlaylistDebug() << "File scheme:" << url.scheme();
				if (url.scheme() == "file") {
					fileName = url.toLocalFile();
				} else {
					fileName = url.toString();
				}
				files << fileName;
				PlaylistDebug() << "Filename:" << fileName;
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

void PlaylistModel::filesFound(const QList<MediaInfo> & files) {
	addFiles(files, _rowWhereToInsertFiles);
}

void PlaylistModel::addFiles(const QList<MediaInfo> & files, int row) {
	_rowWhereToInsertFiles = row;
	insertFilesInsideTheModel(files, row);
}

void PlaylistModel::addFiles(const QStringList & files, int row) {
	_rowWhereToInsertFiles = row;

	QList<MediaInfo> fileList;
	foreach (QString fileName, files) {
		bool isMultimediaFile =
			FileTypes::fileExtensionMatches(fileName, FileTypes::extensions(FileType::Video, FileType::Audio));
		if (isMultimediaFile) {
			fileList << MediaInfo(fileName);
		} else if (FileTypes::fileExtensionMatches(fileName, FileTypes::extensions(FileType::Playlist))) {
			loadPlaylist(fileName);
		} else if (QFileInfo(fileName).isDir()) {
			_nbFindFiles++;
			FindFiles * findFiles = new FindFiles(this);
			connect(findFiles, SIGNAL(filesFound(const QStringList &, const QUuid &)),
				SLOT(filesFound(const QStringList &)));
			connect(findFiles, SIGNAL(finished(int, const QUuid &)),
				SLOT(searchFinished(int)));
			findFiles->setSearchPath(fileName);
			findFiles->setFilesFoundLimit(500);
			findFiles->setFindDirs(false);
			FindFiles::setBackend(static_cast<FindFiles::Backend>(
				Config::instance().value(Config::FINDFILES_BACKEND_KEY).toInt()));
			findFiles->start(QUuid::createUuid());
		} else if (MediaInfo::isUrl(fileName)) {
			//A filename that contains a host/server name is a remote/network media
			//So it should be added to the playlist
			MediaInfo mediaInfo(fileName);
			fileList << mediaInfo;
		} else if (fileName.contains("internal=")) {
			//OK this is a hack, an internal command for the playlist
			fileName.remove("internal=");
			fileList << MediaInfo(fileName);
		}
	}

	insertFilesInsideTheModel(fileList, row);
}

void PlaylistModel::insertFilesInsideTheModel(const QList<MediaInfo> & files, int row) {
	if (files.isEmpty()) {
		return;
	}

	int first = 0;
	if (row == APPEND_FILES) {
		//row == APPEND_FILES means append the files
		first = _fileNames.size();
	} else {
		//row != APPEND_FILES means we have a specific row location where to add the files
		first = row;
	}
	int last = first + files.size() - 1;
	int currentRow = first;

	//Insert rows inside QModel
	//This will tell the Widget (view) the model has changed
	//and that the Widget (view) needs to be updated
	beginInsertRows(QModelIndex(), first, last);
	foreach (MediaInfo mediaInfo, files) {
		_fileNames.insert(currentRow, mediaInfo);
		currentRow++;
	}
	endInsertRows();
	///

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
		int count = files.size();
		if (_position != POSITION_INVALID && row != APPEND_FILES) {
			if (_position >= row) {
				_position += count;
			}
		}
	}
}

void PlaylistModel::searchFinished(int timeElapsed) {
	Q_UNUSED(timeElapsed);

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
		_fileNames.removeAt(row);
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
	return "/playlist_" + _uuid.toString() + '.' + Config::instance().value(PLAYLIST_DEFAULT_FORMAT_KEY).toString().toLower();
}

void PlaylistModel::loadPlaylist(const QString & fileName) {
	static PlaylistReader * parser = new PlaylistReader(this);
	connect(parser, SIGNAL(filesFound(const QList<MediaInfo> &)),
		SLOT(filesFound(const QList<MediaInfo> &)));
	connect(parser, SIGNAL(finished(PlaylistParser::Error, int)),
		SIGNAL(playlistLoaded(PlaylistParser::Error, int)));
	parser->load(fileName);
}

void PlaylistModel::loadCurrentPlaylist() {
	//Restore last current playlist
	loadPlaylist(Config::instance().configDir() + currentPlaylist());
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

		QString path(Config::instance().configDir());
		PlaylistWriter * parser = new PlaylistWriter(this);
		connect(parser, SIGNAL(finished(PlaylistParser::Error, int)),
			SIGNAL(playlistSaved(PlaylistParser::Error, int)));
		parser->save(path + currentPlaylist(), _fileNames);
	}
}

const QList<MediaInfo> & PlaylistModel::files() const {
	return _fileNames;
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
			QString fileName(_fileNames[dragAndDropRow].fileName());
			if (!fileName.isEmpty()) {
				files << fileName;
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

void PlaylistModel::updateMediaInfo(const MediaInfo & mediaInfo) {
	if (_mediaInfoFetcherRow == POSITION_INVALID) {
		PlaylistCritical() << "_mediaInfoFetcherRow invalid";
	} else {
		if (_mediaInfoFetcherRow < _fileNames.size()) {
			MediaInfo mediaInfo2 = _fileNames[_mediaInfoFetcherRow];
			QString fileName(mediaInfo2.fileName());

			if ((fileName == mediaInfo.fileName())
				&& !mediaInfo2.fetched()
				&& !MediaInfo::isUrl(fileName)
				&& (mediaInfo2.cueStartIndex() == MediaInfo::CUE_INDEX_INVALID)) {

				_fileNames[_mediaInfoFetcherRow] = mediaInfo;

				//Update the row since the matching MediaSource has been modified
				emit dataChanged(index(_mediaInfoFetcherRow, COLUMN_FIRST),
					index(_mediaInfoFetcherRow, COLUMN_LAST));
			}
		}
	}
	_mediaInfoFetcherRow = POSITION_INVALID;
}

MediaInfo PlaylistModel::mediaInfo(const QModelIndex & index) const {
	MediaInfo tmp;
	if (!index.isValid()) {
		PlaylistCritical() << "Invalid index:" << index;
	} else {
		int row = index.row();
		tmp = _fileNames[row];
	}
	return tmp;
}

void PlaylistModel::clearInternal() {
	_fileNames.clear();
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
	if (position == POSITION_INVALID) {
		//Invalid position, do nothing
		return;
	}

	_positionToPlay = position;
	if (_positionToPlay < _fileNames.count()) {
		playInternal();
	} else {
		//We need to wait until the file has been added to the playlist/model
		//before we can actually play it
		connect(this, SIGNAL(rowsInserted(const QModelIndex &, int, int)), SLOT(playInternal()));
	}
}

void PlaylistModel::playInternal() {
	if (_positionToPlay == POSITION_INVALID) {
		//Invalid position, do nothing
		return;
	}

	if (_positionToPlay < _fileNames.count()) {

		disconnect(SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(playInternal()));

		setPosition(_positionToPlay);
		if (_position != POSITION_INVALID) {
			QString fileName(_fileNames[_positionToPlay].fileName());
			PlaylistDebug() << "Play file:" << fileName;

			connectToMediaObject(_quarkPlayer.currentMediaObject());

			_quarkPlayer.play(fileName);
		} else {
			PlaylistCritical() << "Invalid position";
		}
	} else {
		//Still have to wait...
	}
}

void PlaylistModel::connectToMediaObject(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, _quarkPlayer.mediaObjectList()) {
		tmp->disconnect(this);
		//tmp->disconnect(_playlistFilter);
	}

	//FIXME aboutToFinish does not work properly with DS9 backend???
	//aboutToFinish -> let's queue/play the next track

	//FIXME Huston, we've got a very strange bug
	//This does not work, we have to create a slot inside PlaylistModel
	//instead of using directly the slot inside _playlistFilter
	//otherwise QObject::connect() only works half of the time :/
	//connect(mediaObject, SIGNAL(aboutToFinish()),
	//	_playlistFilter, SLOT(enqueueNextTrack()));
	connect(mediaObject, SIGNAL(aboutToFinish()), SLOT(enqueueNextTrack()));
	///

	connect(mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
		SLOT(currentSourceChanged(const Phonon::MediaSource &)));

	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State)));

	//Needed for CUE sheet functionality
	connect(mediaObject, SIGNAL(tick(qint64)),
		SLOT(tick(qint64)));
}

void PlaylistModel::enqueueNextTrack() {
	_playlistFilter->enqueueNextTrack();
}

void PlaylistModel::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	if (PlaylistConfig::instance().activePlaylist() == _uuid) {
		connectToMediaObject(mediaObject);
	}
}

void PlaylistModel::currentSourceChanged(const Phonon::MediaSource & source) {
	Q_UNUSED(source);

	if (PlaylistConfig::instance().activePlaylist() == _uuid) {
		//Each time the track changes, we enqueue the next track
		//currentSourceChanged() is the only signal that we get when we queue tracks
		_playlistFilter->setPositionAsNextTrack();
	}
}

void PlaylistModel::stateChanged(Phonon::State newState) {
	switch (newState) {
	case Phonon::ErrorState: {
		Phonon::ErrorType errorType = _quarkPlayer.currentMediaObject()->errorType();
		switch (errorType) {
		case Phonon::NoError:
			//Not possible to be in this case?
			//Let's crash...
			Q_ASSERT(false);
			break;
		case Phonon::NormalError:
			//Do not jump to the next track
			break;
		case Phonon::FatalError: {
			if (_position != POSITION_INVALID) {
				//Show that the playing of the media was not possible
				MediaInfo mediaInfo = _fileNames[_position];
				mediaInfo.setExtendedMetaData(PLAY_MEDIA_STATE, QString(PLAY_MEDIA_ERROR));
				_fileNames[_position] = mediaInfo;
				///
			}

			//Jump to the next track if possible since the current one is not playable
			//Do not repeat again and again the whole playlist: stops at the end of the playlist
			_playlistFilter->playNextTrack(false);

			break;
		}
		default:
			Q_ASSERT(false);
		}
		break;
	}

	case Phonon::PlayingState: {
		if (_position != POSITION_INVALID) {
			MediaInfo mediaInfo = _fileNames[_position];

			//Show that there is no error while playing the media
			mediaInfo.setExtendedMetaData(PLAY_MEDIA_STATE, PLAY_MEDIA_NOERROR);
			_fileNames[_position] = mediaInfo;
			///

			int cueStartIndex = mediaInfo.cueStartIndex();
			if (cueStartIndex != MediaInfo::CUE_INDEX_INVALID) {
				//MediaInfo was created using a CUE sheet
				_quarkPlayer.currentMediaObject()->seek(cueStartIndex);
			}
		}
		break;
	}

	case Phonon::StoppedState:
		break;

	case Phonon::PausedState:
		break;

	case Phonon::LoadingState:
		break;

	case Phonon::BufferingState:
		break;

	default:
		Q_ASSERT(false);
	}
}

void PlaylistModel::tick(qint64 time) {
	if (_position != POSITION_INVALID) {
		//_position can be invalid

		MediaInfo mediaInfo = _fileNames[_position];
		int cueEndIndex = mediaInfo.cueEndIndex();

		if (cueEndIndex != MediaInfo::CUE_INDEX_INVALID
			&& time >= cueEndIndex) {

			//MediaInfo was created using a CUE sheet
			//we have reached the end of the media given the CUE sheet end index

			//Jump to the next track if possible since the current one is not playable
			//Do repeat again and again the whole playlist
			_playlistFilter->playNextTrack(true);
		}
	}
}

void PlaylistModel::enqueue(int position) {
	if (position != POSITION_INVALID) {
		//Important to clear the queue: otherwise we can get some strange behaviors
		//One never knows what is inside the queue of the backend,
		//better to erase it and be sure
		_quarkPlayer.currentMediaObject()->clearQueue();

		QString fileName(_fileNames[position].fileName());
		PlaylistDebug() << "Enqueue file:" << fileName;
		_quarkPlayer.currentMediaObject()->enqueue(fileName);
	} else {
		PlaylistCritical() << "Invalid position";
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

void PlaylistModel::allPluginsLoaded() {
	//Parses the command line arguments
	//new PlaylistCommandLineParser() is needed since PlaylistCommandLineParser waits for the argsReceived() signal
	_commandLineParser = new PlaylistCommandLineParser(this);
}
