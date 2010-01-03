/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MsgHandler.h"

#include <QtCore/QtGlobal>

MsgHandler::MsgHandler() {
}

MsgHandler::~MsgHandler() {
}

void MsgHandler::myMessageOutput(QtMsgType type, const char * msg) {
	QString tmp(msg);

	//Help recognize the real MPlayer output from the other debug messages
	static const char * MPLAYER_LOG = "MPLAYER";
	if (tmp.startsWith(MPLAYER_LOG)) {
		tmp = tmp.trimmed();
		tmp.remove(0, QString(MPLAYER_LOG).length());
		tmp += "\n";
		MsgHandler::instance().mplayerLog += tmp;
		emit MsgHandler::instance().mplayerLogLineAvailable(tmp);
	}

	else {
		qInstallMsgHandler(NULL);

		//Back to the default Qt message handler
		//This is an internal function from Qt
		//Read file C:\Qt\4.6.0\src\corelib\global\qglobal.cpp
		qt_message_output(type, msg);

		qInstallMsgHandler(MsgHandler::myMessageOutput);
	}
}
