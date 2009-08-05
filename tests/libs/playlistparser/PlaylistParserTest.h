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

#include <QtTest/QtTest>

/**
 * Test for playlistparser library.
 *
 * @see PlaylistParser
 * @author Tanguy Krotoff
 */
class PlaylistParserTest public QObject {
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

	void loadM3U();
	void loadPLS();
	void loadWPL();
	void loadASX();
	void loadXSPF();
	void loadCUE();

	void saveM3U();
	void savePLS();
	void saveWPL();
	void saveASX();
	void saveXSPF();
	void saveCUE();
};

#endif	//PLAYLISTPARSERTEST_H
