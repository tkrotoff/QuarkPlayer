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

#include "Win32Util.h"

#include "TkUtilLogger.h"

#include <windows.h>

QString Win32Util::GetLastErrorToString(unsigned long lastError) {
	//Retrieving the Last-Error Code: http://msdn.microsoft.com/en-us/library/ms680582.aspx

	//See Chromium' use of FormatMessage: http://src.chromium.org/viewvc/chrome/trunk/src/base/win_util.cc

	wchar_t * message = NULL;

	//FormatMessage returns a message localized
	//There is no way to force to get the english message, fucking Microsoft :/
	unsigned messageLength = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		lastError,
		0,
		reinterpret_cast<wchar_t *>(&message),
		0,
		NULL
	);

	QString str = "GetLastError: " + QString::number(lastError);

	if (messageLength) {
		QString tmp = QString::fromUtf16(reinterpret_cast<const unsigned short *>(message));

		//Win32 API adds CRLF at the end of the string
		tmp = tmp.trimmed();

		str += ", message: " + tmp;

		LocalFree(message);
	} else {
		//FormatMessage() failed
	}

	return str;
}
