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

#include "PlaylistParserTest.h"

#include <playlistparser/PlaylistParser.h>

QTEST_MAIN(PlaylistParserTest)

void PlaylistParserTest::initTestCase() {
}

void PlaylistParserTest::cleanupTestCase() {
}

void PlaylistParserTest::init() {
}

void PlaylistParserTest::cleanup() {
}

void PlaylistParserTest::loadM3U() {
	PlaylistParser * playlistParser = new PlaylistParser("foobar2000-0.9.6.7/test.m3u", this);
	playlistParser->load();
}

void PlaylistParserTest::loadPLS() {
}

void PlaylistParserTest::loadWPL() {
}

void PlaylistParserTest::loadASX() {
}

void PlaylistParserTest::loadXSPF() {
}

void PlaylistParserTest::loadCUE() {
}

void PlaylistParserTest::saveM3U() {
}

void PlaylistParserTest::savePLS() {
}

void PlaylistParserTest::saveWPL() {
}

void PlaylistParserTest::saveASX() {
}

void PlaylistParserTest::saveXSPF() {
}

void PlaylistParserTest::saveCUE() {
}