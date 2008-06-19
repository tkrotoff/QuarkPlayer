/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "FileExtensions.h"

QStringList FileExtensions::video() {
	QStringList list;
	list << "avi" << "vfw" << "divx"
		<< "mpg" << "mpeg" << "m1v" << "m2v" << "mpv" << "dv" << "3gp"
		<< "mov" << "mp4" << "m4v" << "mqv"
		<< "dat" << "vcd"
		<< "ogg" << "ogm"
		<< "asf" << "wmv"
		<< "bin" << "iso" << "vob"
		<< "mkv" << "nsv" << "ram" << "flv"
		<< "rm" << "swf"
		<< "ts" << "rmvb" << "dvr-ms" << "m2t";
	return list;
}

QStringList FileExtensions::audio() {
	QStringList list;
	list << "mp3" << "ogg" << "wav" << "wma" <<  "ac3" << "ra" << "ape" << "flac";
	return list;
}

QStringList FileExtensions::multimedia() {
	QStringList multimedia(video());
	foreach (QString tmp, audio()) {
		if (!multimedia.contains(tmp)) {
			multimedia << tmp;
		}
	}
	return multimedia;
}

QStringList FileExtensions::subtitle() {
	QStringList list;
	list << "srt" << "sub" << "ssa" << "ass" << "idx" << "txt" << "smi"
		<< "rt" << "utf" << "aqt";
	return list;
}

QStringList FileExtensions::playlist() {
	QStringList list;
	list << "m3u" << "m3u8" << "pls";
	return list;
}

QString FileExtensions::toFilterFormat(const QStringList & extensions) {
	QString tmp;
	tmp += " (";
	foreach (QString ext, extensions) {
		tmp += "*." + ext + " ";
	}
	tmp += ")";
	return tmp;
}
