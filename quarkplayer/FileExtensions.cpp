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

QStringList FileExtensions::_videoExtensions;
QStringList FileExtensions::_audioExtensions;
QStringList FileExtensions::_multimediaExtensions;
QStringList FileExtensions::_subtitleExtensions;
QStringList FileExtensions::_playlistExtensions;

QStringList FileExtensions::video() {
	if (_videoExtensions.isEmpty()) {
		_videoExtensions << "avi" << "vfw" << "divx"
			<< "mpg" << "mpeg" << "m1v" << "m2v" << "mpv" << "dv" << "3gp"
			<< "mov" << "mp4" << "m4v" << "mqv"
			<< "dat" << "vcd"
			<< "ogg" << "ogm"
			<< "asf" << "wmv"
			<< "bin" << "iso" << "vob"
			<< "mkv" << "nsv" << "ram" << "flv"
			<< "rm" << "swf"
			<< "ts" << "rmvb" << "dvr-ms" << "m2t";
	}
	return _videoExtensions;
}

QStringList FileExtensions::audio() {
	if (_audioExtensions.isEmpty()) {
		_audioExtensions << "mp3" << "ogg" << "wav" << "wma" <<  "ac3" << "ra" << "ape" << "flac";
	}
	return _audioExtensions;
}

QStringList FileExtensions::multimedia() {
	if (_multimediaExtensions.isEmpty()) {
		_multimediaExtensions << video();
		foreach (QString tmp, audio()) {
			if (!_multimediaExtensions.contains(tmp)) {
				_multimediaExtensions << tmp;
			}
		}
	}
	return _multimediaExtensions;
}

QStringList FileExtensions::subtitle() {
	if (_subtitleExtensions.isEmpty()) {
		_subtitleExtensions << "srt" << "sub" << "ssa" << "ass" << "idx" << "txt" << "smi"
			<< "rt" << "utf" << "aqt";
	}
	return _subtitleExtensions;
}

QStringList FileExtensions::playlist() {
	if (_playlistExtensions.isEmpty()) {
		_playlistExtensions << "m3u" << "m3u8" << "pls";
	}
	return _playlistExtensions;
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
