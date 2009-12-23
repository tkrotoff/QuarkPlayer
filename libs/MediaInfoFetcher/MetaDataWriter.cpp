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

#include "MetaDataWriter.h"

#include "config.h"
#include "MediaInfo.h"

#ifdef TAGLIB
	#include <taglib/fileref.h>
	#include <taglib/tag.h>
#endif	//TAGLIB

#include <QtCore/QDate>
#include <QtCore/QDebug>

//Local version of taglib's QStringToTString macro. It is here, because taglib's one is
//not Qt3Support clean (uses QString::utf8()). Once taglib will be clean of qt3support
//it is safe to use QStringToTString again
#define Qt4QStringToTString(s) TagLib::String(s.toUtf8().data(), TagLib::String::UTF8)

bool MetaDataWriter::write(const MediaInfo & mediaInfo) {
#ifdef TAGLIB
	if (MediaInfo::isUrl(mediaInfo.fileName())) {
		qWarning() << __FUNCTION__ << "This MediaInfo is not a real file:" << mediaInfo.fileName();
		return false;
	}

	//Taken from Amarok, file: CollectionScanner.cpp
	//See http://gitorious.org/amarok/amarok/trees/master/utilities/collectionscanner/CollectionScanner.cpp
#ifdef Q_OS_WIN
	const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(mediaInfo.fileName().utf16());
#else
	const char * encodedName = QFile::encodeName(mediaInfo.fileName()).constData();
#endif	//Q_OS_WIN

	TagLib::FileRef fileRef(encodedName, true);

	if (fileRef.isNull()) {
		qCritical() << __FUNCTION__ << "Error: the FileRef is null:" << mediaInfo.fileName();
		return false;
	} else {

		TagLib::Tag * tag = fileRef.tag();
		if (tag) {
			//Do it before anything else, very generic, can contains ID3v1 tags I guess
			tag->setTrack(mediaInfo.metaDataValue(MediaInfo::TrackNumber).toInt());
			tag->setTitle(Qt4QStringToTString(mediaInfo.metaDataValue(MediaInfo::Title).toString()));
			tag->setArtist(Qt4QStringToTString(mediaInfo.metaDataValue(MediaInfo::Artist).toString()));
			tag->setAlbum(Qt4QStringToTString(mediaInfo.metaDataValue(MediaInfo::Album).toString()));
			tag->setYear(mediaInfo.metaDataValue(MediaInfo::Year).toDate().year());
			tag->setGenre(Qt4QStringToTString(mediaInfo.metaDataValue(MediaInfo::Genre).toString()));
			tag->setComment(Qt4QStringToTString(mediaInfo.metaDataValue(MediaInfo::Comment).toString()));
		}
	}

	//Saves the metadata
	fileRef.save();
#endif	//TAGLIB

	return true;
}
