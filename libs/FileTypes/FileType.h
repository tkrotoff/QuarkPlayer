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

#ifndef FILETYPE_H
#define FILETYPE_H

#include <QtCore/QString>

/**
 * Type/format (FLAC, DivX, MP3...) of a file.
 *
 * This class supports all the types/formats supported by QuarkPlayer.
 *
 * @author Tanguy Krotoff
 */
struct FileType {

	/** File type category: Video, Audio, Playlist, Subtitle... */
	enum Category {
		CategoryUnknown,
		Video,
		Audio,
		Playlist,
		Subtitle
	};

	/** File type name/format: FLAC, DivX... */
	enum Name {
		NameUnknown,

		//Video
		_3GP,
		ASF,
		AVI,
		MPEG1,
		FLV,
		SWF,
		MKV,
		MOV,
		MP4,
		NSV,
		OGM,
		Tarkin,
		Theora,
		RealMedia,
		WMV,
		DivX,
		Xvid,
		NUT,
		Smacker,
		Bink,
		VOB,
		EVO,

		//Audio

		//Lossless audio
		AIFF,
		AU,
		CDDA,
		WAV,
		FLAC,
		AppleLossless,
		APE,
		TTA,
		WavPack,
		WMA,
		OptimFROG,

		//Lossy audio
		MP2,
		MP3,
		Speex,
		Vorbis,
		AAC,
		MPC,
		VQF,
		RealAudio,
		MID,
		MOD,

		//Playlist
		ASX,
		M3U,
		PLS,
		XSPF,
		WPL,
		CUE,

		//Subtitle
		AQTitle,
		MicroDVD,
		MPsub,
		RealText,
		SAMI,
		SSF,
		SubRip,
		SSA,
		SubViewer,
		USF,
		VOBsub
	};

	/** File type category: Video, Audio, Playlist, Subtitle... */
	Category category;

	/** File type name/format: FLAC, DivX... */
	Name name;

	/**
	 * Full file type name.
	 *
	 * Example: FLAC full name is "Free Lossless Audio Codec (FLAC)"
	 */
	QString fullName;

	/**
	 * Link to the Wikipedia article for this particular file type.
	 *
	 * Cannot be a QUrl since this variable does not contain the entire URL,
	 * just the last part (most important part).
	 * Example: FLAC Wikipedia article is "Free_Lossless_Audio_Codec"
	 */
	QString wikipediaArticle;

	/**
	 * File extensions (wihtout the dot) matching this particular file type.
	 *
	 * Example: FLAC file extensions is flac
	 */
	QString extensions;

	/**
	 * Mime types matching this particular file type.
	 *
	 * Example: FLAC mime types are "audio/x-flac,audio/x-oggflac"
	 */
	QString mimeTypes;
};

#endif	//FILETYPE_H
