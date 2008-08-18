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

#ifndef FILETYPE_H
#define FILETYPE_H

#include <QtCore/QString>

struct FileType {
	enum Category {
		CategoryUnknown,
		Video,
		Audio,
		Playlist,
		Subtitle
	};

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
		Theora,
		WMV,
		DivX,
		NUT,
		OGM,

		//Audio

		//Lossless audio
		AIFF,
		AU,
		WAV,
		FLAC,
		AppleLossless,
		APE,
		TTA,
		WavPack,
		WMA,

		//Lossy audio
		MP2,
		MP3,
		Speex,
		Vorbis,
		AAC,
		MPC,
		VQF,
		RealMedia,
		MID,
		MOD,

		//Playlist
		ASX,
		M3U,
		PLS,
		XSPF,
		WPL,

		//Subtitle
		AQTitle,
		MicroDVD,
		MPsub,
		SSA,
		SubViewer,
		RealText,
		SAMI,
		SSF,
		SubRip,
		USF,
		VOBsub
	};

	Category category;
	Name name;
	QString fullName;
	QString extensions;
	QString mimeTypes;
};

#endif	//FILETYPE_H
