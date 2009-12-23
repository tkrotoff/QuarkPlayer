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

#ifndef METADATAWRITER_H
#define METADATAWRITER_H

#include <MediaInfoFetcher/mediainfofetcher_export.h>

class MediaInfo;

/**
 * Writes the metadata of a MediaInfo to a file using TagLib.
 *
 * If TAGLIB is not defined, MediaInfoFetcher is compiled without TagLib
 * support and then MetaDataWriter won't work (will do nothing and return false).
 *
 * Very basic implementation for now, in the future will probably be threaded
 * via QtConcurrent::run
 *
 * @author Tanguy Krotoff
 */
class MEDIAINFOFETCHER_API MetaDataWriter {
public:

	/**
	 * Writes the metadata from a given MediaInfo to a file.
	 *
	 * The file where the metadata will be written/updated
	 * is the filename given to MediaInfo.
	 *
	 * @param mediaInfo metadata to write on the hard drive
	 * @return true if succeded; false otherwise
	 */
	static bool write(const MediaInfo & mediaInfo);

private:

	MetaDataWriter();

	~MetaDataWriter();
};

#endif	//METADATAWRITER_H
