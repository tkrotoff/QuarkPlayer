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

#include "PlaylistFilter.h"

#include "PlaylistModel.h"

#include <tkutil/Random.h>

#include <QtCore/QStringList>
#include <QtCore/QDebug>

static const int POSITION_INVALID = -1;

PlaylistFilter::PlaylistFilter(QObject * parent, PlaylistModel * playlistModel)
	: QSortFilterProxyModel(parent),
	_playlistModel(playlistModel) {

	_shuffle = false;
	_repeat = false;
	_position = POSITION_INVALID;

	setSourceModel(_playlistModel);
}

bool PlaylistFilter::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const {
	if (_filter.isEmpty()) {
		return true;
	}

	QString artist(_playlistModel->data(_playlistModel->index(sourceRow, PlaylistModel::COLUMN_ARTIST, sourceParent)).toString());
	QString album(_playlistModel->data(_playlistModel->index(sourceRow, PlaylistModel::COLUMN_ALBUM, sourceParent)).toString());
	QString title(_playlistModel->data(_playlistModel->index(sourceRow, PlaylistModel::COLUMN_TITLE, sourceParent)).toString());
	bool keep = true;
	foreach(QString word, _filter.split(' ')) {
		keep &= artist.contains(word, Qt::CaseInsensitive) ||
			album.contains(word, Qt::CaseInsensitive) ||
			title.contains(word, Qt::CaseInsensitive);
	}
	return keep;
}

void PlaylistFilter::setFilter(const QString & filter) {
	if (filter != _filter) {
		_filter = filter;
		QModelIndex current = currentIndex();
		invalidateFilter();
		if (current.isValid()) {
			_position = mapFromSource(current).row();
		}
		emit filterChanged();
	}
}

QModelIndex PlaylistFilter::currentIndex() const {
	if (_position >= 0) {
		return _playlistModel->index(_playlistModel->position(), PlaylistModel::COLUMN_FIRST);
	}
	return QModelIndex();
}

void PlaylistFilter::setCurrentIndex(const QModelIndex & index) {
	if (!index.isValid()) {
		_position = POSITION_INVALID;
		_playlistModel->setPosition(_position);
		return;
	}

	const QAbstractItemModel * model = index.model();
	if (model == this) {
		_position = index.row();
		_playlistModel->setPosition(mapToSource(index).row());
	} else if (model == _playlistModel) {
		_position = mapFromSource(index).row();
		_playlistModel->setPosition(index.row());
	}

	emit dataChanged(this->index(_position, PlaylistModel::COLUMN_FIRST),
			this->index(_position, PlaylistModel::COLUMN_LAST));
}

void PlaylistFilter::play(const QModelIndex & index) {
	_playlistModel->play(index);
	setCurrentIndex(index);
}

void PlaylistFilter::playNextTrack() {
	play(nextTrack());
}

void PlaylistFilter::playPreviousTrack() {
	play(previousTrack());
}

void PlaylistFilter::enqueueNextTrack() {
	if (_shuffle) {
		_position = Random::randomInt(0, rowCount() - 1);
	} else {
		_position++;
	}

	if (_position < 0 || _position >= rowCount()) {
		if (_repeat) {
			//Back to the top of the playlist
			_position = 0;
		}
	}

	if (_position >= 0 && _position < rowCount()) {
		QModelIndex nextIndex = mapToSource(index(_position, PlaylistModel::COLUMN_FIRST));
		_playlistModel->enqueue(nextIndex);
		setCurrentIndex(nextIndex);
	}
}

QModelIndex PlaylistFilter::nextTrack() const {
	if (_shuffle) {
		_position = Random::randomInt(0, rowCount() - 1);
	} else {
		_position++;
	}

	if (_position < 0 || _position >= rowCount()) {
		//Back to the top of the playlist
		_position = 0;
	}

	return mapToSource(index(_position, PlaylistModel::COLUMN_FIRST));
}

QModelIndex PlaylistFilter::previousTrack() const {
	if (_shuffle) {
		_position = Random::randomInt(0, rowCount() - 1);
	} else {
		_position--;
	}

	if (_position < 0 || _position >= rowCount()) {
		//Back to the bottom of the playlist
		_position = rowCount() - 1;
	}

	return mapToSource(index(_position, PlaylistModel::COLUMN_FIRST));
}

void PlaylistFilter::setShuffle(bool shuffle) {
	_shuffle = shuffle;
}

void PlaylistFilter::setRepeat(bool repeat) {
	_repeat = repeat;
}
