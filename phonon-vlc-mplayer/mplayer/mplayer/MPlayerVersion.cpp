/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <QtCore/QRegExp>
#include <QtCore/QtDebug>

int MPlayerVersion::parse(QString & version) {
	//static QRegExp rx_mplayer_revision("^MPlayer (\\S+)-SVN-r(\\d+)-(.*)");
	static QRegExp rx_mplayer_revision("^MPlayer (.*)-r(\\d+)(.*)");
	static QRegExp rx_mplayer_version("^MPlayer ([a-z,0-9,.]+)-(.*)");

#ifndef Q_OS_WIN
	static QRegExp rx_mplayer_version_ubuntu("^MPlayer (\\d):(\\d)\\.(\\d)~(.*)");
#endif	//!Q_OS_WIN

	int revision = 0;

#ifdef Q_OS_WIN
	//Hack to recognize MPlayer 1.0rc2 from CCCP
	//See http://en.wikipedia.org/wiki/Combined_Community_Codec_Pack
	//for more informations about CCCP
	if (version.startsWith("MPlayer CCCP ")) {
		version.remove("CCCP ");
		qDebug() << __FUNCTION__ << "Removing CCCP from MPlayer version:" << version;
	}
#else
	//Hack to recognize MPlayer 1.0rc1 from Ubuntu
	if (rx_mplayer_version_ubuntu.indexIn(version) > -1) {
		int v1 = rx_mplayer_version_ubuntu.cap(2).toInt();
		int v2 = rx_mplayer_version_ubuntu.cap(3).toInt();
		QString rest = rx_mplayer_version_ubuntu.cap(4);
		version = QString("MPlayer %1.%2%3").arg(v1).arg(v2).arg(rest);
		qDebug() << __FUNCTION__ << "line converted to:" << version;
	}
#endif	//Q_OS_WIN

	if (rx_mplayer_revision.indexIn(version) > -1) {
		revision = rx_mplayer_revision.cap(2).toInt();
		qDebug() << __FUNCTION__ << "MPlayer SVN revision:" << revision;
	}

	else if (rx_mplayer_version.indexIn(version) > -1) {
		version = rx_mplayer_version.cap(1);
		qDebug() << __FUNCTION__ << "MPlayer version:" << version;
		if (version == "1.0rc2") {
			revision = MPLAYER_1_0_RC2_SVN;
		}
		else if (version == "1.0rc1") {
			revision = MPLAYER_1_0_RC1_SVN;
		}
		else {
			qCritical() << "Error: unknown MPlayer version";
		}
	}

	return revision;
}
