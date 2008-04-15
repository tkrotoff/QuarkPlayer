/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MediaData.h"

#include <QtCore/QtDebug>

#include <cmath>

MediaData::MediaData() {
	clear();
}

MediaData::~MediaData() {
}

void MediaData::clear() {
	/*
	filename.clear();
	dvd_id.clear();

	totalTime = 0;
	currentTime = 0;

	videoWidgetId = 0;

	novideo = false;

	video_width = 0;
	video_height = 0;
	video_aspect= (double) 4 / 3;

	//audios.clear();
	//titles.clear();

	//subs.clear();

	mkv_chapters = 0;

	initialized = false;

	clip_name.clear();
	clip_artist.clear();
	clip_author.clear();
	clip_album.clear();
	clip_genre.clear();
	clip_date.clear();
	clip_track.clear();
	clip_copyright.clear();
	clip_comment.clear();
	clip_software.clear();

	stream_title.clear();
	stream_url.clear();

	demuxer.clear();
	video_format.clear();
	audio_format.clear();
	video_bitrate = 0;
	video_fps.clear();
	audio_bitrate = 0;
	audio_rate = 0;
	audio_nch = 0;
	video_codec.clear();
	audio_codec.clear();
	*/
}
