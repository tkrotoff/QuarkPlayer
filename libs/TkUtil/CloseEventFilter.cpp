/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2004-2007  Wengo
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "CloseEventFilter.h"

#include <QtCore/QEvent>

CloseEventFilter::CloseEventFilter(QObject * receiver, const char * member, bool filter)
	: EventFilter(receiver, member, filter) {
}

bool CloseEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::Close) {
		return filter(event);
	}
	return EventFilter::eventFilter(watched, event);
}
