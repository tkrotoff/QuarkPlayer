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

#ifndef VERSION_H
#define VERSION_H

#include <quarkplayer/quarkplayer_export.h>

class QString;

/**
 * QuarkPlayer version string: "major.minor.patch". E.g. "1.2.1"
 */
QUARKPLAYER_API extern const char * QUARKPLAYER_VERSION;

/**
 * QuarkPlayer Subversion revision number.
 */
QUARKPLAYER_API extern const int QUARKPLAYER_SVN_REVISION;

/**
 * Compiler used (GCC, MSVC...)
 */
QUARKPLAYER_API extern const char * QUARKPLAYER_COMPILER;

/**
 * System/OS used (Win32, Linux, MacOS...)
 */
QUARKPLAYER_API extern const char * QUARKPLAYER_SYSTEM;

/**
 * Build type: Release, Debug, RelWithDebInfo...
 */
QUARKPLAYER_API extern const char * QUARKPLAYER_BUILD_TYPE;

/**
 * Build date.
 */
QUARKPLAYER_API extern const char * QUARKPLAYER_BUILD_DATE;

/**
 * Returns a formatted string with full version description.
 */
QUARKPLAYER_API QString quarkPlayerFullVersion();

#endif	//VERSION_H
