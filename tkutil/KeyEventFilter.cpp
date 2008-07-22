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

#include "KeyEventFilter.h"

#include <QtCore/QEvent>

#include <QtGui/QKeyEvent>

KeyPressEventFilter::KeyPressEventFilter(QObject * receiver, const char * member, Qt::Key key, bool filter)
	: EventFilter(receiver, member, filter),
	_key(key) {
}

bool KeyPressEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);

		if (keyEvent->key() == _key) {
			return filter(event);
		}
	}
	return EventFilter::eventFilter(watched, event);
}

KeyReleaseEventFilter::KeyReleaseEventFilter(QObject * receiver, const char * member, Qt::Key key, bool filter)
	: EventFilter(receiver, member, filter),
	_key(key) {
}

bool KeyReleaseEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::KeyRelease) {
		QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);

		if (keyEvent->key() == _key) {
			return filter(event);
		}
	}
	return EventFilter::eventFilter(watched, event);
}
