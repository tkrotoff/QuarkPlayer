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

#include "MouseEventFilter.h"

#include <QtCore/QEvent>
#include <QtCore/QDebug>

#include <QtGui/QMouseEvent>

MouseMoveEventFilter::MouseMoveEventFilter(QObject * receiver, const char * member, bool filter)
	: EventFilter(receiver, member, filter) {
}

bool MouseMoveEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::MouseMove) {
		return filter(event);
	}
	return EventFilter::eventFilter(watched, event);
}


MousePressEventFilter::MousePressEventFilter(QObject * receiver, const char * member, Qt::MouseButton button, bool filter)
	: EventFilter(receiver, member, filter),
	_button(button) {
}

bool MousePressEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::MouseButtonPress, false) {
		QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

		if ((_button == Qt::NoButton) || (mouseEvent->button() == _button)) {
			return filter(event);
		}
	}
	return EventFilter::eventFilter(watched, event);
}


MouseReleaseEventFilter::MouseReleaseEventFilter(QObject * receiver, const char * member, Qt::MouseButton button, bool filter)
	: EventFilter(receiver, member, filter),
	_button(button) {
}

bool MouseReleaseEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

		if ((_button == Qt::NoButton) || (mouseEvent->button() == _button)) {
			return filter(event);
		}
	}
	return EventFilter::eventFilter(watched, event);
}


MouseEnterEventFilter::MouseEnterEventFilter(QObject * receiver, const char * member, bool filter)
	: EventFilter(receiver, member, false) {
}

bool MouseEnterEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::Enter) {
		return filter(event);
	}
	return EventFilter::eventFilter(watched, event);
}


MouseLeaveEventFilter::MouseLeaveEventFilter(QObject * receiver, const char * member, bool filter)
	: EventFilter(receiver, member, filter) {
}

bool MouseLeaveEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::Leave) {
		return filter(event);
	}
	return EventFilter::eventFilter(watched, event);
}
