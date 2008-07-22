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

#include "DropEventFilter.h"

#include <QtCore/QEvent>

#include <QtGui/QDropEvent>

DropEventFilter::DropEventFilter(QObject * receiver, const char * member, bool filter)
	: EventFilter(receiver, member, filter) {
}

bool DropEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::Drop) {
		return filter(event);
	}
	return EventFilter::eventFilter(watched, event);
}


DragEnterEventFilter::DragEnterEventFilter(QObject * receiver, const char * member, bool filter)
	: EventFilter(receiver, member, filter) {
}

bool DragEnterEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::DragEnter) {
		return filter(event);
	}
	return EventFilter::eventFilter(watched, event);
}


DragMoveEventFilter::DragMoveEventFilter(QObject * receiver, const char * member, bool filter)
	: EventFilter(receiver, member, filter) {
}

bool DragMoveEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::DragMove) {
		return filter(event);
	}
	return EventFilter::eventFilter(watched, event);
}
