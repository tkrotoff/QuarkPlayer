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

#ifndef MOUSEEVENTFILTER_H
#define MOUSEEVENTFILTER_H

#include <tkutil/EventFilter.h>

/**
 * Catch MouseMove event.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API MouseMoveEventFilter : public EventFilter {
public:

	MouseMoveEventFilter(QObject * receiver, const char * member, bool filter = false);

private:

	bool eventFilter(QObject * watched, QEvent * event);
};


/**
 * Catch MouseButtonPress event.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API MousePressEventFilter : public EventFilter {
public:

	MousePressEventFilter(QObject * receiver, const char * member, Qt::MouseButton button = Qt::NoButton, bool filter = false);

private:

	bool eventFilter(QObject * watched, QEvent * event);

	Qt::MouseButton _button;
};


/**
 * Catch MouseButtonRelease event.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API MouseReleaseEventFilter : public EventFilter {
public:

	MouseReleaseEventFilter(QObject * receiver, const char * member, Qt::MouseButton button = Qt::NoButton, bool filter = false);

private:

	bool eventFilter(QObject * watched, QEvent * event);

	Qt::MouseButton _button;
};


/**
 * Catch Enter event.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API MouseEnterEventFilter : public EventFilter {
public:

	MouseEnterEventFilter(QObject * receiver, const char * member, bool filter = false);

private:

	bool eventFilter(QObject * watched, QEvent * event);
};


/**
 * Catch Leave event.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API MouseLeaveEventFilter : public EventFilter {
public:

	MouseLeaveEventFilter(QObject * receiver, const char * member, bool filter = false);

private:

	bool eventFilter(QObject * watched, QEvent * event);
};

#endif	//MOUSEEVENTFILTER_H
