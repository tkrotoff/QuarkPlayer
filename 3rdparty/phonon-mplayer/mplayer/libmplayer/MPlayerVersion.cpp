/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2007-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MPlayerVersion.h"

//For MPLAYER_VERSION_NOTFOUND define
#include "MPlayerProcess.h"

#include "LibMPlayerLogger.h"

#include <QtCore/QRegExp>

namespace Phonon
{
namespace MPlayer
{

static const int MPLAYER_1_0_RC1_SVN = 20372;
static const int MPLAYER_1_0_RC2_SVN = 24722;
static const int MPLAYER_1_0_RC3_SVN = 31271;
static const int MPLAYER_VERSION_FAILED = 0;

int MPlayerVersion::parse(const QString & line) {
	//Examples:
	//
	//Official 1.0rc1 release 2006-10-22:
	//MPlayer 1.0rc1-4.1.2 (C) 2000-2006 MPlayer Team
	//
	//Windows, official 1.0rc2 release 2007-10-07:
	//MPlayer 1.0rc2-4.2.1 (C) 2000-2007 MPlayer Team
	//MPlayer 1.0rc2-4.2.3 (C) 2000-2007 MPlayer Team
	//
	//Ubuntu 8.10:
	//MPlayer 1.0rc2-4.3.2 (C) 2000-2007 MPlayer Team
	//
	//Windows, rvm build:
	//MPlayer dev-SVN-r27130-3.4.5 (with -volume) (C) 2000-2008 MPlayer Team
	//
	//Ubuntu 8.10, rvm build:
	//MPlayer dev-SVN-r27900-4.3.2 (C) 2000-2008 MPlayer Team
	//
	//Windows, http://sourceforge.net/projects/mplayer-win32/ build:
	//MPlayer Sherpya-SVN-r28126-4.2.5 (C) 2000-2008 MPlayer Team
	//
	//openSUSE 11.1:
	//MPlayer dev-SVN-r27637-4.3-openSUSE Linux 11.1 (i686)-Packman (C) 2000-2008 MPlayerTeam
	//
	//Windows, http://sourceforge.net/projects/mplayer-win32/ build:
	//MPlayer Sherpya-SVN-r28311-4.2.5 (C) 2000-2009 MPlayer Team
	//
	//Ubuntu 9.10:
	//MPlayer SVN-r29237-4.4.1 (C) 2000-2009 MPlayer Team
	//
	//Red Hat Enterprise Linux 5.1, official 1.0rc3 release 2010-05-30:
	//MPlayer 1.0rc3-4.1.2 (C) 2000-2009 MPlayer Team


	static QRegExp rx_mplayer_revision("^MPlayer (.*)-r(\\d+)(.*)");
	static QRegExp rx_mplayer_version("^MPlayer ([a-z,0-9,.]+)-(.*)");

	QString version(line);
	int revision = MPLAYER_VERSION_FAILED;

	if (rx_mplayer_revision.indexIn(version) > -1) {
		revision = rx_mplayer_revision.cap(2).toInt();
		LibMPlayerDebug() << "MPlayer SVN revision:" << revision;
	}

	else if (rx_mplayer_version.indexIn(version) > -1) {
		version = rx_mplayer_version.cap(1);
		LibMPlayerDebug() << "MPlayer version:" << version;
		if (version == "1.0rc1") {
			revision = MPLAYER_1_0_RC1_SVN;
		}
		else if (version == "1.0rc2") {
			revision = MPLAYER_1_0_RC2_SVN;
		}
		else if (version == "1.0rc3") {
			revision = MPLAYER_1_0_RC3_SVN;
		}
		else {
			LibMPlayerCritical() << "Unknown MPlayer version";
		}
	}

	if (revision == MPLAYER_VERSION_FAILED) {
		LibMPlayerCritical() << "Couldn't parse MPlayer revision:" << version;
	}

	return revision;
}

}}	//Namespace Phonon::MPlayer
