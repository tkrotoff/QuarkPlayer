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

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/FileExtensions.h>

#include <tkutil/FindFiles.h>
#include <tkutil/TkFile.h>

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
	qDebug() << __FUNCTION__ << "newState:" << newState << "oldState:" << oldState;

	Phonon::MediaSource source = _metaObjectInfoResolver->currentSource();
	QMap<QString, QString> metaData = _metaObjectInfoResolver->metaData();

	if (newState == Phonon::ErrorState) {
		/*QMessageBox::warning(this, tr("Error opening files"),
				_metaObjectInfoResolver->errorString());

		while (!_mediaSources.isEmpty() &&
			!(_mediaSources.takeLast() == source));*/
		return;
	}

	if (newState != Phonon::StoppedState && newState != Phonon::PausedState) {
		return;
	}

	if (source.type() == Phonon::MediaSource::Invalid) {
		return;
	}

	qDebug() << __FUNCTION__ << source.fileName();

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
	emit dataChanged(index(row, 0), index(row, COLUMN_COUNT));
}

void PlaylistModel::playNextTrack() {
	_position++;

	if (_position < 0 || _position >= _mediaSources.size()) {
		//Back to the top of the playlist
		_position = 0;
	}

	_quarkPlayer.play(_mediaSources[_position].mediaSource());
}

void PlaylistModel::playPreviousTrack() {
	_position--;

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




//Track class
PlaylistModel::Track::Track(const Phonon::MediaSource & mediaSource) {
	_source = mediaSource;

	QString shortFilename;
	switch (_source.type()) {
	case Phonon::MediaSource::LocalFile:
		_filename = _source.fileName();
		shortFilename = TkFile::fileName(_filename);
		break;
	case Phonon::MediaSource::Url:
		shortFilename = _filename = _source.url().toString();
		break;
	case Phonon::MediaSource::Disc:
		shortFilename = _filename = _source.deviceName();
		break;
	case Phonon::MediaSource::Invalid: {
		//Try to get the filename from the url
		QUrl url(_source.url());
		if (url.isValid()) {
			if (url.scheme() == "file") {
				_filename = url.toLocalFile();
				shortFilename = TkFile::fileName(_filename);
			} else {
				shortFilename = _filename = url.toString();
			}
		}
		break;
	}
	default:
		qCritical() << __FUNCTION__ << "Error: unknown MediaSource type:" << _source.type();
	}

	//By default, title is just the filename
	_title = shortFilename;
}

PlaylistModel::Track::Track(const Track & track) {
	copy(track);
}

PlaylistModel::Track::~Track() {
}

void PlaylistModel::Track::copy(const Track & track) {
	_source = track._source;

	_filename = track._filename;

	_trackNumber = track._trackNumber;
	_title = track._title;
	_artist = track._artist;
	_album = track._album;
	_length = track._length;
}

PlaylistModel::Track & PlaylistModel::Track::operator=(const Track & right) {
	//Handle self-assignment
	if (this == &right) {
		return *this;
	}

	copy(right);
	return *this;
}

int PlaylistModel::Track::operator==(const Track & right) {
	return _filename == right._filename;
}

QString PlaylistModel::Track::fileName() const {
	return _filename;
}

Phonon::MediaSource PlaylistModel::Track::mediaSource() const {
	return _source;
}

void PlaylistModel::Track::setTrackNumber(const QString & trackNumber) {
	_trackNumber = trackNumber;
}

QString PlaylistModel::Track::trackNumber() const {
	return _trackNumber;
}

void PlaylistModel::Track::setTitle(const QString & title) {
	if (title.isEmpty()) {
		//Not the fullpath, only the filename
		_title = TkFile::fileName(_filename);
	} else {
		_title = title;
	}
}

QString PlaylistModel::Track::title() const {
	return _title;
}

void PlaylistModel::Track::setArtist(const QString & artist) {
	_artist = artist;
}

QString PlaylistModel::Track::artist() const {
	return _artist;
}

void PlaylistModel::Track::setAlbum(const QString & album) {
	_album = album;
}

QString PlaylistModel::Track::album() const {
	return _album;
}

void PlaylistModel::Track::setLength(const QString & length) {
	_length = convertMilliseconds(length.toULongLong());
}

QString PlaylistModel::Track::length() const {
	return _length;
}

QString PlaylistModel::Track::convertMilliseconds(qint64 totalTime) const {
	QTime displayTotalTime((totalTime / 3600000) % 60, (totalTime / 60000) % 60, (totalTime / 1000) % 60);

	QString timeFormat;

	if (displayTotalTime.hour() == 0 && displayTotalTime.minute() == 0 &&
		displayTotalTime.second() == 0 && displayTotalTime.msec() == 0) {
		//Total time is 0, return nothing
		return QString();
	} else {
		if (displayTotalTime.hour() > 0) {
			timeFormat = "hh:mm:ss";
		} else {
			timeFormat = "mm:ss";
		}
		return displayTotalTime.toString(timeFormat);
	}
}
