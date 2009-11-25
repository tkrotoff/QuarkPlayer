/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2004-2007  Wengo
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

#ifndef KEYEVENTFILTER_H
#define KEYEVENTFILTER_H

#include <tkutil/EventFilter.h>

/**
 * Catch KeyPress event.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class TKUTIL_API KeyPressEventFilter : public EventFilter {
public:

	KeyPressEventFilter(QObject * receiver, const char * member, Qt::Key key, bool filter = false);

private:

	bool eventFilter(QObject * watched, QEvent * event);

	Qt::Key _key;
};


/**
 * Catch KeyRelease event.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class TKUTIL_API KeyReleaseEventFilter : public EventFilter {
public:

	KeyReleaseEventFilter(QObject * receiver, const char * member, Qt::Key key, bool filter = false);

private:

	bool eventFilter(QObject * watched, QEvent * event);

	Qt::Key _key;
};

#endif	//KEYEVENTFILTER_H
