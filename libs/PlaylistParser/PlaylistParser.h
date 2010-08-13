/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef PLAYLISTPARSER_H
#define PLAYLISTPARSER_H

#include <PlaylistParser/IPlaylistParser.h>

#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QTime>

class IPlaylistParserImpl;

class QString;

/**
 * Parses a playlist.
 *
 * Code factorization.
 *
 * A playlist file can be a .m3u, m3u8, .pls, .wpl...
 * Prefered format is m3u8 (8 means UTF-8) "The most popular format, M3U, is also the simplest"
 *
 * @see http://en.wikipedia.org/wiki/M3u
 * @see http://gonze.com/playlists/playlist-format-survey.html
 * @author Tanguy Krotoff
 */
class PLAYLISTPARSER_API PlaylistParser : public IPlaylistParser {
	Q_OBJECT
public:

	enum Error {
		/** No error condition. */
		NoError,

		/** Playlist format not supported. */
		UnsupportedFormatError,

		/**
		 * Playlist file could not be read/write using QFile.
		 *
		 * Use file().error() in order to get more informations.
		 */
		FileError,
	};

	PlaylistParser(QObject * parent);

	virtual ~PlaylistParser();

	void stop();

	const QFile & file() const;

signals:

	/**
	 * Sends the signal after all filesFound() signals, this is the last signal sent.
	 *
	 * Guaranteed to be sent only once.
	 * Will be sent even if the playlist file cannot be loaded or stop() has been called.
	 *
	 * @param error error while parsing the playlist file
	 * @param timeElapsed time in milliseconds needed to perform the operation (load or save)
	 */
	void finished(PlaylistParser::Error error, int timeElapsed);

protected slots:

	void concurrentFinished();

protected:

	/**
	 * Gets the parser implementation given a playlist filename.
	 *
	 * The playlist parser implementation is choosen depending on the file extension.
	 * Example: if file extension is ".m3u" then M3UParser will be instantiated.
	 *
	 * Code factorization: returns nothing, instead sets _parser variable to the right
	 * parser implementation or NULL if none could be find given the playlist filename.
	 *
	 * @param fileName playlist filename/location
	 */
	void findParser(const QString & fileName);

	/** List of all available parsers. */
	QList<IPlaylistParserImpl *> _parserList;

	/** Current selected parser. */
	IPlaylistParserImpl * _parser;

	/** Error for the finished() signal. */
	Error _error;

	/** QFile used to read/write the playlist file. */
	QFile _file;

	/** Computes the time needed to parser the playlist file. */
	QTime _timeElapsed;
};

class MediaInfo;

class QIODevice;

/**
 * Loads a playlist file.
 *
 * @see PlaylistWriter
 * @author Tanguy Krotoff
 */
class PLAYLISTPARSER_API PlaylistReader : public PlaylistParser {
	Q_OBJECT
public:

	PlaylistReader(QObject * parent);

	~PlaylistReader();

	/**
	 * Loads the playlist.
	 *
	 * The playlist parser implementation is choosen depending on the file extension.
	 * Example: if file extension is ".m3u" then M3UParser will be instantiated.
	 *
	 * @param fileName playlist file (full path) or URL to load
	 * @see filesFound()
	 */
	void load(const QString & fileName);

signals:

	/**
	 * Sends the signal every FILES_FOUND_LIMIT files found.
	 *
	 * @param files list of files (full path filename) contained inside the playlist
	 */
	void filesFound(const QList<MediaInfo> & files);

private:

	void loadIODevice(QIODevice * device, const QString & fileName);
};

/**
 * Saves a playlist file.
 *
 * @see PlaylistReader
 * @author Tanguy Krotoff
 */
class PLAYLISTPARSER_API PlaylistWriter : public PlaylistParser {
public:

	PlaylistWriter(QObject * parent);

	~PlaylistWriter();

	/**
	 * Saves the playlist.
	 *
	 * The playlist parser implementation is choosen depending on the file extension.
	 * Example: if file extension is ".m3u" then M3UParser will be instantiated.
	 *
	 * @param fileName playlist file (full path)
	 * @param files files to add to the playlist file
	 */
	void save(const QString & fileName, const QList<MediaInfo> & files);

private:

	void saveIODevice(QIODevice * device, const QString & fileName, const QList<MediaInfo> & files);

};

#endif	//PLAYLISTPARSER_H
