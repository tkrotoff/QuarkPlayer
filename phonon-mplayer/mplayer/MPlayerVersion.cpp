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

#include "MPlayerVersion.h"

#include <QtCore/QRegExp>

int MPlayerVersion::parseSvnVersion(QString version) {
	//static QRegExp rx_mplayer_revision("^MPlayer (\\S+)-SVN-r(\\d+)-(.*)");
	static QRegExp rx_mplayer_revision("^MPlayer (.*)-r(\\d+)(.*)");
	static QRegExp rx_mplayer_version("^MPlayer ([a-z,0-9,.]+)-(.*)");
#ifndef Q_OS_WIN
	static QRegExp rx_mplayer_version_ubuntu("^MPlayer (\\d):(\\d)\\.(\\d)~(.*)");
#endif

	int mplayer_svn = 0;

#ifdef Q_OS_WIN
	// Hack to recognize mplayer 1.0rc2 from CCCP:
	if (version.startsWith("MPlayer CCCP ")) { 
		version.remove("CCCP "); 
		qDebug("MPlayerVersion::parseSvnVersion: removing CCCP: '%s'", version.toUtf8().data()); 
	}
#else
	// Hack to recognize mplayer 1.0rc1 from Ubuntu:
	if (rx_mplayer_version_ubuntu.indexIn(version) > -1) {
		int v1 = rx_mplayer_version_ubuntu.cap(2).toInt();
		int v2 = rx_mplayer_version_ubuntu.cap(3).toInt();
		QString rest = rx_mplayer_version_ubuntu.cap(4);
		//qDebug("%d - %d - %d", rx_mplayer_version_ubuntu.cap(1).toInt(), v1 , v2);
		version = QString("MPlayer %1.%2%3").arg(v1).arg(v2).arg(rest);
		qDebug("MPlayerVersion::parseSvnVersion: line converted to '%s'", version.toUtf8().data());
	}
#endif

	if (rx_mplayer_revision.indexIn(version) > -1) {
		mplayer_svn = rx_mplayer_revision.cap(2).toInt();
		qDebug("MPlayerVersion::parseSvnVersion: MPlayer SVN revision found: %d", mplayer_svn);
	} 
	else
	if (rx_mplayer_version.indexIn(version) > -1) {
		QString version = rx_mplayer_version.cap(1);
		qDebug("MPlayerVersion::parseSvnVersion: MPlayer version found: %s", version.toUtf8().data());
		mplayer_svn = 0;
		if (version == "1.0rc2") mplayer_svn = MPLAYER_1_0_RC2_SVN;
		else
		if (version == "1.0rc1") mplayer_svn = MPLAYER_1_0_RC1_SVN;
		else qWarning("MPlayerVersion::parseSvnVersion: unknown MPlayer version");
	}

	return mplayer_svn;
}
