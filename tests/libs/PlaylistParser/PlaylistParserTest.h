/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef PLAYLISTPARSERTEST_H
#define PLAYLISTPARSERTEST_H

#include <PlaylistParser/PlaylistParser.h>

#include <QtTest/QtTest>
#include <QtCore/QList>

class MediaInfo;

/**
 * Test for playlistparser library.
 *
 * Playlist files made with free albums from jamendo:
 *
 * - Nine Inch Nails (NIN) - Ghosts I–IV
 *   http://ghosts.nin.com/
 *   - FLAC lossless (259 mb)
 *     http://dl.nin.com/data/dl/Nine_Inch_Nails_-_The_Slip_-_Flac.torrent
 *   - FLAC high definition 24/96 (942 mb)
 *     http://dl.nin.com/data/dl/Nine_Inch_Nails_-_The_Slip_24-96k_Flac.torrent
 *   - M4A apple lossless (263 mb)
 *     http://dl.nin.com/data/dl/Nine_Inch_Nails_-_The_Slip_-_Apple_Lossless.torrent
 *   - High definition WAVE 24/96 (1.5 gb)
 *     http://dl.nin.com/data/dl/Nine_Inch_Nails_-_The_Slip_-_Wave_96-24_High_Res.torrent
 *
 * - Nine Inch Nails (NIN) - The Slip http://theslip.nin.com/
 *
 * In order to test file names with special characters:
 *
 * - David TMX - fournée d'automne
 *   - MP3
 *   http://imgjam.com/torrents/album/795/4795/4795-mp32.torrent/David%20TMX%20-%20fournee%20d%27automne%20--%20Jamendo%20-%20MP3%20VBR%20192k%20-%202007.03.23%20%5Bwww.jamendo.com%5D.torrent
 *   - OGG
 *   http://imgjam.com/torrents/album/795/4795/4795-ogg3.torrent/David%20TMX%20-%20fournee%20d%27automne%20--%20Jamendo%20-%20OGG%20Vorbis%20q7%20-%202007.03.23%20%5Bwww.jamendo.com%5D.torrent
 *
 * - Bézèd'h - Ton jour viendra
 *   - MP3
 *   http://imgjam.com/torrents/album/135/135/135-mp32.torrent/Bezed%27h%20-%20Ton%20jour%20viendra%20--%20Jamendo%20-%20MP3%20VBR%20192k%20-%202005.04.28%20%5Bwww.jamendo.com%5D.torrent
 *   - OGG
 *   http://imgjam.com/torrents/album/135/135/135-ogg3.torrent/Bezed%27h%20-%20Ton%20jour%20viendra%20--%20Jamendo%20-%20OGG%20Vorbis%20q7%20-%202005.04.28%20%5Bwww.jamendo.com%5D.torrent
 *
 * French radio list:
 * http://code.google.com/p/radioshell/wiki/ListeRadios
 * http://flux.radio.free.fr/
 * http://www.infomaniak.ch/societe/streaming.php
 *
 * @see PlaylistParser
 * @author Tanguy Krotoff
 */
class PlaylistParserTest : public QObject {
	Q_OBJECT
private slots:

	/** Called before the first testfunction is executed. */
	void initTestCase();

	/** Called after the last testfunction was executed. */
	void cleanupTestCase();

	/** Called before each testfunction is executed. */
	void init();

	/** Called after every testfunction. */
	void cleanup();


	void loadPlaylist_data();

	void loadPlaylist();

	void filesFound(const QList<MediaInfo> & files);

	void finished(PlaylistParser::Error error, int timeElapsed);

private:

	PlaylistReader * _parser;
};

#endif	//PLAYLISTPARSERTEST_H
