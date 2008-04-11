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

#ifndef MEDIADATA_H
#define MEDIADATA_H

#include <QtCore/QString>

/**
 * Stores some volatile informations about the stream.
 *
 * Contains the file meta data + others informations.
 *
 * @see http://xiph.org/vorbis/doc/Vorbis_I_spec.html#vorbis-spec-comment
 */
class MediaData {
public:

	MediaData();
	~MediaData();

	/** Reset all datas to zero. */
	void clear();

	void print();

	/** Filename of the media. */
	QString filename;

	/** Media total time (duration) in seconds. */
	double totalTime;

	/**
	 * Current playing time of the media.
	 *
	 * Updated each time MPlayer plays the media.
	 *
	 * If MPlayer is not playing, then the value is not valid
	 */
	double currentTime;

	/** Id of the widget where MPlayer will show the video. */
	int videoWidgetId;

	//Resolution of the video
	int video_width;
	int video_height;
	double video_aspect;

	QString dvd_id;

	//Only audio
	bool novideo;

	bool initialized;

	//TrackList audios;
	//For DVDs
	//TrackList titles;

	//SubTracks subs;

	//Matroska chapters
	int mkv_chapters;

	//Clip info
	QString clip_name;
	QString clip_artist;
	QString clip_author;
	QString clip_album;
	QString clip_genre;
	QString clip_date;
	QString clip_track;
	QString clip_copyright;
	QString clip_comment;
	QString clip_software;

	QString stream_title;
	QString stream_url;

	//Other data not really useful for us,
	//just to show info to the user
	QString demuxer;
	QString video_format;
	QString audio_format;
	int video_bitrate;
	QString video_fps;
	int audio_bitrate;
	int audio_rate;
	//Channels?
	int audio_nch;
	QString video_codec;
	QString audio_codec;
};

#endif	//MEDIADATA_H
