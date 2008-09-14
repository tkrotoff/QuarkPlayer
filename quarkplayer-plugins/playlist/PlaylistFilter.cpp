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
	_previousPosition = POSITION_INVALID;

	setSourceModel(_playlistModel);
}

bool PlaylistFilter::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const {
	QString artist(_playlistModel->data(_playlistModel->index(sourceRow, PlaylistModel::COLUMN_ARTIST, sourceParent)).toString());
	QString album(_playlistModel->data(_playlistModel->index(sourceRow, PlaylistModel::COLUMN_ALBUM, sourceParent)).toString());
	QString title(_playlistModel->data(_playlistModel->index(sourceRow, PlaylistModel::COLUMN_TITLE, sourceParent)).toString());

	bool keep = artist.contains(filterRegExp()) ||
			album.contains(filterRegExp()) ||
			title.contains(filterRegExp());

	return keep;
}

QModelIndex PlaylistFilter::currentIndex() const {
	return _playlistModel->index(_playlistModel->position(), PlaylistModel::COLUMN_FIRST);
}

int PlaylistFilter::convertCurrentModelToFilterPosition() {
	QModelIndex index = _playlistModel->index(_playlistModel->position(), PlaylistModel::COLUMN_FIRST);
	return mapFromSource(index).row();
}

int PlaylistFilter::convertToModelPosition(const QModelIndex & index) {
	int position = POSITION_INVALID;
	if (index.isValid()) {
		const QAbstractItemModel * model = index.model();
		if (model == this) {
			position = mapToSource(index).row();
		} else if (model == _playlistModel) {
			position = index.row();
		}
	}
	return position;
}

void PlaylistFilter::play(const QModelIndex & index) {
	int position = convertToModelPosition(index);
	_previousPosition = position;
	_playlistModel->play(position);
}

void PlaylistFilter::playNextTrack() {
	play(nextTrack());
}

void PlaylistFilter::playPreviousTrack() {
	play(previousTrack());
}

void PlaylistFilter::enqueueNextTrack() {
	_playlistModel->setPosition(_previousPosition);

	int nextPosition = convertToModelPosition(nextTrack());
	_playlistModel->enqueue(nextPosition);

	_previousPosition = nextPosition;
}

QModelIndex PlaylistFilter::nextTrack() {
	int position = convertCurrentModelToFilterPosition();

	if (_shuffle) {
		position = Random::randomInt(0, rowCount() - 1);
	} else {
		position++;
	}

	if (position < 0 || position >= rowCount()) {
		//Back to the top of the playlist
		position = 0;
	}

	return mapToSource(index(position, PlaylistModel::COLUMN_FIRST));
}

QModelIndex PlaylistFilter::previousTrack() {
	int position = convertCurrentModelToFilterPosition();

	if (_shuffle) {
		position = Random::randomInt(0, rowCount() - 1);
	} else {
		position--;
	}

	if (position < 0 || position >= rowCount()) {
		//Back to the bottom of the playlist
		position = rowCount() - 1;
	}

	return mapToSource(index(position, PlaylistModel::COLUMN_FIRST));
}

void PlaylistFilter::setShuffle(bool shuffle) {
	_shuffle = shuffle;
}

void PlaylistFilter::setRepeat(bool repeat) {
	_repeat = repeat;
}
