/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2008 Ricardo Villalba <rvm@escomposlinux.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MPLAYERVERSION_H
#define MPLAYERVERSION_H

#include <QtCore/QString>

#define MPLAYER_1_0_RC1_SVN 20372
#define MPLAYER_1_0_RC2_SVN 24722

class MPlayerVersion {
public:

	//! Parses the mplayer svn version from the string and returns it.
	//! If the parsing fails, returns 0
	static int parseSvnVersion(QString version);

private:

	MPlayerVersion();
	~MPlayerVersion();
};

#endif	//MPLAYERVERSION_H
