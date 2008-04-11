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
}

void MediaData::print() {
	qDebug() << __FUNCTION__;

	qDebug() << "  filename:" << filename;
	qDebug() << "  totalTime:" << totalTime;

	qDebug() << "  video_width:" << video_width;
	qDebug() << "  video_height:" << video_height;
	qDebug() << "  video_aspect:" << video_aspect;

	qDebug() << "  novideo:" << novideo;
	qDebug() << "  dvd_id:" << dvd_id;

	qDebug() << "  initialized:" << initialized;

	qDebug() << "  mkv_chapters:" << mkv_chapters;

	//qDebug() << "  Subs:";
	//subs.print();

	//qDebug() << "  Audios:";
	//audios.print();

	//qDebug() << "  Titles:";
	//titles.print();

	qDebug() << "  demuxer:" <<  demuxer;
	qDebug() << "  video_format:" << video_format;
	qDebug() << "  audio_format:" << audio_format;
	qDebug() << "  video_bitrate:" << video_bitrate;
	qDebug() << "  video_fps:" << video_fps;
	qDebug() << "  audio_bitrate:" << audio_bitrate;
	qDebug() << "  audio_rate:" << audio_rate;
	qDebug() << "  audio_nch:" << audio_nch;
	qDebug() << "  video_codec:" << video_codec;
	qDebug() << "  audio_codec:" << audio_codec;
}
